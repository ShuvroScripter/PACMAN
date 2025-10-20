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

// 0 = path with pellet, 1 = wall, 2 = eaten
int pacmanX = 1, pacmanY = 1;
int dirX = 0, dirY = 0;
int score = 0;

// === NEW CODE: Ghosts and Game State ===
struct Ghost {
    int x, y;
    float r, g, b;
    int dirX, dirY;
    int moveCounter;
};

vector<Ghost> ghosts;
int lives = 3;
bool gameOver = false;
bool gameWon = false;

void initializeGhosts() {
    ghosts.clear();

    Ghost ghost1 = {7, 7, 1.0f, 0.0f, 0.0f, 1, 0, 0}; // Red - Blinky
    Ghost ghost2 = {7, 8, 0.0f, 1.0f, 1.0f, -1, 0, 0}; // Cyan - Inky
    Ghost ghost3 = {8, 7, 1.0f, 0.5f, 0.0f, 0, 1, 0}; // Orange - Clyde
    Ghost ghost4 = {8, 8, 1.0f, 0.6f, 0.8f, 0, -1, 0}; // Pink - Pinky

    ghosts.push_back(ghost1);
    ghosts.push_back(ghost2);
    ghosts.push_back(ghost3);
    ghosts.push_back(ghost4);
}
