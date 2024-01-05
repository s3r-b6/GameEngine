# This should run from inside ./build

include=( -I../deps/ )

libs=(
	-L/lib 
	-lSDL2 
	-lGLEW 
	-lGL 
	# -lGLU
)

sources_main=(
	../src/main.cpp 
	../src/initialization.cpp 
	../src/assets.cpp 
	../src/renderer.cpp 
)

sources_game=(
	../src/assets.cpp 
	../src/renderer.cpp 
)

flags=( -Wno-write-strings )

timestamp=$(date '+%s')

current_directory=$(pwd)

if [[ "$current_directory" != *"build" ]]; then
	echo "Running from wrong directory $current_directory"
	echo "Should be running from ./build..."
	exit 1
fi

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
