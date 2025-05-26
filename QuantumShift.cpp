#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

// Constants
const int GRID_SIZE = 5;
const char EMPTY = '.';
const char PARTICLE = 'Q';
const int MAX_MOVES = 20;

// Game state
struct Game {
    vector<vector<char>> grid;
    vector<vector<char>> target;
    int level;
    int movesLeft;
    int score;
    bool gameOver;
};

// Initialize game
void initGame(Game& game, int level) {
    game.grid = vector<vector<char>>(GRID_SIZE, vector<char>(GRID_SIZE, EMPTY));
    game.target = vector<vector<char>>(GRID_SIZE, vector<char>(GRID_SIZE, EMPTY));
    game.level = level;
    game.movesLeft = MAX_MOVES;
    game.gameOver = false;

    // Place particles randomly
    srand(time(0));
    int particles = 3 + level; // Increase particles with level
    for (int i = 0; i < particles; ++i) {
        int rx, ry;
        do {
            rx = rand() % GRID_SIZE;
            ry = rand() % GRID_SIZE;
        } while (game.grid[rx][ry] == PARTICLE);
        game.grid[rx][ry] = PARTICLE;
        do {
            rx = rand() % GRID_SIZE;
            ry = rand() % GRID_SIZE;
        } while (game.target[rx][ry] == PARTICLE);
        game.target[rx][ry] = PARTICLE;
    }
}

// Display game
void displayGame(const Game& game) {
    system("clear"); // Use "cls" on Windows
    cout << "=== Quantum Shift | Level " << game.level << " ===" << endl;
    cout << "Current Grid:       Target Grid:\n";
    for (int i = 0; i < GRID_SIZE; ++i) {
        // Current grid
        for (int j = 0; j < GRID_SIZE; ++j) {
            cout << game.grid[i][j] << " ";
        }
        cout << "    ";
        // Target grid
        for (int j = 0; j < GRID_SIZE; ++j) {
            cout << game.target[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Moves Left: " << game.movesLeft << " | Score: " << game.score << endl;
}

// Check if puzzle is solved
bool isSolved(const Game& game) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (game.grid[i][j] != game.target[i][j]) {
                return false;
            }
        }
    }
    return true;
}

// Shift row or column
void shift(Game& game, char type, int index, char direction) {
    if (game.movesLeft <= 0) return;

    if (type == 'r') { // Shift row
        vector<char> row = game.grid[index];
        if (direction == 'l') {
            char first = row[0];
            for (int i = 0; i < GRID_SIZE - 1; ++i) {
                row[i] = row[i + 1];
            }
            row[GRID_SIZE - 1] = first;
        } else { // Right
            char last = row[GRID_SIZE - 1];
            for (int i = GRID_SIZE - 1; i > 0; --i) {
                row[i] = row[i - 1];
            }
            row[0] = last;
        }
        game.grid[index] = row;
    } else { // Shift column
        vector<char> col(GRID_SIZE);
        for (int i = 0; i < GRID_SIZE; ++i) {
            col[i] = game.grid[i][index];
        }
        if (direction == 'u') {
            char first = col[0];
            for (int i = 0; i < GRID_SIZE - 1; ++i) {
                col[i] = col[i + 1];
            }
            col[GRID_SIZE - 1] = first;
        } else { // Down
            char last = col[GRID_SIZE - 1];
            for (int i = GRID_SIZE - 1; i > 0; --i) {
                col[i] = col[i - 1];
            }
            col[0] = last;
        }
        for (int i = 0; i < GRID_SIZE; ++i) {
            game.grid[i][index] = col[i];
        }
    }
    game.movesLeft--;
}

// Save game
void saveGame(const Game& game) {
    ofstream file("quantumshift_save.txt");
    if (file.is_open()) {
        file << game.level << " " << game.movesLeft << " " << game.score << endl;
        for (const auto& row : game.grid) {
            for (char cell : row) {
                file << cell << " ";
            }
            file << endl;
        }
        for (const auto& row : game.target) {
            for (char cell : row) {
                file << cell << " ";
            }
            file << endl;
        }
        file.close();
        cout << "Game saved!" << endl;
    }
}

// Load game
bool loadGame(Game& game) {
    ifstream file("quantumshift_save.txt");
    if (file.is_open()) {
        file >> game.level >> game.movesLeft >> game.score;
        game.grid = vector<vector<char>>(GRID_SIZE, vector<char>(GRID_SIZE));
        game.target = vector<vector<char>>(GRID_SIZE, vector<char>(GRID_SIZE));
        for (auto& row : game.grid) {
            for (char& cell : row) {
                file >> cell;
            }
        }
        for (auto& row : game.target) {
            for (char& cell : row) {
                file >> cell;
            }
        }
        file.close();
        cout << "Game loaded!" << endl;
        return true;
    }
    return false;
}

// Main game loop
int main() {
    Game game;
    cout << "Load saved game? (y/n): ";
    char load;
    cin >> load;
    if (load == 'y' && !loadGame(game)) {
        cout << "No save file found. Starting new game." << endl;
        initGame(game, 1);
    } else if (load != 'y') {
        initGame(game, 1);
    }

    cout << "Welcome to Quantum Shift! Align the grid to match the target.\n";
    cout << "Commands: 'r <row> l/r' (shift row left/right), 'c <col> u/d' (shift column up/down), 's' to save, 'q' to quit.\n";

    while (!game.gameOver) {
        displayGame(game);
        if (isSolved(game)) {
            game.score += game.movesLeft * 10; // Bonus for unused moves
            cout << "Level " << game.level << " complete! Score: " << game.score << endl;
            game.level++;
            initGame(game, game.level);
            cout << "Starting Level " << game.level << "!" << endl;
            continue;
        }
        if (game.movesLeft <= 0) {
            cout << "Out of moves! Game Over. Final Score: " << game.score << endl;
            game.gameOver = true;
            continue;
        }

        cout << "Enter command (e.g., 'r 0 l', 'c 1 u', 's', 'q'): ";
        string command;
        cin >> command;
        if (command == "q") {
            cout << "Game Over. Final Score: " << game.score << ". Thanks for playing!" << endl;
            break;
        } else if (command == "s") {
            saveGame(game);
        } else if (command == "r") {
            int row;
            string dir;
            cin >> row >> dir;
            if (row >= 0 && row < GRID_SIZE && (dir == "l" || dir == "r")) {
                shift(game, 'r', row, dir[0]);
            } else {
                cout << "Invalid command!" << endl;
            }
        } else if (command == "c") {
            int col;
            string dir;
            cin >> col >> dir;
            if (col >= 0 && col < GRID_SIZE && (dir == "u" || dir == "d")) {
                shift(game, 'c', col, dir[0]);
            } else {
                cout << "Invalid command!" << endl;
            }
        } else {
            cout << "Invalid command!" << endl;
        }
    }

    return 0;
}
