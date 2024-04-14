#ifdef __linux__
#include "./engine/linux_platform.cpp"
#elif _WIN32
#include "./engine/win32_platform.cpp"
#endif

#include "./engine/assets.cpp"
#include "./game/game_render.cpp"
#include "./game/game_input.cpp"

#include "./game/game.cpp"