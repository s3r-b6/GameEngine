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


# TODO: probably not every single thing is needed
sources_main+=(../deps/imgui/imgui.cpp) 
sources_main+=(../deps/imgui/imgui_draw.cpp)
sources_main+=(../deps/imgui/imgui_tables.cpp) 
sources_main+=(../deps/imgui/imgui_widgets.cpp)
sources_main+=(../deps/imgui/backends/imgui_impl_sdl2.cpp)
sources_main+=(../deps/imgui/backends/imgui_impl_opengl3.cpp)


sources_game=(../src/assets.cpp)
sources_game+=(../src/renderer.cpp)
sources_game+=(../deps/glad/glad.c)
sources_game+=(../src/linux_platform.cpp)

# TODO: probably not every single thing is needed
sources_game+=(../deps/imgui/imgui.cpp) 
sources_game+=(../deps/imgui/imgui_draw.cpp)
sources_game+=(../deps/imgui/imgui_tables.cpp) 
sources_game+=(../deps/imgui/imgui_widgets.cpp)
sources_game+=(../deps/imgui/backends/imgui_impl_sdl2.cpp)
sources_game+=(../deps/imgui/backends/imgui_impl_opengl3.cpp)

flags=(-Wno-write-strings -D_REENTRANT)

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
    g++ "${include[@]}" "${libs[@]}" "${flags[@]}" -o game.out "${sources_main[@]}"
    echo "Compiling main.cpp..."
    echo "Compilation completed."
fi
