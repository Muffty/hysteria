#pragma once
#include "Types.h"
#include "SimulationContext.h"
#include <vector>


template <int W, int H, int N_AGENTS>
struct WorldState
{
	CellType grid[H][W];
	ItemType items[H][W];
	AgentState agents[N_AGENTS];
	uint8_t turnCounter;

	int GetAgentCount() const
	{
		return N_AGENTS;
	}
	int GetWidth() const
	{
		return W;
	}
	int GetHeight() const
	{
		return H;
	}

	bool CanExecute(int agent, const FAgentAction& action)
	{
		switch (action.Type)
		{
		case EActionType::MoveUp:
			if (agents[agent].y > 0 && grid[agents[agent].y - 1][agents[agent].x] == CellType::Empty)
				return true;
			return false;
		case EActionType::MoveDown:
			if (agents[agent].y < H - 1 && grid[agents[agent].y + 1][agents[agent].x] == CellType::Empty)
				return true;
			return false;
		case EActionType::MoveLeft:
			if (agents[agent].x > 0 && grid[agents[agent].y][agents[agent].x - 1] == CellType::Empty)
				return true;
			return false;
		case EActionType::MoveRight:
			if (agents[agent].x < W - 1 && grid[agents[agent].y][agents[agent].x + 1] == CellType::Empty)
				return true;
			return false;
		case EActionType::Pickup:
			if (items[agents[agent].y][agents[agent].x] != ItemType::None)
				return true;
			return false;
		case EActionType::Drop:
			if (items[agents[agent].y][agents[agent].x] == ItemType::None && agents[agent].hasItem)
				return true;
			return false;
		case EActionType::UseItem:
			if (agents[agent].hasItem && agents[agent].item != ItemType::None)
				return true;
			return false;
		default:
			return false;
		}
	}

	void NextState(std::array<FAgentAction, N_AGENTS> Actions, bool increaseTurn = true)
	{
		// Apply actions for each agent
		for (int i = 0; i < N_AGENTS; i++)
		{
			if (CanExecute(i, Actions[i]))
			{
				ApplyAgentAction(i, Actions[i]);
			}
			else
			{
				// If the action is not executable, we just wait
				ApplyAgentAction(i, FAgentAction{EActionType::Wait});
			}
		}

		if (increaseTurn)
		{
			// Increment turn counter after all actions are applied
			turnCounter++;
		}
	}

	void AgentTurnOverride(FSimulationContext<W, H, N_AGENTS> SimulationContext, int agentNr,
	                       const FAgentAction& action, bool increaseTurn = true)
	{
		//Iterate over all agents and apply the action. For agentNr, we choose action, for the others we get it from the simulation context.
		for (int i = 0; i < N_AGENTS; i++)
		{
			if (i == agentNr)
			{
				ApplyAgentAction(i, action);
			}
			else
			{
				const auto& step = SimulationContext.GetPlannedActionForAgent(i, turnCounter);
				if (!CanExecute(i, step))
				{
					//We cant execute the action, so we wait. Mark the agent as waiting temporarily.
					SimulationContext.SetAgentWaitingTemporarily(i, true);
					ApplyAgentAction(i, FAgentAction{EActionType::Wait});
				}
				ApplyAgentAction(i, step.actions[i]);
			}
		}

		if (increaseTurn)
		{
			// Increment turn counter after all actions are applied
			turnCounter++;
		}
	}

	void ApplyAgentAction(int agent, const FAgentAction& action)
	{
		switch (action.Type)
		{
		case EActionType::MoveUp:
			if (agents[agent].y > 0 && grid[agents[agent].y - 1][agents[agent].x] == CellType::Empty)
			{
				agents[agent].y--;
			}
			break;
		case EActionType::MoveDown:
			if (agents[agent].y < H - 1 && grid[agents[agent].y + 1][agents[agent].x] == CellType::Empty)
			{
				agents[agent].y++;
			}
			break;
		case EActionType::MoveLeft:
			if (agents[agent].x > 0 && grid[agents[agent].y][agents[agent].x - 1] == CellType::Empty)
			{
				agents[agent].x--;
			}
			break;
		case EActionType::MoveRight:
			if (agents[agent].x < W - 1 && grid[agents[agent].y][agents[agent].x + 1] == CellType::Empty)
			{
				agents[agent].x++;
			}
			break;
		case EActionType::Pickup:
			if (items[agents[agent].y][agents[agent].x] != ItemType::None)
			{
				ItemType previousItem = agents[agent].item;
				agents[agent].item = items[agents[agent].y][agents[agent].x];
				items[agents[agent].y][agents[agent].x] = previousItem;
				agents[agent].hasItem = true;
			}
			break;
		case EActionType::Drop:
			if (items[agents[agent].y][agents[agent].x] == ItemType::None && agents[agent].hasItem)
			{
				items[agents[agent].y][agents[agent].x] = agents[agent].item;
				agents[agent].hasItem = false;
				agents[agent].item = ItemType::None;
			}
			break;
		case EActionType::UseItem:
			if (agents[agent].hasItem && agents[agent].item != ItemType::None)
			{
				//TODO: Implement usage logic
				//For now, just reset the item
				agents[agent].hasItem = false;
				agents[agent].item = ItemType::None;
			}
		default:
			break; // Wait or any other action does nothing
		}
	}

	std::vector<FAgentAction> GetLegalActionsForAgent(int agent)
	{
		std::vector<FAgentAction> actions = {};
		// Check if agent can move up
		if (agents[agent].y > 0 && grid[agents[agent].y - 1][agents[agent].x] == CellType::Empty)
		{
			actions.push_back({EActionType::MoveUp});
		}
		// Check if agent can move down
		if (agents[agent].y < H - 1 && grid[agents[agent].y + 1][agents[agent].x] == CellType::Empty)
		{
			actions.push_back({EActionType::MoveDown});
		}
		// Check if agent can move left
		if (agents[agent].x > 0 && grid[agents[agent].y][agents[agent].x - 1] == CellType::Empty)
		{
			actions.push_back({EActionType::MoveLeft});
		}
		// Check if agent can move right
		if (agents[agent].x < W - 1 && grid[agents[agent].y][agents[agent].x + 1] == CellType::Empty)
		{
			actions.push_back({EActionType::MoveRight});
		}
		// Check if agent can pick up an item
		if (items[agents[agent].y][agents[agent].x] != ItemType::None && !(agents[agent].hasItem && agents[agent].item
			== items[agents[agent].y][agents[agent].x]))
		{
			// Only allow pickup if agent does not already have an item or has a different item (will drop the currently held one)
			actions.push_back({EActionType::Pickup});
		}
		// Check if agent can drop an item
		if (agents[agent].hasItem && items[agents[agent].y][agents[agent].x] == ItemType::None)
		{
			actions.push_back({EActionType::Drop});
		}
		// Check if agent can use an item
		if (agents[agent].hasItem && agents[agent].item == ItemType::Hose)
		{
			actions.push_back({EActionType::UseItem});
		}

		return actions;
	}
};
