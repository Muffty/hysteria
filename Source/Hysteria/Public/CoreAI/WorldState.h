#pragma once
#include "Types.h"
#include "SimulationContext.h"


template <int W, int H, int N_AGENTS>
struct WorldState
{
	CellType grid[H][W];
	ItemType items[H][W];
	AgentState agents[N_AGENTS];
	uint8_t turnCounter;
	EDirection Directions[4] = { EDirection::Up, EDirection::Down, EDirection::Left, EDirection::Right };

	WorldState Clone()
	{
		WorldState<W, H, N_AGENTS> newState;
		for (int y = 0; y < H; ++y)
		{
			for (int x = 0; x < W; ++x)
			{
				newState.grid[y][x] = grid[y][x];
				newState.items[y][x] = items[y][x];
			}
		}
		for (int i = 0; i < N_AGENTS; ++i)
		{
			newState.agents[i] = agents[i];
		}
		newState.turnCounter = turnCounter;
		return newState;
	}

	WorldState()
	{
		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
				grid[y][x] = CellType::Empty;

		for (int i = 0; i < N_AGENTS; ++i)
			agents[i] = AgentState{0, 0, false, ItemType::None, 0, false};

		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
				items[y][x] = ItemType::None;

		turnCounter = 0;
	}

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
				ApplyAgentAction(i, step);
			}
		}

		if (increaseTurn)
		{
			// Increment turn counter after all actions are applied
			turnCounter++;
		}
	}

	void SetItem(int x, int y, ItemType item)
	{
		// Ensure valid coordinates
		if (x >= 0 && x < W && y >= 0 && y < H)
		{
			items[y][x] = item;
		}
	}

	void SetTile(int x, int y, CellType type)
	{
		// Ensure valid coordinates
		if (x >= 0 && x < W && y >= 0 && y < H)
		{
			grid[y][x] = type;
		}
	}

	void HandleNeighborTileOnUseItem(int Agent, CellType CType)
	{
		for (int i = 0; i < sizeof(Directions); i++)
		{
			auto direction = Directions[i];
			if (GetNeighborTileCell(agents[Agent].x, agents[Agent].y, direction) == CType)
			{
				SetNeighborTileCell(agents[Agent].x, agents[Agent].y, direction, CellType::Empty);
				agents[Agent].score += 10;
			}
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
				if (items[agents[agent].y][agents[agent].x] == ItemType::Coin)
				{
					agents[agent].score += 10;
					items[agents[agent].y][agents[agent].x] = ItemType::None;
				}
				else
				{
					ItemType previousItem = agents[agent].item;
					agents[agent].item = items[agents[agent].y][agents[agent].x];
					items[agents[agent].y][agents[agent].x] = previousItem;
					agents[agent].hasItem = true;
				}
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
				switch (agents[agent].item)
				{
				case ItemType::Hose:
					HandleNeighborTileOnUseItem(agent, CellType::Fire);
					break;
				case ItemType::Pickaxe:
					HandleNeighborTileOnUseItem(agent, CellType::PlayerObstacle);
					break;
				default:
					break;
				}
			}
		default:
			break; // Wait or any other action does nothing
		}
	}


	ItemType GetNeighborTileItem(int X, int Y, EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::Down:
			Y = Y + 1;
			break;
		case EDirection::Up:
			Y = Y - 1;
			break;
		case EDirection::Left:
			X = X - 1;
			break;
		case EDirection::Right:
			X = X + 1;
			break;
		}
		if (X >= 0 && X < W && Y >= 0 && Y < H)
		{
			return items[Y][X];
		}
		return ItemType::None;
	}

	CellType GetNeighborTileCell(int X, int Y, EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::Down:
			Y = Y + 1;
			break;
		case EDirection::Up:
			Y = Y - 1;
			break;
		case EDirection::Left:
			X = X - 1;
			break;
		case EDirection::Right:
			X = X + 1;
			break;
		}
		if (X >= 0 && X < W && Y >= 0 && Y < H)
		{
			return grid[Y][X];
		}
		return CellType::Empty;
	}

	void SetNeighborTileCell(int X, int Y, EDirection Direction, CellType Type)
	{
		switch (Direction)
		{
		case EDirection::Down:
			Y = Y + 1;
			break;
		case EDirection::Up:
			Y = Y - 1;
			break;
		case EDirection::Left:
			X = X - 1;
			break;
		case EDirection::Right:
			X = X + 1;
			break;
		}
		if (X >= 0 && X < W && Y >= 0 && Y < H)
		{
			grid[Y][X] = Type;
		}
	}

	HYSTERIA_VECTOR<FAgentAction> GetLegalActionsForAgent(int agent)
	{
		HYSTERIA_VECTOR<FAgentAction> actions = {};
		// Check if agent can move up
		if (agents[agent].y > 0 && grid[agents[agent].y - 1][agents[agent].x] == CellType::Empty)
		{
#ifdef HYSTERIA_USE_UNREAL
			actions.Add({EActionType::MoveUp});
#else
			actions.push_back({EActionType::MoveUp});
#endif
		}
		// Check if agent can move down
		if (agents[agent].y < H - 1 && grid[agents[agent].y + 1][agents[agent].x] == CellType::Empty)
		{
#ifdef HYSTERIA_USE_UNREAL
			actions.Add({EActionType::MoveDown});
#else
			actions.push_back({EActionType::MoveDown});
#endif
		}
		// Check if agent can move left
		if (agents[agent].x > 0 && grid[agents[agent].y][agents[agent].x - 1] == CellType::Empty)
		{
#ifdef HYSTERIA_USE_UNREAL
			actions.Add({EActionType::MoveLeft});
#else
			actions.push_back({EActionType::MoveLeft});
#endif
		}
		// Check if agent can move right
		if (agents[agent].x < W - 1 && grid[agents[agent].y][agents[agent].x + 1] == CellType::Empty)
		{
#ifdef HYSTERIA_USE_UNREAL
			actions.Add({EActionType::MoveRight});
#else
			actions.push_back({EActionType::MoveRight});
#endif
		}
		// Check if agent can pick up an item
		if (items[agents[agent].y][agents[agent].x] != ItemType::None && !(agents[agent].hasItem && agents[agent].item
			== items[agents[agent].y][agents[agent].x]))
		{
			// Only allow pickup if agent does not already have an item or has a different item (will drop the currently held one)
#ifdef HYSTERIA_USE_UNREAL
			actions.Add({EActionType::Pickup});
#else
			actions.push_back({EActionType::Pickup});
#endif
		}
		// Check if agent can drop an item
		if (agents[agent].hasItem && items[agents[agent].y][agents[agent].x] == ItemType::None)
		{
#ifdef HYSTERIA_USE_UNREAL
			actions.Add({EActionType::Drop});
#else
			actions.push_back({EActionType::Drop});
#endif
		}
		// Check if agent can use an item
		if (agents[agent].hasItem && agents[agent].item == ItemType::Hose)
		{
#ifdef HYSTERIA_USE_UNREAL
			actions.Add({EActionType::UseItem});
#else
			actions.push_back({EActionType::UseItem});
#endif
		}

		return actions;
	}
};
