
#include <iostream>
#include "WorldStateFactory.h"
#include "FMultiAgentMCTS.h"

int main()
{
    using namespace HysteriaSim;

    // 1. Setup world
    auto world = CreateDemoMap();

    // 2. Initialize MCTS system
    FMultiAgentMCTS<16, 16, 3> Planner;
    Planner.SetTurnCounter(world.turnCounter);

    // 3. Run one round of planning
    Planner.PlanAllAgents(world, /*numIterations=*/500);

    // 4. Print planned actions
    for (int i = 0; i < N; ++i)
    {
        auto action = Planner.GetPlannedActionForAgent(i, world.turnCounter);
        std::cout << "Agent " << i << ": " << ToString(action.Type) << "\n";
    }

    return 0;
}