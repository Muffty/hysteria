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
		: CurrentState(InitialState),
		  SimulationContext()
	{
		CurrentState = InitialState;

		SimulationContext = FSimContext();
		SimulationContext.GlobalTurn = InitialState.turnCounter;

		AgentTrees = std::vector<FMCTS<W, H, N_AGENTS>>(N_AGENTS);
		for (int i = 0; i < N_AGENTS; ++i)
		{
			AgentTrees[i] = FMCTS<W, H, N_AGENTS>(InitialState, i);
		}
	}

	FAgentAction GetPlannedActionForAgent(int Agent)
	{
		if (Agent < 0 || Agent >= N_AGENTS)
			return FAgentAction(EActionType::Wait);

		// Get the best action from the MCTS tree for the specified agent
		return AgentTrees[Agent].GetBestAction();
	}

	//Performs a single step of the MCTS process for all agents.
	std::array<FAgentAction, N_AGENTS> Step()
	{
		//TODO: for each agent, measure how long their trajectory is still valid and order the execution accordingly.
		std::vector<int> agentOrder(N_AGENTS, 0);
		for (int i = 0; i < N_AGENTS; ++i)
			agentOrder[i] = i;

		
		std::array<FAgentAction, N_AGENTS> Actions;

		for (int i = 0; i < N_AGENTS; ++i)
		{
			int AgentIndex = agentOrder[i];
			auto& tree = AgentTrees[AgentIndex];
			tree.RunSearch(numThreads, totalRollouts, SimulationContext);
			Actions[AgentIndex] = tree.GetBestAction();
		}

		// Apply joint actions to world
		CurrentState.NextState(Actions);

		// Extract the best trajectory for each action
		for (int i = 0; i < N_AGENTS; ++i)
		{
			auto& tree = AgentTrees[i];
			SimulationContext.SetTrajectory(i, tree.GetBestTrajectory());
		}
		
		// Reset each agent's tree to the new state
		for (int i = 0; i < N_AGENTS; ++i)
		{
			AgentTrees[i] = FMCTS<W, H, N_AGENTS>(CurrentState, i);
		}
		
		return Actions;
	}
	FWorldState& GetCurrentState()
	{
		return CurrentState;
	}
private:
	std::vector<FMCTS<W, H, N_AGENTS>> AgentTrees;
	FSimContext SimulationContext;
	FWorldState CurrentState;
	int numThreads = 4;
	int totalRollouts = 1000;
};
