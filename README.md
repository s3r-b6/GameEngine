<div align="center"> <img src="/preview/game_crt.png" style="max-width: 100%;"/> </div>

# What this is:
This is a game engine pet project implemented in C++. 

This project is primarily intended for educational purposes and personal exploration of game engine architecture and design principles. Therefore, it certainly is not suitable for commercial game development projects without significant enhancements and optimizations.

The main goal is to learn about memory management, lower-level programming and software architecture. 

The engine provides a few basic tools without hiding any of the implementation details.
This aims to be a small game engine that could be used as a foundation for some 2D games.

## Implemented Features:
- **BumpAllocator based memory management**: Utilizes a permanent bump allocator and a transient bump allocator (can get invalidated) to handle memory instead of making atomic memory allocations.
- **Rendering Engine**: Utilizes OpenGL to render quads (to render tiles and sprites).
- **Cross-Platform**: Designed to run seamlessly on multiple platforms, Windows and Linux are supported right now, but should be easy enough to adapt to other platforms.
- **Hot-reloading**: Hot reloading of game-logic and shaders without losing the current game state.
- **Input Manager**: Management of input (keyboard and controller)
- **Entity-System**: While the engine has no concept of Systems, and is in no way an ECS, it has entities and components. This should be useful to bundle some common patterns: for example, Entities with a position in the world can use a Transform, and Entities that should render a sprite can use a SpriteRenderer.

<div align="center">  <img src="/preview/dynamic_rendering.gif" style="max-width: 100%;"/> </div>

# Intent:
I keep this public just in case anyone can learn anything from it, in the same way I have benefitted of similar efforts by other developers.
I welcome any comments and suggestions. This is my first time working in a game engine and much of it is rough and probably unfinished, and many decisions certainly are questionable.

# Assets:
- [Zelda-like tilesets and sprites](https://opengameart.org/content/zelda-like-tilesets-and-sprites)
- [Joystix font](https://www.1001fonts.com/joystix-font.html)

# Dependencies:
- [SDL2](https://www.libsdl.org/)
- [glad](https://glad.dav1d.de/)
- [glm](https://github.com/g-truc/glm)
- [stb_image](https://github.com/nothings/stb)
- [freetype](https://freetype.org/)

# Credits:
- [Cakez](https://www.youtube.com/@Cakez77)
- [Casey Muratori](https://www.youtube.com/c/MollyRocket)
- [jdh](https://www.youtube.com/@jdh)

# MIT License:
Copyright <2023> <Sergio Bermejo de las Heras>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
