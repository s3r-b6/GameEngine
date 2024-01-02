include=(-I/user/include/)
libs=(-L/lib -lSDL2 -lGLEW -lGL -lGLU)

sources=(./src/main.cpp ./src/initialization.cpp)

echo "Compiling main.cpp..."
g++ "${include[@]}" "${libs[@]}" -o ./build/game.out "${sources[@]}"
