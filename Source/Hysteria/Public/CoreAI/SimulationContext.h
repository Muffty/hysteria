#pragma once

#include "Types.h"
#include <array>
#include <vector>
#include <optional>

template <int W, int H, int N_AGENTS>
struct FSimulationContext
{
	const std::array<std::vector<FAgentAction>, N_AGENTS>* AgentTrajectories = nullptr;
	uint8_t GlobalTurn = 0;
	std::array<bool, N_AGENTS>* AgentIsWaitingTemporarily = {};

	FAgentAction GetPlannedActionForAgent(int AgentIdx, int TurnIndex)
	{
		if (!AgentTrajectories || AgentIdx < 0 || AgentIdx >= N_AGENTS)
			return FAgentAction(EActionType::Wait);

		if (AgentIsWaitingTemporarily[AgentIdx])
			return FAgentAction(EActionType::Wait);

		const auto& traj = (*AgentTrajectories)[AgentIdx];
		int offset = TurnIndex - GlobalTurn;
		if (offset >= 0 && offset < traj.size())
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