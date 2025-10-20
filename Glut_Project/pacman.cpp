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
void resetGame() {
    pacmanX = 1;
    pacmanY = 1;
    dirX = 0;
    dirY = 0;
    score = 0;
    lives = 3;
    gameOver = false;
    gameWon = false;

    // Reset maze pellets
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (maze[y][x] == 2) {
                maze[y][x] = 0;
            }
        }
    }

    initializeGhosts();
}

void drawSquare(int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x * CELL, y * CELL);
    glVertex2f((x + 1) * CELL, y * CELL);
    glVertex2f((x + 1) * CELL, (y + 1) * CELL);
    glVertex2f(x * CELL, (y + 1) * CELL);
    glEnd();
}

void drawText(float x, float y, string text) {
    glColor3f(1, 1, 1);
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void drawPellets() {
    glColor3f(1.0, 1.0, 0.5);
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (maze[y][x] == 0) {
                glPointSize(5);
                glBegin(GL_POINTS);
                glVertex2f(x * CELL + CELL / 2, y * CELL + CELL / 2);
                glEnd();
            }
        }
    }
}

void drawPacman() {
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180;
        glVertex2f(pacmanX * CELL + CELL/2 + 15 * cos(theta),
                   pacmanY * CELL + CELL/2 + 15 * sin(theta));
    }
    glEnd();
}
