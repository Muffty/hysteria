#include <iostream>
#include <vector>
#include <conio.h>     // For _kbhit() and _getch()
#include <windows.h>   // For Sleep() and system("cls")

constexpr int ROWS = 5;
constexpr int COLS = 5;

enum class CellState { Empty, Marked };

struct Cursor {
    int x = 0;
    int y = 0;
};

void clearScreen() {
    system("cls");  // On Linux/macOS: use system("clear")
}

void printGrid(const std::vector<std::vector<CellState>>& grid, const Cursor& cursor) {
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            if (cursor.x == x && cursor.y == y) {
                std::cout << "[";
            } else {
                std::cout << " ";
            }

            if (grid[y][x] == CellState::Marked)
                std::cout << "X";
            else
                std::cout << ".";

            if (cursor.x == x && cursor.y == y) {
                std::cout << "]";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\nUse WASD to move, E to mark, Q to quit\n";
}

int main() {
    std::vector<std::vector<CellState>> grid(ROWS, std::vector<CellState>(COLS, CellState::Empty));
    Cursor cursor;

    bool running = true;
    while (running) {
        clearScreen();
        printGrid(grid, cursor);

        char key = _getch();  // Waits for keypress

        switch (key) {
            case 'w': case 'W':
                if (cursor.y > 0) cursor.y--;
                break;
            case 's': case 'S':
                if (cursor.y < ROWS - 1) cursor.y++;
                break;
            case 'a': case 'A':
                if (cursor.x > 0) cursor.x--;
                break;
            case 'd': case 'D':
                if (cursor.x < COLS - 1) cursor.x++;
                break;
            case 'e': case 'E':
                grid[cursor.y][cursor.x] = CellState::Marked;
                break;
            case 'q': case 'Q':
                running = false;
                break;
        }
    }

    return 0;
}
