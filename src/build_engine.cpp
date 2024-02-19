#ifdef __linux__
#include "linux_platform.cpp"
#elif _WIN32
#include "win32_platform.cpp"
#endif

#include "glad.c"

#include "assets.cpp"
#include "fonts.cpp"
#include "initialization.cpp"
#include "renderer.cpp"

#include "main.cpp"
