# Assignment 2 — Analysis of the existing game

**Source:** Student fork of `https://github.com/shoonyank/game` (follow assignment instructions).
**Assignment reference (uploaded):** Final Assignment PDF. :contentReference[oaicite:1]{index=1}

---

## 1. Overview / Purpose
This document summarizes the game's structure and suggests how to add the requested features:
- Start Menu (new)
- Sound Effects (high-level)

## 2. Typical program structure (observed / expected)
Most simple C console games in this repo use the following structure:

- `main()` — game loop control and top-level initialization
- `init()` / `cleanup()` — set up state and resources
- `game_loop()` — core loop: input -> update -> render -> delay
- `render()` — prints game state to the console (ASCII)
- `process_input()` — handles keyboard input (blocking or polled)
- `update()` — applies game physics and rules (collision detection, score)
- `data` — score, player position, obstacles, level data
- Optional: functions for saving/loading high scores, audio, menus

## 3. Program flow (recommended)
1. Initialize platform-specific libs (e.g., ncurses on Linux).
2. Show **Start Menu**; read selection (Start, Instructions, Quit).
3. When user selects Start:
   - Initialize game state, optional assets
   - Possibly play a short "start" sound
4. Enter game loop:
   - Poll input
   - Update positions and collision
   - Render frame
   - Play sound on specific events (collision, life lost)
   - Check for game over
5. On game over, display scores and ask to restart or return to menu.

## 4. Where to add the two requested features
### Start Menu (simple)
- Add a `showStartMenu()` function that loops until a valid selection is made.
- Integrate at program start in `main()` before entering the game instance.
- Menu options: `1) Start game  2) Instructions  3) Exit`
- Implement using standard console I/O or, preferably, the same rendering/input technique used by the game (e.g., ncurses).

### Sound Effects (High-Level)
- Two implementation levels:
  - **Low-effort, cross-platform hint:** use ASCII bell `'\a'` for a short beep on events (works in many terminals).
  - **Better:** call an external audio player (Linux: `aplay`/`paplay`) or use SDL2/SDL_mixer for programmatic audio playback.
- Provide a wrapper function `playSound(const char *event)` that:
  - Uses a simple `switch` to select which sound to play
  - Falls back to `'\a'` if audio binary or SDL is not available
- Since this is a "High-Level" modification per assignment, document that you used a wrapper which invokes system audio or SDL (explain required libraries).

## 5. Example files I added for grading
- `README_PART1.md` (this file)
- `game_with_menu_and_sound.c` — standalone example showing how to add a start menu and play simple sound (copy into your repo and adapt)
  - Build: `gcc -o game_with_menu_and_sound game_with_menu_and_sound.c -lncurses` (if compiled with ncurses)
  - Or compile plain console version with `gcc -o game_with_menu_and_sound game_with_menu_and_sound.c`

## 6. Notes on integration
- If the repository uses `conio.h` or Windows-specific functions, adapt the sample functions accordingly.
- If you wish to use actual .wav sound files inside the repo, add them under `/assets/sounds/` and invoke `aplay` (Linux) or `PowerShell`/`PlaySound` on Windows, or use SDL2 for a cross-platform approach.
- Document any additional external libraries in the top-level README (e.g., `sudo apt install libncurses5-dev libncursesw5-dev` or `libsdl2-dev`).

---

If any merge conflicts arise when you add these files to the original repo, let me know the filenames and I will produce a patch.
