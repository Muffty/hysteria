#pragma once

#include "Types.h"
#include <array>
#include <optional>

template <int W, int H, int N_AGENTS>
struct FSimulationContext
{
	std::array<HYSTERIA_VECTOR<FAgentAction>, N_AGENTS>* AgentTrajectories = nullptr;
	uint8_t GlobalTurn = 0;
	std::array<bool, N_AGENTS> AgentIsWaitingTemporarily = {};

	explicit FSimulationContext()
	{
		AgentTrajectories = new std::array<HYSTERIA_VECTOR<FAgentAction>, N_AGENTS>();
	}

	void SetTrajectory(int AgentIdx, const HYSTERIA_VECTOR<FAgentAction>& Trajectory)
	{
		// Sets the trajectory for the given agent.
		if (AgentIdx >= 0 && AgentIdx < N_AGENTS)
		{
			if (!AgentTrajectories)
			{
				AgentTrajectories = new std::array<HYSTERIA_VECTOR<FAgentAction>, N_AGENTS>();
			}
			(*AgentTrajectories)[AgentIdx] = Trajectory;
		}
	}

	FAgentAction GetPlannedActionForAgent(int AgentIdx, int TurnIndex)
	{
		if (!AgentTrajectories || AgentIdx < 0 || AgentIdx >= N_AGENTS)
			return FAgentAction(EActionType::Wait);

		if (AgentIsWaitingTemporarily[AgentIdx])
			return FAgentAction(EActionType::Wait);

		const HYSTERIA_VECTOR<FAgentAction>& traj = (*AgentTrajectories)[AgentIdx];
		int offset = TurnIndex - GlobalTurn;
#ifdef HYSTERIA_USE_UNREAL
		if (offset >= 0 && offset < traj.Num())
#else
		if (offset >= 0 && offset < traj.size())
#endif
			return traj[offset];
		return FAgentAction(EActionType::Wait);
	}

	void ResetTemporaryData()
	{
		for (int i = 0; i < N_AGENTS; ++i)
		{
			AgentIsWaitingTemporarily[i] = false;
		}
	}

	void SetAgentWaitingTemporarily(int AgentIdx, bool bWaiting)
	{
		if (AgentIdx >= 0 && AgentIdx < N_AGENTS)
			AgentIsWaitingTemporarily[AgentIdx] = bWaiting;
	}
};
