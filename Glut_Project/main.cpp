#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

const int ROWS = 15;
const int COLS = 15;
const float CELL = 40.0f;

int maze[ROWS][COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,1,0,0,1},
    {1,0,1,0,1,0,1,1,1,0,0,1,0,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,0,1,1,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,0,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,1,0,0,0,0,0,1,0,0,1,0,1},
    {1,0,0,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,0,1,1,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// 0 = path with pellet, 1 = wall, 2 = eaten, 3 = bonus perk
int pacmanX = 1, pacmanY = 1;
int dirX = 0, dirY = 0;
int score = 0;

// === NEW CODE: Ghosts and Game State ===
struct Ghost {
    int x, y;
    float r, g, b;
    int dirX, dirY;
    int moveCounter;
    bool isSlowed;
    int slowTimer;
};

vector<Ghost> ghosts;
int lives = 3;
bool gameOver = false;
bool gameWon = false;

// === NEW CODE: Bonus Perks and Ghost Slow Effect ===
bool ghostSlowEffect = false;
int ghostSlowTimer = 0;
const int SLOW_DURATION = 300; // 300 timer ticks (about 10 seconds)
const int BONUS_POINTS = 100;

void initializeBonusPerks() {
    // Place bonus perks in strategic locations
    maze[1][COLS-2] = 3;  // Top right
    maze[ROWS-2][1] = 3;  // Bottom left
    maze[ROWS-2][COLS-2] = 3; // Bottom right
    maze[ROWS/2][COLS/2] = 3; // Center
}

void initializeGhosts() {
    ghosts.clear();

    Ghost ghost1 = {1, COLS-2, 1.0f, 0.0f, 0.0f, 1, 0, 0, false, 0}; // Red - Blinky
    Ghost ghost2 = {ROWS-2, 1, 0.0f, 1.0f, 1.0f, -1, 0, 0, false, 0}; // Cyan - Inky
    Ghost ghost3 = {ROWS-2, COLS-2, 1.0f, 0.5f, 0.0f, 0, -1, 0, false, 0}; // Orange - Clyde
    Ghost ghost4 = {ROWS/2, COLS/2, 1.0f, 0.6f, 0.8f, 1, 0, 0, false, 0}; // Pink - Pinky

    ghosts.push_back(ghost1);
    ghosts.push_back(ghost2);
    ghosts.push_back(ghost3);
    ghosts.push_back(ghost4);
}

void resetGame() {
    pacmanX = 1;
    pacmanY = 1;
    dirX = 0;
    dirY = 0;
    score = 0;
    lives = 3;
    gameOver = false;
    gameWon = false;
    ghostSlowEffect = false;
    ghostSlowTimer = 0;

    // Reset maze pellets and bonus perks
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (maze[y][x] == 2 || maze[y][x] == 3) {
                maze[y][x] = 0;
            }
        }
    }

    initializeBonusPerks();
    initializeGhosts();
}
