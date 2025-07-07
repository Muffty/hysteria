
#include <iostream>
#include "WorldStateFactory.h"
#include "FMultiAgentMCTS.h"
#include <conio.h>

template <int W, int H, int N_AGENTS>
void PrintBoard(const WorldState<W, H, N_AGENTS>& world, int selectedX, int selectedY) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            char ch = '.'; // Default for empty
            bool printed = false;

            // 1) Agent?
            for (int a = 0; a < N_AGENTS; ++a) {
                const auto& agent = world.agents[a];
                if (agent.x == x && agent.y == y) {
                    if (agent.hasItem && agent.item != ItemType::None) {
                        // Uppercase first letter of item
                        switch (agent.item) {
                            case ItemType::Food:    ch = 'F'; break;
                            case ItemType::Hose:    ch = 'H'; break;
                            case ItemType::Pickaxe: ch = 'P'; break;
                            case ItemType::Coin:    ch = 'C'; break;
                            default: ch = 'A'; break;
                        }
                    } else {
                        ch = 'a';
                    }
                    printed = true;
                    break;
                }
            }

            // 2) Field type, only if not already printed
            if (!printed) {
                switch (world.grid[y][x]) {
                    case CellType::Wall:           ch = '#'; break;
                    case CellType::Fire:           ch = '~'; break;
                    case CellType::PlayerObstacle: ch = 'O'; break;
                    default: break; // do nothing if empty
                }
                printed = (world.grid[y][x] != CellType::Empty);
            }

            // 3) Item, only if not already printed and cell is empty
            if (!printed) {
                // Check for dropped items
                // If you track items by cell, e.g. world.items[y][x]:
                switch (world.items[y][x]) {
                    case ItemType::Food:    ch = 'f'; break;
                    case ItemType::Hose:    ch = 'h'; break;
                    case ItemType::Pickaxe: ch = 'p'; break;
                    case ItemType::Coin:    ch = 'c'; break;
                    default: break; // leave as '.'
                }
            }

            // Selected cell highlight (optional, e.g. reverse video)
            if (x == selectedX && y == selectedY)
                std::cout << '[' << ch << ']';
            else
                std::cout << ' ' << ch << ' ';
        }
        std::cout << "\n";
    }
}

int main()
{
    using namespace HysteriaSim;

    // 1. Setup world
    auto world = CreateDemoMap();

    // 2. Initialize MCTS system
    FMultiAgentMCTS<16, 16, 3> Planner = FMultiAgentMCTS<16, 16, 3>(world);

    // 3. Run one round of planning
    Planner.Step();

    // 4. Print planned actions
    for (int i = 0; i < 3; ++i)
    {
        auto action = Planner.GetPlannedActionForAgent(i);
        std::cout << "Agent " << i << ": " << int(action.Type) << "\n";
    }

    int selX = 0, selY = 0;
    while (true) {
        PrintBoard(Planner.GetCurrentState(), selX, selY);
        std::cout << "Use WASD to move, c to put a coint, SPACE to step, q to quit\n";
        char c = _getch(); // or std::cin.get(), but _getch() doesn't require enter
        if (c == 'q') break;
        if (c == 'c')
        {
            //Place a coin at the selected position
            Planner.GetCurrentState().PlaceItem(selX, selY, ItemType::Coin);
        }
        if (c == 'w' && selY > 0) --selY;
        if (c == 's' && selY < 16-1) ++selY;
        if (c == 'a' && selX > 0) --selX;
        if (c == 'd' && selX < 16-1) ++selX;
        if (c == ' ') {
            // Advance simulation
            Planner.Step();
        }
        system("cls"); // clear screen (Windows); use "clear" on Unix
    }

    return 0;
}

