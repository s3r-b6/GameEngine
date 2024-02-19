#ifdef __linux__
#include "linux_platform.cpp"
#elif _WIN32
#include "win32_platform.cpp"
#endif

#include "assets.cpp"
#include "game_render.cpp"

#include "game.cpp"
