#pragma once
#include <atomic>
#include <cmath>
#ifdef HYSTERIA_USE_UNREAL
#include "Async/Async.h"
#include "HAL/CriticalSection.h"
#else
#include <mutex>
#include <random>
#endif
#include <algorithm>
#include "WorldState.h"
#include "Types.h"

#ifdef HYSTERIA_USE_UNREAL
template<typename TCallable>
struct FLambdaWorker
{
	TCallable Job;
	bool isDone = false;
	FLambdaWorker(TCallable InJob) : Job(InJob) {}

	void DoWork()
	{
		Job();
		isDone = true;		
	}
	bool CanAbandon() const { return false; }
	void Abandon() {}
	bool IsDone() const { return isDone; }
	TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FLambdaWorker, STATGROUP_ThreadPoolAsyncTasks); }

};
#endif

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
	HYSTERIA_VECTOR<FMCTSNode*> children;

	// Expansion guard
#ifdef HYSTERIA_USE_UNREAL
	FCriticalSection ExpandMutex;
#else
	std::mutex ExpandMutex;	
#endif

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

	FMCTS() = default;

	~FMCTS()
	{
		// TODO: recursively delete entire tree
	}

	HYSTERIA_VECTOR<FAgentAction> GetBestTrajectory() const
	{
		// Collect the best trajectory from the root node
		HYSTERIA_VECTOR<FAgentAction> trajectory;
		FMCTSNode* node = Root;
#ifdef HYSTERIA_USE_UNREAL
		while (node && !node->children.IsEmpty())
#else
		while (node && !node->children.empty())
#endif
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
#ifdef HYSTERIA_USE_UNREAL
			trajectory.Add(bestChild->actionFromParent);
#else
			trajectory.push_back(bestChild->actionFromParent);
#endif
			node = bestChild;
		}
		return trajectory;
	}

	// Kick off numThreads running rollouts until totalRollouts are done
	void RunSearch(int numThreads, int totalRollouts, FSimContext InSimContext)
	{
		this->SimContext = InSimContext;

		
#ifdef HYSTERIA_USE_UNREAL
		// Unreal Engine uses Async tasks for multithreading
		auto MyJob = [&]()
		{
			int n = 0;
			while (n < totalRollouts)
			{
				Rollout();
				n++;
			}
		};
		
		TArray<FAsyncTask<FLambdaWorker<decltype(MyJob)>>*> Tasks;
		for (int i = 0; i < numThreads; ++i)
		{
			auto* Task = new FAsyncTask<FLambdaWorker<decltype(MyJob)>>(MyJob);
			Task->StartBackgroundTask();
			Tasks.Add(Task);
		}
		
		for (auto* Task : Tasks)
			Task->EnsureCompletion();
#else		
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
#endif
	}

	// After search, pick the action with highest (visit or blended) score
	FAgentAction GetBestAction() const
	{
		FMCTSNode* best = nullptr;
		double bestV = -1;
		for (auto* c : Root->children)
		{
			double v = c->currVisits.load();
			//Add a epsilon random value to avoid ties			
#ifdef HYSTERIA_USE_UNREAL
			v += 1e-6 * (FMath::Rand() % 1000); // Small random perturbation
#else
			v += 1e-6 * (rand() % 1000); // Small random perturbation
#endif
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
#ifdef HYSTERIA_USE_UNREAL
		FScopeLock Lock(&node->ExpandMutex);
#else
		std::lock_guard<std::mutex> lock(node->ExpandMutex);
#endif
		if (node->bExpanded) return;

		// list of legal FAgentAction from node’s state for this agent
		HYSTERIA_VECTOR<FAgentAction> actions = state.GetLegalActionsForAgent(agentNr);

		// Shuffle actions to avoid order bias
#ifdef HYSTERIA_USE_UNREAL
		for (int32 i = actions.Num() - 1; i > 0; i--) {
			int32 j = FMath::Floor(FMath::Rand() * (i + 1)) % actions.Num();
			FAgentAction temp = actions[i];
			actions[i] = actions[j];
			actions[j] = temp;
		}
#else
		static thread_local std::mt19937 rng(std::random_device{}());
		std::shuffle(actions.begin(), actions.end(), rng);
#endif

		for (auto& a : actions)
		{
			auto* child = new FMCTSNode(node, a);
#ifdef HYSTERIA_USE_UNREAL
			node->children.Add(child);
#else
			node->children.push_back(child);
#endif
		}
		node->bExpanded = true;
	}

	// Simulate a random playout from state
	double Simulate(const FWorldState& state)
	{
		FWorldState simState = state;
		for (int i = 0; i < 10; ++i)
		{
			HYSTERIA_VECTOR<FAgentAction> actions = simState.GetLegalActionsForAgent(agentNr);
#ifdef HYSTERIA_USE_UNREAL
			if (actions.IsEmpty()) break;
			FAgentAction action = actions[FMath::RandRange(0, actions.Num() - 1)];
#else
			if (actions.empty()) break;
			static thread_local std::mt19937 rng(std::random_device{}());
			std::uniform_int_distribution<size_t> dist(0, actions.size() - 1);
			FAgentAction action = actions[dist(rng)];
#endif
			simState.AgentTurnOverride(SimContext, agentNr, action);
		}
		return simState.agents[agentNr].score;
	}

	// Backpropagate reward
	static void Backpropagate(FMCTSNode* node, double reward)
	{
		while (node)
		{
			node->currVisits.fetch_add(1);
			atomic_add(node->currValue, reward);
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

	static inline void atomic_add(std::atomic<double>& atom, double value)
	{
		double old = atom.load();
		double desired;
		do
		{
			desired = old + value;
		}
		while (!atom.compare_exchange_weak(old, desired));
	}
};

#ifdef HYSTERIA_USE_UNREAL
// Define a worker struct
template <int W, int H, int N_AGENTS>
struct FMCTSWorker
{
	FMCTS<W, H, N_AGENTS>* Instance;
	int TotalRollouts;

	FMCTSWorker(FMCTS<W, H, N_AGENTS>* InInstance, int InTotalRollouts)
		: Instance(InInstance), TotalRollouts(InTotalRollouts) {}

	void DoWork()
	{
		int n = 0;
		while (n < TotalRollouts)
		{
			Instance->Rollout();
			n++;
		}
	}
};
#endif