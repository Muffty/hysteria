#pragma once

#include "WorldState.h"

namespace HysteriaSim
{
	WorldState<16, 16, 3> CreateDemoMap()
	{
		WorldState<16, 16, 3> state{};

		// Example: empty map
		for (int y = 0; y < state.GetHeight(); ++y)
			for (int x = 0; x < state.GetWidth(); ++x)
				state.grid[y][x] = CellType::Empty;

		// Add walls
		state.grid[3][3] = CellType::Wall;
		state.grid[4][3] = CellType::Wall;
		state.grid[5][3] = CellType::Wall;

		// Add an item
		state.items[1][1] = ItemType::Coin;

		// Place agents
		state.agents[0] = AgentState{0, 0, false};
		state.agents[1] = AgentState{2, 2, false};
		state.agents[2] = AgentState{5, 12, false};

		state.turnCounter = 0;

		return state;
	}
}