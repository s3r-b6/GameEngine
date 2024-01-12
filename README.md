# What this is:
This is a game engine pet project implemented in C++. 
The engine aims to provide developers with a basic set of tools without hiding anything from them.
The main goal is to learn about memory management, lower-level programming and software architecture. 
This aims to be a small but flexible game engine that could be used as a foundation for various types of 2D games.

This project is primarily intended for educational purposes and personal exploration of game engine architecture and design principles. Therefore, it certainly is not suitable for large-scale commercial game development projects without significant enhancements and optimizations.

# What this is not:
This project is not a full-fledged commercial game engine (like Unity or Unreal Engine). 
It aims to offer a comprehensive set of features for game development, but it does lack advanced functionalities and optimizations found in established game engines. Since this is my first attempt at something like this, the architecture can probably be improved by a lot.

## Key Features:
- **Rendering Engine**: Utilizes OpenGL to render quads (to render tiles and sprites) and TODO: particles
- **Cross-Platform Compatibility**: Designed to run seamlessly on multiple platforms, Windows and Linux are supported right now, but should be easy enough to adapt to other platforms.
- **Hot-reloading**: Hot reloading of game-logic without losing the current game state. TODO: Hot reloading of shaders and textures.
- WIP: **Entity-System**: While the engine has no concept of Systems, and is in no way an ECS engine, it has entities and components. This should be useful to bundle common patterns: for example, Entities with a position in the world can choose to use a Transform, and Entities that should render a sprite can choose to use a SpriteRenderer.
- TODO: **Physics Simulation**: Incorporates a simple physics engine to handle interactions between game objects.
- TODO: **Asset Management**: Management of game assets such as textures and audio files to optimize memory usage and loading times.
- TODO: **Input Manager**: Management of input (keyboard and controller)
- MAYBE TODO: **Scripting Support**: Provide scripting capabilities to make game development doable for level-designers and other non-technical individuals

#

I primarily keep this public in the hopes that anyone can learn anything from it, in the same way I have benefitted of similar efforts by other developers.

I welcome any comments, suggestions, bug fixes, and improvements. This is the first release of the code, and my first time working in a game engine and much of it is rough and unfinished.

# Dependencies:
- [SDL2](https://www.libsdl.org/)
- [glad](https://glad.dav1d.de/)
- [glm](https://github.com/g-truc/glm)
- [stb_image](https://github.com/nothings/stb)

# Credits:
- [Cakez](https://www.youtube.com/@Cakez77)
- [Casey Muratori](https://www.youtube.com/c/MollyRocket)
- [jdh](https://www.youtube.com/@jdh)

# MIT License:
Copyright <2023> <Sergio Bermejo de las Heras>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
