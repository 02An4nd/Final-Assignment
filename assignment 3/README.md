# BREAKOUT – Assignment 3

A simple *console-based Breakout game* written in C using Windows API cursor control.  
The game features brick breaking, paddle movement, real-time ball physics, scoring, and lives.

---

## 📌 Features
- Real-time ball movement with float-based physics  
- Paddle control using Arrow Keys or A/D  
- Brick grid (5 × 10) with destruction  
- Score tracking and life system  
- Smooth rendering using SetConsoleCursorPosition  
- Borders, HUD display, and win/lose screens  

---

## 🎮 Controls
| Key | Action |
|-----|--------|
| *A* / *Left Arrow* | Move paddle left |
| *D* / *Right Arrow* | Move paddle right |

Game ends when you clear all bricks or lose all lives.

---

## ⚙ Compiling & Running

### *MinGW (GCC)*
gcc breakout.c -o breakout.exe
./breakout.exe


### *MSVC (Visual Studio Developer Prompt)*


cl breakout.c
breakout.exe


---

## 📁 Files
- breakout.c – main game source  
- README.md – documentation  

---

## 📝 Summary
This project demonstrates a functional Breakout game using Windows console rendering, AABB collisions, paddle-ball physics, and a simple real-time game loop suitable for Assessment 3.