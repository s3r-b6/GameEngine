#ifdef __linux__
#include "./engine/linux_platform.cpp"
#elif _WIN32
#include "./engine/win32_platform.cpp"
#endif

#include "glad.c"

#include "./engine/assets.cpp"
#include "./engine/fonts.cpp"
#include "./engine/initialization.cpp"
#include "./engine/renderer.cpp"

#include "./engine/main.cpp"