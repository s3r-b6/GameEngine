# This should run from inside ./build
if [[ $(pwd) != *"build" ]]; then
    echo "Not running from build directory."
    echo "Attempting to change directory to build..."

    if cd build; then
        echo "Successfully changed to build directory."
    else
        echo "Failed to change to build directory. Aborting script."
        exit 1
    fi
fi

include=(-I../deps/)

# SDL creates a config file with flags specific to each OS
# This is dumb, but for now it will stay like this
include+=(-I../deps/freetype2/) 
include+=(-I../deps/glad/) 
include+=(-I../deps/linux/) 
include+=(-I../deps/linux/SDL2)
include+=(-I../include)

include+=(-L/lib)
include+=(-lfreetype)
include+=(-lSDL2)
include+=(-lGL)

flags=(-pipe -Wno-write-strings -D_REENTRANT)

gamesrc_stamp=$(stat -c %Y ../src/game.cpp)
gameobj_stamp=0  

if [ -f "./game.so" ]; then
    gameobj_stamp=$(stat -c %Y ./game.so)
fi

gamehdr_stamp=$(stat -c %Y ../include/game.h)
assets_stamp=$(stat -c %Y ../src/assets.cpp)
renderer_stamp=$(stat -c %Y ../src/renderer.cpp)

echo "Trying to recompile game.cpp"
# This is a bit naive, but for now is OKish
if [ "$gamesrc_stamp" -gt "$gameobj_stamp"  ] ||
   [ "$renderer_stamp" -gt "$gameobj_stamp" ] ||
   [ "$gamehdr_stamp" -gt "$gameobj_stamp" ] ||
   [ "$assets_stamp" -gt "$gameobj_stamp" ]; 
then
    timestamp=$(date '+%s')
    echo "Game.cpp file is newer. Recompiling..."
    echo "Compiling game_$timestamp.so..."
    g++ -fPIC "${include[@]}" "${flags[@]}" -shared -o "game_$timestamp.so" ../src/build_game.cpp
    echo "Renaming game_$timestamp.so to game.so"
    mv "./game_$timestamp.so" ./game.so
else 
    echo "Game.cpp file is not newer. No need to recompile."
fi

if not pgrep game > /dev/null; then
    echo "Game is not running"
    echo "Compiling main.cpp..."
    g++ "${include[@]}" "${flags[@]}" -o game.out ../src/build_engine.cpp
else 
    echo "Game is running, skipping main.cpp..."
fi

echo "Compilation completed."
