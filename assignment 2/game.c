/*
 * ASSESSMENT 2: ENHANCED LANE SWITCHING GAME
 * 
 * Modifications Implemented:
 * 1. Scoring System: Tracks successful obstacle evasions.
 * 2. High Score Persistence: Saves/Loads best score from 'highscore.txt'.
 * 3. Modular Architecture: Separated logic into functional blocks.
 *
 * Compiler: GCC / MinGW / MSVC
 * Platform: Windows (Due to conio.h and windows.h dependencies)
 */

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

// --- CONSTANTS & MACROS ---
#define DATA_FILE "highscore.txt"

// --- FUNCTION PROTOTYPES ---
// Defining prototypes ahead of time is a C best practice for large codebases.[7]
int loadHighScore();
void saveHighScore(int score);
void drawFrame(int playerX, int obstacleLane, int obstacleStep, int score, int highScore);

int main() {
    // Seed the Random Number Generator
    srand((unsigned int)time(0));

    // --- GAME STATE VARIABLES ---
    int playerX = 1;            // Player horizontal position (0, 1, 2)
    int step = 0;               // Obstacle vertical progress (0 to 10)
    int obstacleLane = rand() % 3; // Lane of the current obstacle
    int currentScore = 0;       // Session score
    int highScore = 0;          // All-time best score
    int gameActive = 1;         // Boolean flag for game loop
    
    // Load the persistent high score at startup
    highScore = loadHighScore();

    // --- MAIN GAME LOOP ---
    while (gameActive) {

        // 1. INPUT HANDLING
        // _kbhit() checks the buffer without blocking execution.
        if (_kbhit()) {
            char ch = getch();

            // Handle Arrow Keys (Windows sends a prefix code first, usually -32 or 224)
            if (ch == -32 || ch == 0) {
                ch = getch(); // Consume the actual key code
                if (ch == 75 && playerX > 0) { // 75 is Left Arrow
                    playerX--;
                }
                if (ch == 77 && playerX < 2) { // 77 is Right Arrow
                    playerX++;
                }
            } 
            // Handle WASD (Standard gaming convention)
            else {
                if ((ch == 'a' || ch == 'A') && playerX > 0) playerX--;
                if ((ch == 'd' || ch == 'D') && playerX < 2) playerX++;
            }
        }

        // 2. RENDERING
        drawFrame(playerX, obstacleLane, step, currentScore, highScore);

        // 3. COLLISION DETECTION
        // Collision occurs if obstacle is at bottom (step 10) AND in same lane as player
        if (step == 10 && playerX == obstacleLane) {
            printf("\n  !!! GAME OVER!!!\n");
            printf("   Final Score: %d\n", currentScore);
            
            // High Score Check
            if (currentScore > highScore) {
                printf("   *** NEW HIGH SCORE! ***\n");
                saveHighScore(currentScore);
                highScore = currentScore; // Update memory for display
            } else {
                printf("   Current High Score: %d\n", highScore);
            }
            
            printf("   Press any key to exit...");
            getch(); // Blocking wait for user acknowledgement
            gameActive = 0; // Terminate loop
            break;
        }

        // 4. GAME TIMING
        Sleep(120); // 120ms delay ~ 8.3 FPS

        // 5. UPDATE WORLD STATE
        step++; // Move obstacle down

        // If obstacle clears the bottom (Reset Logic)
        if (step > 10) {
            step = 0;
            obstacleLane = rand() % 3; // Choose new lane
            currentScore++; // Increment score for survival
        }
    }

    return 0;
}

// --- HELPER FUNCTIONS ---

/**
 * Reads the high score integer from disk.
 * Returns 0 if file is missing or corrupt.
 */
int loadHighScore() {
    FILE *fp = fopen(DATA_FILE, "r");
    int score = 0;
    
    // Always check if file pointer is valid 
    if (fp!= NULL) {
        // fscanf returns the number of items successfully read
        if (fscanf(fp, "%d", &score)!= 1) {
            score = 0; // Fallback if file is empty/corrupt
        }
        fclose(fp); // Always close the stream
    }
    return score;
}

/**
 * Writes the new high score to disk.
 * Uses "w" mode to overwrite previous content.
 */
void saveHighScore(int score) {
    FILE *fp = fopen(DATA_FILE, "w");
    if (fp!= NULL) {
        fprintf(fp, "%d", score);
        fclose(fp);
    } else {
        // Error handling for permissions issues
        printf("Error: Could not save high score to disk.\n");
    }
}

/**
 * Renders the game state to the console.
 * Uses ASCII art to represent lanes, player (P), and obstacles (X).
 */
void drawFrame(int playerX, int obstacleLane, int obstacleStep, int score, int highScore) {
    system("cls"); // Clear screen buffer (expensive but simple)

    // HUD (Heads Up Display)
    printf("==============================\n");
    printf(" LANE SURVIVOR | HIGH: %d\n", highScore);
    printf("==============================\n");
    printf(" Score: %d\n\n", score);

    printf(" |--- --- ---|\n");

    // Render the 10 rows of the track
    for (int i = 0; i < 10; i++) {
        printf(" |"); // Left border

        if (i == obstacleStep) {
            // Draw Obstacle Row
            if (obstacleLane == 0)      printf(" X | | |");
            else if (obstacleLane == 1) printf(" | X | |");
            else if (obstacleLane == 2) printf(" | | X |");
        } else {
            // Draw Empty Row
            printf(" | | |");
        }
        printf("\n");
    }

    // Render Player Row (Bottom)
    printf(" |");
    if (playerX == 0)      printf(" P | | |");
    else if (playerX == 1) printf(" | P | |");
    else if (playerX == 2) printf(" | | P |");
    printf("\n");

    printf(" |--- --- ---|\n");
}