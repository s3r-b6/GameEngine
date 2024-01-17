# This should run from inside ./build
if [[ $(pwd) != *"build" ]]; then
	echo "Running from wrong directory!"
	echo "Should be running from ./build..."
	exit 1
fi

include=(-I../deps/)

# SDL creates a config file with flags specific to each OS
# This is dumb, but for now it will stay like this
include+=(-I../deps/linux/) 
include+=(-I../deps/linux/SDL2)
include+=(-I../include)

include+=(-L/lib)
include+=(-lSDL2)
include+=(-lGL)

sources_main=(../src/main.cpp)
sources_main+=(../src/linux_platform.cpp)
sources_main+=(../src/initialization.cpp)
sources_main+=(../src/assets.cpp)
sources_main+=(../src/renderer.cpp)
sources_main+=(../deps/glad/glad.c)

sources_game=(../src/assets.cpp)
sources_game+=(../src/renderer.cpp)
sources_game+=(../src/linux_platform.cpp)
sources_game+=(../deps/glad/glad.c)

flags=(-pipe -Wno-write-strings -D_REENTRANT)

gamesrc_stamp=$(stat -c %Y ../src/game.cpp)
gameobj_stamp=0  

if [ -f "./game.so" ]; then
    gameobj_stamp=$(stat -c %Y ./game.so)
fi

echo "Trying to recompile game.cpp"
# This is a bit naive, but for now is OKish
if [ "$gamesrc_stamp" -gt "$gameobj_stamp" ]; then
    timestamp=$(date '+%s')
    echo "Game.cpp file is newer. Recompiling..."
    echo "Compiling game_$timestamp.so..."
    g++ -fPIC "${include[@]}" "${flags[@]}" -shared -o "game_$timestamp.so" ../src/game.cpp "${sources_game[@]}"
    echo "Renaming game_$timestamp.so to game.so"
    mv "./game_$timestamp.so" ./game.so
else 
    echo "Game.cpp file is not newer. No need to recompile."
fi

if not pgrep game > /dev/null; then
    echo "Game is not running"
    echo "Compiling main.cpp..."
    g++ "${include[@]}" "${flags[@]}" -o game.out "${sources_main[@]}"
else 
    echo "Game is running, skipping main.cpp..."
fi

echo "Compilation completed."
