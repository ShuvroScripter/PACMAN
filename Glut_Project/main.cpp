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
    // Draw regular pellets
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

    // Draw bonus perks (larger and flashing)
    float flash = (sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f) > 0) ? 1.0f : 0.7f;
    glColor3f(0.0, flash, flash); // Cyan flashing color
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (maze[y][x] == 3) {
                glPointSize(10);
                glBegin(GL_POINTS);
                glVertex2f(x * CELL + CELL / 2, y * CELL + CELL / 2);
                glEnd();
            }
        }
    }
}

void drawPacman() {
    float centerX = pacmanX * CELL + CELL/2;
    float centerY = pacmanY * CELL + CELL/2;
    float radius = 15.0f;

    // Draw the yellow circle
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180;
        glVertex2f(centerX + radius * cos(theta),
                   centerY + radius * sin(theta));
    }
    glEnd();

    // Draw the black triangle (mouth) pointing in movement direction
    glColor3f(0.0, 0.0, 0.0); // Black color for the mouth

    if (dirX == 1 || (dirX == 0 && dirY == 0)) { // Right or default (not moving)
        glBegin(GL_TRIANGLES);
        glVertex2f(centerX, centerY); // Center point
        glVertex2f(centerX + radius, centerY - radius/2); // Top of mouth
        glVertex2f(centerX + radius, centerY + radius/2); // Bottom of mouth
        glEnd();
    }
    else if (dirX == -1) { // Left
        glBegin(GL_TRIANGLES);
        glVertex2f(centerX, centerY); // Center point
        glVertex2f(centerX - radius, centerY + radius/2); // Top of mouth
        glVertex2f(centerX - radius, centerY - radius/2); // Bottom of mouth
        glEnd();
    }
    else if (dirY == 1) { // Down
        glBegin(GL_TRIANGLES);
        glVertex2f(centerX, centerY); // Center point
        glVertex2f(centerX - radius/2, centerY + radius); // Left of mouth
        glVertex2f(centerX + radius/2, centerY + radius); // Right of mouth
        glEnd();
    }
    else if (dirY == -1) { // Up
        glBegin(GL_TRIANGLES);
        glVertex2f(centerX, centerY); // Center point
        glVertex2f(centerX + radius/2, centerY - radius); // Right of mouth
        glVertex2f(centerX - radius/2, centerY - radius); // Left of mouth
        glEnd();
    }
}
