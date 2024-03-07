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

echo "Trying to recompile game.cpp"
# This is a bit naive, but for now is OKish
timestamp=$(date '+%s')
echo "Recompiling game.cpp..."
echo "Compiling game_$timestamp.so..."
g++ -fPIC -g "${include[@]}" "${flags[@]}" -shared -o "game_$timestamp.so" ../src/build_game.cpp
echo "Renaming game_$timestamp.so to game.so"
mv "./game_$timestamp.so" ./game.so

if not pgrep game > /dev/null; then
    echo "Game is not running"
    echo "Compiling main.cpp..."
    g++ -g "${include[@]}" "${flags[@]}" -o game.out ../src/build_engine.cpp
else 
    echo "Game is running, skipping main.cpp..."
fi

echo "Compilation completed."
