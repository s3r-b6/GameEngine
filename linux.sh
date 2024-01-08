# This should run from inside ./build
if [[ $(pwd) != *"build" ]]; then
	echo "Running from wrong directory!"
	echo "Should be running from ./build..."
	exit 1
fi

# TODO: This would benefit of a bit of cleaning...
include=(-I../deps/)

# SDL creates a config file with flags specific to each OS
# This is dumb, but for now it will stay like this
include+=(-I../deps/linux/) 
include+=(-I../deps/linux/SDL2)
include+=(-I../deps/imgui/)
include+=(-I../src/headers)

libs=(-L/lib)
libs+=(-lSDL2)
libs+=(-lGLEW)
libs+=(-lGL)

sources_main=(../src/main.cpp)
sources_main+=(../src/linux_platform.cpp)
sources_main+=(../src/initialization.cpp)
sources_main+=(../src/assets.cpp)
sources_main+=(../src/renderer.cpp)
sources_main+=(../deps/glad/glad.c)
sources_main+=(./imgui.so)

sources_game=(../src/assets.cpp)
sources_game+=(../src/renderer.cpp)
sources_game+=(../src/linux_platform.cpp)
sources_game+=(../deps/glad/glad.c)
sources_game+=(./imgui.so)

# TODO: probably not every single thing is needed
sources_imgui=(../deps/imgui/imgui.cpp) 
sources_imgui+=(../deps/imgui/imgui_draw.cpp)
sources_imgui+=(../deps/imgui/imgui_tables.cpp) 
sources_imgui+=(../deps/imgui/imgui_widgets.cpp)
sources_imgui+=(../deps/imgui/backends/imgui_impl_sdl2.cpp)
sources_imgui+=(../deps/imgui/backends/imgui_impl_opengl3.cpp)

flags=(-Wno-write-strings -D_REENTRANT)

if not [ -f "./imgui.so" ]; then
    echo "Imgui.so not found, compiling..."
    g++ -fPIC -pipe "${sources_imgui[@]}" -I../deps/imgui/ -I../deps/imgui/backends -I../deps/linux/SDL2 -shared -o imgui.so
fi

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
    g++ -pipe -fPIC "${include[@]}" "${libs[@]}" "${flags[@]}" -shared -o "game_$timestamp.so" ../src/game.cpp "${sources_game[@]}"
    echo "Renaming game_$timestamp.so to game.so"
    mv "./game_$timestamp.so" ./game.so
else 
    echo "Game.cpp file is not newer. No need to recompile."
fi

if not pgrep game > /dev/null; then
    echo "Game is not running"
    echo "Compiling main.cpp..."
    g++ -pipe "${include[@]}" "${libs[@]}" "${flags[@]}" -o game.out "${sources_main[@]}"
else 
    echo "Game is running, skipping main.cpp..."
fi

echo "Compilation completed."
