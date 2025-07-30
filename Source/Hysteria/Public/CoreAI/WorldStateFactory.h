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

		// Add House on top
		state.grid[0][3] = CellType::Wall;
		state.grid[1][3] = CellType::Wall;
		state.grid[2][3] = CellType::Wall;
		state.grid[3][3] = CellType::Wall;
		state.grid[4][3] = CellType::Wall;
		state.grid[4][2] = CellType::Wall;
		state.grid[4][1] = CellType::PlayerObstacle;
		state.grid[4][0] = CellType::Wall;
		
		state.items[0][2] = ItemType::Pickaxe;

		// Add fire area
		state.items[5][0] = ItemType::Hose;
		state.grid[6][0] = CellType::Fire;
		state.grid[6][1] = CellType::Fire;
		state.grid[6][2] = CellType::Fire;
		state.grid[7][0] = CellType::Fire;
		state.grid[7][1] = CellType::Fire;
		state.grid[7][2] = CellType::Fire;
		state.grid[7][3] = CellType::Fire;
		state.grid[8][3] = CellType::Fire;
		state.grid[9][3] = CellType::Fire;
		state.grid[10][3] = CellType::Fire;
		state.grid[11][3] = CellType::Fire;
		state.grid[12][3] = CellType::Fire;
		state.grid[12][2] = CellType::Fire;
		state.grid[13][2] = CellType::Fire;
		state.grid[14][2] = CellType::Fire;
		state.grid[14][1] = CellType::Fire;
		state.grid[14][0] = CellType::Fire;

		// Place agents
		state.agents[0] = AgentState{2, 1, false};
		state.agents[1] = AgentState{1, 8, false};
		state.agents[2] = AgentState{5, 12, false};

		state.turnCounter = 0;

		return state;
	}
	
	WorldState<16, 16, 1> CreateDebugMap()
	{
		WorldState<16, 16, 1> state{};

		// Example: empty map
		for (int y = 0; y < state.GetHeight(); ++y)
			for (int x = 0; x < state.GetWidth(); ++x)
				state.grid[y][x] = CellType::Empty;

		// Add walls
		state.grid[3][3] = CellType::PlayerObstacle;

		// Place agents
		AgentState agent = AgentState{2,3,false};
		agent.hasItem = true;
		agent.item = ItemType::Pickaxe;
		state.agents[0] = agent;

		state.turnCounter = 0;

		return state;
	}
}