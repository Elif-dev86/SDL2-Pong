# SDL2 Pong Clone

A modern Pong clone written in C++ using SDL2, SDL_ttf, and SDL_mixer. 
This project recreates the classic gameplay of Pong while demonstrating game architecture concepts like 
event-driven design, audio/visual feedback, collision physics, and a clean game loop.

## Features

- Classic Pong gameplay: 1-player vs AI
- Custom event system (e.g., for ball resets)
- Angle-based ball reflection depending on paddle hit position
- Sound effects for paddle hits, wall bounces, and scoring
- "Press SPACE to Start" intro mechanic
- Victory condition with automatic score reset
- Basic AI opponent (tracks ball vertically)
- Frame rate control with FPS display
- Modular functions for input, update, render, and collision logic

## Controls

| Key         | Action              |
|-------------|---------------------|
| `↑` / `↓`   | Move player paddle  |
| `SPACE`     | Start game / serve  |
| `ESC`       | Quit game           |

## Dependencies

Make sure these are installed:

- [SDL2](https://www.libsdl.org/)
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL_mixer](https://github.com/libsdl-org/SDL_mixer)

On Linux (example):

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev
