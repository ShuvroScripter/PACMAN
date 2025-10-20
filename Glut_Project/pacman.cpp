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
// === UPDATED CODE: Classic Ghost Drawing ===
void drawGhost(const Ghost& ghost) {
    float centerX = ghost.x * CELL + CELL/2;
    float centerY = ghost.y * CELL + CELL/2;
    float radius = 15.0f;

    // Set ghost color
    glColor3f(ghost.r, ghost.g, ghost.b);

    // Main ghost body (semi-circle with wavy bottom)
    glBegin(GL_POLYGON);

    // Draw top semi-circle
    for (int i = 0; i <= 180; i++) {
        float theta = i * 3.14159f / 180;
        glVertex2f(centerX + radius * cos(theta),
                   centerY + radius * sin(theta));
    }

    // Draw wavy bottom
    glVertex2f(centerX + radius, centerY);
    glVertex2f(centerX + radius * 0.6, centerY - radius * 0.3);
    glVertex2f(centerX + radius * 0.3, centerY - radius * 0.6);
    glVertex2f(centerX, centerY - radius * 0.3);
    glVertex2f(centerX - radius * 0.3, centerY - radius * 0.6);
    glVertex2f(centerX - radius * 0.6, centerY - radius * 0.3);
    glVertex2f(centerX - radius, centerY);

    glEnd();

    // Draw eyes (white part)
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180;
        glVertex2f(centerX - 6 + 4 * cos(theta),
                   centerY + 5 + 4 * sin(theta));
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180;
        glVertex2f(centerX + 6 + 4 * cos(theta),
                   centerY + 5 + 4 * sin(theta));
    }
    glEnd();

    // Draw pupils (black part) - direction based
    glColor3f(0.0, 0.0, 0.0);
    float pupilOffsetX = 0, pupilOffsetY = 0;

    // Adjust pupil position based on ghost direction
    if (ghost.dirX == 1) { // Right
        pupilOffsetX = 2;
    } else if (ghost.dirX == -1) { // Left
        pupilOffsetX = -2;
    } else if (ghost.dirY == 1) { // Down
        pupilOffsetY = -2;
    } else if (ghost.dirY == -1) { // Up
        pupilOffsetY = 2;
    }

    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180;
        glVertex2f(centerX - 6 + pupilOffsetX + 2 * cos(theta),
                   centerY + 5 + pupilOffsetY + 2 * sin(theta));
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180;
        glVertex2f(centerX + 6 + pupilOffsetX + 2 * cos(theta),
                   centerY + 5 + pupilOffsetY + 2 * sin(theta));
    }
    glEnd();
}
void drawGhosts() {
    for (const auto& ghost : ghosts) {
        drawGhost(ghost);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Maze
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (maze[y][x] == 1)
                drawSquare(x, y, 0.1, 0.1, 0.8);
            else
                drawSquare(x, y, 0.0, 0.0, 0.0);
        }
    }

    drawPellets();
    drawPacman();
    drawGhosts();

    // Display Score and Lives
    drawText(10, 20, "Score: " + to_string(score));
    drawText(COLS * CELL - 150, 20, "Lives: " + to_string(lives));

    // Game over or win messages
    if (gameOver) {
        drawText(COLS * CELL / 2 - 80, ROWS * CELL / 2, "GAME OVER!");
        drawText(COLS * CELL / 2 - 100, ROWS * CELL / 2 + 30, "Press R to Restart");
    }

    if (gameWon) {
        drawText(COLS * CELL / 2 - 60, ROWS * CELL / 2, "YOU WIN!");
        drawText(COLS * CELL / 2 - 100, ROWS * CELL / 2 + 30, "Press R to Restart");
    }

    glutSwapBuffers();
}

void movePacman() {
    if (gameOver || gameWon) return;

    int nx = pacmanX + dirX;
    int ny = pacmanY + dirY;

    if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && maze[ny][nx] != 1) {
        pacmanX = nx;
        pacmanY = ny;
        if (maze[ny][nx] == 0) {   // pellet eaten
            maze[ny][nx] = 2;
            score += 10;

            // Check if all pellets are eaten
            bool allEaten = true;
            for (int y = 0; y < ROWS && allEaten; y++) {
                for (int x = 0; x < COLS && allEaten; x++) {
                    if (maze[y][x] == 0) {
                        allEaten = false;
                    }
                }
            }
            if (allEaten) {
                gameWon = true;
            }
        }
    }
}
void moveGhosts() {
    if (gameOver || gameWon) return;

    for (auto& ghost : ghosts) {
        ghost.moveCounter++;

        // Change direction randomly every 5-10 moves
        if (ghost.moveCounter > (5 + rand() % 6)) {
            int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
            int newDir = rand() % 4;
            ghost.dirX = directions[newDir][0];
            ghost.dirY = directions[newDir][1];
            ghost.moveCounter = 0;
        }

        int nx = ghost.x + ghost.dirX;
        int ny = ghost.y + ghost.dirY;

        // If next position is valid, move; otherwise, change direction
        if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && maze[ny][nx] != 1) {
            ghost.x = nx;
            ghost.y = ny;
        } else {
            // Change direction when hitting wall
            int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
            int newDir = rand() % 4;
            ghost.dirX = directions[newDir][0];
            ghost.dirY = directions[newDir][1];
            ghost.moveCounter = 0;
        }
    }
}

void checkCollisions() {
    if (gameOver || gameWon) return;

    for (const auto& ghost : ghosts) {
        if (pacmanX == ghost.x && pacmanY == ghost.y) {
            lives--;

            if (lives <= 0) {
                gameOver = true;
            } else {
                // Reset positions but keep score and maze state
                pacmanX = 1;
                pacmanY = 1;
                dirX = 0;
                dirY = 0;
                initializeGhosts();
            }
            break;
        }
    }
}

void timer(int value) {
    movePacman();
    moveGhosts();
    checkCollisions();
    glutPostRedisplay();
    glutTimerFunc(200, timer, 0);
}

void handleKeys(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:    dirX = 0; dirY = -1; break;
        case GLUT_KEY_DOWN:  dirX = 0; dirY = 1;  break;
        case GLUT_KEY_LEFT:  dirX = -1; dirY = 0; break;
        case GLUT_KEY_RIGHT: dirX = 1;  dirY = 0; break;
    }
}

void keyboard(unsigned char key, int, int) {
    if (key == 'r' || key == 'R') {
        resetGame();
    }
}

void init() {
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, COLS * CELL, ROWS * CELL, 0);
    srand(time(0));
    initializeGhosts();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(COLS * CELL, ROWS * CELL + 40);
    glutCreateWindow("Pac-Man with Classic Ghosts");

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(handleKeys);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(200, timer, 0);

    glutMainLoop();
    return 0;
}
