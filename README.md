# Atari Basketball 1979 OpenGL Port

A simple port of 1979 atari basketball game implemented in C++ using OpenGL/GLUT.
<https://en.wikipedia.org/wiki/Basketball_(1979_video_game)>

## Description

This is a 2-player basketball game where players can compete against each other on the same screen. The game features basic basketball mechanics including shooting, stealing, and jumping.

## Features

- 2-player local multiplayer
- Basic basketball mechanics (shooting, stealing, jumping)
- Score tracking
- 1-minute game timer
- Textured player sprites
- Physics-based ball movement
- Basketball hoops with rim collision detection

## Technical Details

The game is built using:

- C++
- OpenGL/GLUT for graphics rendering
- STB Image library for texture loading

## Project Structure

- `main.cpp` - Contains the core game logic and rendering code
- `stb_image.h` - External library for image loading
- `Glut.h` - OpenGL utility toolkit header
- Various texture files for player sprites (not included in repository)

## Building the Project

The project is configured for Visual Studio with the following build configurations:

- Debug/Release
- x86/x64 platforms

Requirements:

- Visual Studio 2022 or later
- OpenGL
- GLUT library

## License

The project uses the following external libraries:

- STB Image: Available under MIT License or Public Domain (see license text in stb_image.h)

```7951:7967:Projekat/stb_image.h
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Development Notes

- The game uses a simple physics system for ball movement
- Player controls are keyboard-based
- Collision detection is implemented for:
  - Ball-rim interactions
  - Ball-player interactions
  - Player boundaries

## Future Improvements

Potential areas for enhancement:

1. Add sound effects
2. Implement AI for single-player mode
3. Add more advanced basketball mechanics
4. Add menu system and game options
