/*
 * ASSESSMENT 3: CONSOLE BREAKOUT
 * 
 * Architecture:
 * - Physics: Continuous 2D Floating Point Vector System
 * - Rendering: Windows API Double-Buffering (Flicker-Free)
 * - Collision: AABB (Axis-Aligned Bounding Box) with Reflection
 * - Input: Asynchronous Keyboard Handling
 */

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// --- CONFIGURATION CONSTANTS ---
#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 25
#define PADDLE_WIDTH 8
#define BRICK_ROWS 5
#define BRICK_COLS 10

// --- DATA STRUCTURES (ENTITY COMPONENT MODEL) ---

// The Ball: Uses floats for sub-pixel precision 
typedef struct {
    float x, y;     // Position
    float dx, dy;   // Velocity Vector
    float speed;    // Magnitude scalar
} Ball;

// The Paddle: Constrained to integer grid logic for control tightness
typedef struct {
    int x, y;
    int width;
    int velocity;
} Paddle;

// The Brick: Simple boolean state and position
typedef struct {
    int x, y;
    int active;     // 1 = Exists, 0 = Destroyed
    int width;
} Brick;
// --- GLOBAL GAME STATE ---
// In a larger engine, these would be encapsulated in a 'GameWorld' struct.
Ball ball;
Paddle paddle;
Brick bricks[BRICK_ROWS][BRICK_COLS];
int score = 0;
int lives = 3;
int totalBricks = 0;

// --- LOW-LEVEL CONSOLE HELPERS ---

/**
 * Moves the cursor to specific coordinates without clearing screen.
 * This is the secret to flicker-free console games.
 */
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/**
 * Hides the blinking underscore cursor.
 * Essential for aesthetic quality in ASCII games.
 */
void setCursorVisibility(int visible) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = visible;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// --- GAME LOGIC FUNCTIONS ---

void initGame() {
    // 1. Setup Paddle at bottom center
    paddle.x = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2;
    paddle.y = SCREEN_HEIGHT - 2;
    paddle.width = PADDLE_WIDTH;
    paddle.velocity = 2; // Move 2 spaces per key press for responsiveness

    // 2. Setup Ball
    ball.x = SCREEN_WIDTH / 2.0f;
    ball.y = SCREEN_HEIGHT / 2.0f;
    ball.dx = 0.5f;  // Move right
    ball.dy = -0.5f; // Move up
    
    // 3. Setup Bricks Grid
    totalBricks = 0;
    int brickWidth = SCREEN_WIDTH / BRICK_COLS;
    
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            bricks[r][c].x = c * brickWidth;
            bricks[r][c].y = r + 2; // Offset from top
            bricks[r][c].width = brickWidth - 1; // -1 for visual gap
            bricks[r][c].active = 1;
            totalBricks++;
        }
    }
    
    // 4. Draw Initial Static Border
    system("cls");
    for (int i = 0; i <= SCREEN_WIDTH; i++) {
        gotoxy(i, 0); printf("#"); // Top
        gotoxy(i, SCREEN_HEIGHT); printf("#"); // Bottom
    }
    for (int i = 0; i <= SCREEN_HEIGHT; i++) {
        gotoxy(0, i); printf("#"); // Left
        gotoxy(SCREEN_WIDTH, i); printf("#"); // Right
    }
}

void handleInput() {
    if (_kbhit()) {
        char ch = getch();
        // Erase paddle at old position (Optimization: only erase necessary parts)
        gotoxy(paddle.x, paddle.y);
        for(int i=0; i<paddle.width; i++) printf(" "); 

        // Update Position
        if ((ch == 'a' || ch == 75) && paddle.x > 1) {
            paddle.x -= paddle.velocity;
        }
        if ((ch == 'd' || ch == 77) && paddle.x < SCREEN_WIDTH - paddle.width) {
            paddle.x += paddle.velocity;
        }
    }
}

void updatePhysics() {
    // ERASE BALL at old position
    gotoxy((int)ball.x, (int)ball.y);
    printf(" ");

    // UPDATE POSITION
    ball.x += ball.dx;
    ball.y += ball.dy;

    // WALL COLLISION
    if (ball.x <= 1 || ball.x >= SCREEN_WIDTH - 1) {
        ball.dx = -ball.dx; // Reflect X
        ball.x += ball.dx;  // Anti-tunneling push
    }
    if (ball.y <= 1) {
        ball.dy = -ball.dy; // Reflect Y
    }
    if (ball.y >= SCREEN_HEIGHT - 1) {
        // DEATH CONDITION
        lives--;
        ball.x = SCREEN_WIDTH / 2.0f;
        ball.y = SCREEN_HEIGHT / 2.0f;
        ball.dy = -0.5f;
        ball.dx = (rand() % 2 == 0)? 0.5f : -0.5f;
        Sleep(1000); // Penalty pause
    }

    // PADDLE COLLISION
    // Check if ball is at paddle Y level and within X range
    if ((int)ball.y == paddle.y) {
        if (ball.x >= paddle.x && ball.x <= paddle.x + paddle.width) {
            ball.dy = -ball.dy; // Standard bounce
            
            // "English" Mechanic: Add X velocity based on where it hit the paddle
            // Hit center = 0 change. Hit edges = sharp angle change.
            float hitOffset = (ball.x - (paddle.x + paddle.width / 2.0f)) / (paddle.width / 2.0f);
            ball.dx = hitOffset * 0.8f; // Max horizontal speed 0.8
        }
    }

    // BRICK COLLISION (Iterative AABB)
    // Optimization: Only check bricks if ball is in the upper half of screen
    if (ball.y < BRICK_ROWS + 5) {
        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                Brick *b = &bricks[r][c];
                if (b->active) {
                    // Check Intersection
                    if (ball.x >= b->x && ball.x <= b->x + b->width &&
                        (int)ball.y == b->y) {
                        
                        b->active = 0;      // Destroy Brick
                        ball.dy = -ball.dy; // Reflect
                        score += 10;
                        totalBricks--;
                        
                        // Erase Brick Visually
                        gotoxy(b->x, b->y);
                        for(int k=0; k<b->width; k++) printf(" ");
                        
                        goto end_brick_check; // Break loop to prevent multi-hit bug
                    }
                }
            }
        }
    }
    end_brick_check:;
}

void render() {
    // DRAW PADDLE
    gotoxy(paddle.x, paddle.y);
    for (int i = 0; i < paddle.width; i++) printf("=");

    // DRAW BALL
    gotoxy((int)ball.x, (int)ball.y);
    printf("O");

    // REFRESH BRICKS (Occasionally, to fix any console artifacts)
    static int frame = 0;
    frame++;
    if (frame % 50 == 0) {
        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                if (bricks[r][c].active) {
                    gotoxy(bricks[r][c].x, bricks[r][c].y);
                    for(int k=0; k<bricks[r][c].width; k++) printf("#");
                }
            }
        }
    }

    // DRAW HUD
    gotoxy(2, SCREEN_HEIGHT + 1);
    printf("SCORE: %d   LIVES: %d   BLOCKS LEFT: %d   ", score, lives, totalBricks);
}

int main() {
    setCursorVisibility(0); // Hide cursor
    srand((unsigned int)time(NULL));
    initGame();

    // GAME LOOP
    while (lives > 0 && totalBricks > 0) {
        handleInput();
        updatePhysics();
        render();
        Sleep(30); // ~33 FPS
    }

    // END SCREEN
    system("cls");
    gotoxy(SCREEN_WIDTH/2 - 5, SCREEN_HEIGHT/2);
    if (totalBricks == 0) printf("VICTORY!");
    else printf("GAME OVER");
    
    gotoxy(SCREEN_WIDTH/2 - 6, SCREEN_HEIGHT/2 + 2);
    printf("Final Score: %d", score);
    
    while(1); // Wait forever
    return 0;
}