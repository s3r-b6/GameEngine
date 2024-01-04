# This should run from inside ./build

include=(-I/user/include/ -I../deps/)
libs=(-L/lib -lSDL2 -lGLEW -lGL -lGLU)

sources=(../src/main.cpp ../src/initialization.cpp)

echo "Compiling main.cpp..."
g++ "${include[@]}" "${libs[@]}" -o game.out "${sources[@]}"
