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

![Start Screen](StartScreen.png)

## Notes

-AI paddle speed is constant; it tracks the ball without delay.

-Scoring to 5 triggers a reset after a brief pause.

-MAX_BOUNCE_ANGLE controls ball deflection angles (60° max).

-Uses custom SDL user events to cleanly trigger ball resets.

## Learning Goals

-This project was built to explore:

-SDL2 fundamentals (rendering, input, audio, timing)

-Event-driven architecture using SDL_USEREVENT

-Game loop structuring (input → update → render)

-Collision detection (AABB) and response

-Minimal use of RAII and memory management (manual cleanup)

## Credits

Built by Adrian Ascanio

Inspired by the original Pong (Atari, 1972)

Sound and font assets used under open licenses

## Dependencies

Make sure these are installed:

- [SDL2](https://www.libsdl.org/)
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL_mixer](https://github.com/libsdl-org/SDL_mixer)

On Linux (example):

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev
