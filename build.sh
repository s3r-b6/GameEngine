include=(-I/user/include/)
libs=(-L/lib -lSDL2 -lGLEW -lGL -lGLU)

echo "Compiling main.cpp..."
g++ "${include[@]}" "${libs[@]}" -o ./build/game.out ./src/main.cpp
