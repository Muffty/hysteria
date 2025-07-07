// FMultiAgentMCTS.h

#pragma once

#include "WorldState.h"
#include "SearchTree.h"
#include "Types.h"
#include "SimulationContext.h"
#include <array>
#include <vector>
#include <optional>


template <int W, int H, int N_AGENTS>
class FMultiAgentMCTS
{
public:
	using FWorldState = WorldState<W, H, N_AGENTS>;
	using FSimContext = FSimulationContext<W, H, N_AGENTS>;

	explicit FMultiAgentMCTS(const FWorldState& InitialState)
	{
		CurrentState = InitialState;

		SimulationContext = FSimContext();
		SimulationContext.AgentTrajectories.fill(std::vector<FAgentAction>());
		SimulationContext.AgentIsWaitingTemporarily.fill(false);
		SimulationContext.GlobalTurn = InitialState.turnCounter;

		for (int i = 0; i < N_AGENTS; ++i)
		{
			AgentTrees[i] = FMCTS<W, H, N_AGENTS>(InitialState, i);
		}
	}

	//Performs a single step of the MCTS process for all agents.
	std::array<FAgentAction, N_AGENTS> Step()
	{
		//TODO: for each agent, measure how long their trajectory is still valid and order the execution accordingly.
		std::vector<int> agentOrder(N_AGENTS, 0);
		std::array<FAgentAction, N_AGENTS> Actions;

		for (int i = 0; i < N_AGENTS; ++i)
		{
			int AgentIndex = agentOrder[i];
			auto tree = AgentTrees[AgentIndex];
			tree.RunSearch(numThreads, totalRollouts, SimulationContext);
			Actions[AgentIndex] = tree.GetBestAction();
		}

		// Apply joint actions to world
		CurrentState = CurrentState.NextState(Actions);

		// Extract the best trajectory for each action
		for (int i = 0; i < N_AGENTS; ++i)
		{
			auto& tree = AgentTrees[i];
			SimulationContext.AgentTrajectories[i] = tree.GetBestTrajectory();
		}

		return Actions;
	}

private:
	std::array<FMCTS<W, H, N_AGENTS>, N_AGENTS> AgentTrees;
	FSimContext SimulationContext;
	FWorldState CurrentState;
	int numThreads = 4;
	int totalRollouts = 1000;
};