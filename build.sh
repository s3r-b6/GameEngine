# This should run from inside ./build
if [[ $(pwd) != *"build" ]]; then
	echo "Running from wrong directory!"
	echo "Should be running from ./build..."
	exit 1
fi

include=( -I../deps/ -I../src/headers)

libs=( -L/lib -lSDL2 -lGLEW -lGL )

sources_main=(
	../src/main.cpp 
	../src/linux_platform.cpp
	../src/initialization.cpp 
	../src/assets.cpp 
	../src/renderer.cpp 
	../deps/glad/glad.c
)

sources_game=(
	../src/assets.cpp 
	../src/renderer.cpp 
	../deps/glad/glad.c
	../src/linux_platform.cpp
)

flags=( -Wno-write-strings )

timestamp=$(date '+%s')

if pgrep game > /dev/null; then
    echo "Game is already running."

    echo "Compiling game_$timestamp.so..."
    g++ -fPIC "${include[@]}" "${libs[@]}" "${flags[@]}" -shared -o "game_$timestamp.so" ../src/game.cpp "${sources_game[@]}"
	echo "Renaming game_$timestamp.so to game.so"
	mv "./game_$timestamp.so" ./game.so

else
    echo "Game is not running..."

	echo "Cleaning build folder..."
	rm -rf ./*

    echo "Compiling game_$timestamp.so..."
    g++ -fPIC "${include[@]}" "${libs[@]}" "${flags[@]}" -shared -o "game_$timestamp.so" ../src/game.cpp "${sources_game[@]}"
	echo "Renaming game_$timestamp.so to game.so"
	mv "./game_$timestamp.so" ./game.so

    # Compile main.cpp
    echo "Compiling main.cpp..."
    g++ "${include[@]}" "${libs[@]}" "${flags[@]}" -o game.out "${sources_main[@]}"

    echo "Compilation completed."
fi
