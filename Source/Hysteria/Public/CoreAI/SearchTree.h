#pragma once
#include <atomic>
#include <cmath>
#include <mutex>
#include <vector>
#include <random>
#include "WorldState.h"
#include "Types.h"

struct FMCTSNode
{
	// Statistics for this node
	std::atomic<int> currVisits{0};
	std::atomic<double> currValue{0.0};
	int pastVisits = 0;
	double pastValue = 0.0;

	std::atomic<int> virtualLoss{0};

	// Tree structure
	FAgentAction actionFromParent;
	FMCTSNode* parent = nullptr;
	std::vector<FMCTSNode*> children;

	// Expansion guard
	std::mutex ExpandMutex;
	bool bExpanded = false;

	FMCTSNode(FMCTSNode* InParent = nullptr, const FAgentAction InAction = {})
	{
		parent = InParent;
		actionFromParent = InAction;
	}
};

template <int W, int H, int N_AGENTS>
class FMCTS
{
	using FWorldState = WorldState<W, H, N_AGENTS>;
	using FSimContext = FSimulationContext<W, H, N_AGENTS>;

public:
	FMCTS(const FWorldState& InRootState, const int AgentNr) : RootState(InRootState)
	{
		agentNr = AgentNr;
		Root = new FMCTSNode(nullptr, FAgentAction{EActionType::Wait});
	}

	~FMCTS()
	{
		// TODO: recursively delete entire tree
	}

	std::vector<FAgentAction> GetBestTrajectory() const
	{
		// Collect the best trajectory from the root node
		std::vector<FAgentAction> trajectory;
		FMCTSNode* node = Root;
		while (node && !node->children.empty())
		{
			// Find the child with the highest visit count
			FMCTSNode* bestChild = nullptr;
			int bestVisits = -1;
			for (auto* child : node->children)
			{
				int visits = child->currVisits.load();
				if (visits > bestVisits)
				{
					bestVisits = visits;
					bestChild = child;
				}
			}
			if (!bestChild) break; // No children found
			trajectory.push_back(bestChild->actionFromParent);
			node = bestChild;
		}
		return trajectory;
	}

	// Kick off numThreads running rollouts until totalRollouts are done
	void RunSearch(int numThreads, int totalRollouts, FSimContext SimContext)
	{
		this->SimContext = SimContext;
		std::atomic<int> rolloutCount{0};

		auto Worker = [&]()
		{
			while (true)
			{
				int n = rolloutCount.fetch_add(1);
				if (n >= totalRollouts) break;
				Rollout();
			}
		};

		std::vector<std::thread> threads;
		for (int i = 0; i < numThreads; ++i)
			threads.emplace_back(Worker);
		for (auto& t : threads) t.join();
	}

	// After search, pick the action with highest (visit or blended) score
	FAgentAction GetBestAction() const
	{
		FMCTSNode* best = nullptr;
		int bestV = -1;
		for (auto* c : Root->children)
		{
			int v = c->currVisits.load();
			if (v > bestV)
			{
				bestV = v;
				best = c;
			}
		}
		return best ? best->actionFromParent : FAgentAction{EActionType::Wait};
	}

	// Warm-start: archive stats then reset curr* for next iteration
	void ArchiveAndResetStats() const
	{
		for (auto* c : Root->children)
		{
			c->pastVisits = c->currVisits.load();
			c->pastValue = c->currValue.load();
			c->currVisits = 0;
			c->currValue = 0.0;
		}
	}

private:
	FMCTSNode* Root;
	FWorldState RootState;
	int agentNr;
	FSimContext SimContext;

	// Single-rollout entry (Select→Expand→Simulate→Backprop)
	void Rollout()
	{
		// Clone the root state for simulation
		FWorldState simState = RootState.Clone();

		SimContext.ResetTemporaryData();
		
		// 1. Selection
		FMCTSNode* node = Root;
		while (node->bExpanded)
		{
			node = Select(node);
			simState.AgentTurnOverride(SimContext, agentNr, node->actionFromParent);
		}

		// 2. Expansion
		Expand(node, simState);

		// 3. Simulation
		double reward = Simulate(simState);

		// 4. Backpropagation
		Backpropagate(node, reward);
		SimContext.ResetTemporaryData();
	}

	// Thread-safe selection with virtual loss
	static FMCTSNode* Select(const FMCTSNode* node)
	{
		FMCTSNode* best = nullptr;
		double bestScore = -1e9;
		int parentVisits = node->currVisits.load();
		for (auto* child : node->children)
		{
			int v = child->currVisits.load();
			double q = GetBlendedValue(child);
			int vl = child->virtualLoss.load();
			double uct = (q - vl) / (1 + v)
				+ 1.4 * std::sqrt(std::log(parentVisits + 1) / (1 + v));
			if (uct > bestScore)
			{
				bestScore = uct;
				best = child;
			}
		}
		// Reserve
		best->virtualLoss.fetch_add(1);
		return best;
	}

	// Expand leaf by creating all child nodes
	void Expand(FMCTSNode* node, FWorldState& state)
	{
		std::lock_guard<std::mutex> lock(node->ExpandMutex);
		if (node->bExpanded) return;

		// list of legal FAgentAction from node’s state for this agent
		std::vector<FAgentAction> actions = state.GetLegalActionsForAgent(agentNr);

		for (auto& a : actions)
		{
			auto* child = new FMCTSNode(node, a);
			node->children.push_back(child);
		}
		node->bExpanded = true;
	}

	// Simulate a random playout from state
	static double Simulate(const FWorldState& state)
	{
		//TODO: implement a proper simulation strategy
		thread_local std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<> dist(0.0, 1.0);
		return dist(rng);
	}

	// Backpropagate reward
	static void Backpropagate(FMCTSNode* node, double reward)
	{
		while (node)
		{
			node->currVisits.fetch_add(1);
			node->currValue.fetch_add(reward);
			node->virtualLoss.fetch_sub(1);
			node = node->parent;
		}
	}

	// UCT score blending past & current values
	static double GetBlendedValue(const FMCTSNode* node)
	{
		int cv = node->currVisits.load();
		double cQ = (cv > 0 ? node->currValue.load() / cv : 0.0);
		int pv = node->pastVisits;
		double pQ = (pv > 0 ? node->pastValue / pv : 0.0);

		if (cv < pv / 10.0 && pv > 0)
		{
			double const alpha = static_cast<double>(cv) / (pv + 1);
			return alpha * cQ + (1 - alpha) * pQ;
		}
		return cQ;
	}
};
