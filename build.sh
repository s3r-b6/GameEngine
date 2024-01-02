include=(-I/user/include/)
libs=(-L/lib -lSDL2 -lGLEW -lGL -lGLU)

sources=(../src/main.cpp ../src/initialization.cpp)

pushd ./build
echo "Compiling main.cpp..."
g++ "${include[@]}" "${libs[@]}" -o game.out "${sources[@]}"
popd
