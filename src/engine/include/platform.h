#pragma once

#include "./engine_memory.h"
#include "./types.h"

#ifdef __linux__
#define plat_main() main(int argc, char *args[])
#define EXPORT_FN __attribute__((visibility("default")))
#include <unistd.h>
#define platform_sleep(ms) sleep(ms)
#elif _WIN32
#define plat_main()                                                                                \
    __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#define WIN32_LEAN_AND_MEAN
#define EXPORT_FN __declspec(dllexport)
#define platform_sleep(ms) Sleep(ms)
#include <windows.h>
#endif

#ifdef _WIN32
#define gameSharedObject "./game.dll"
#define loadedgameSharedObject "./game_load.dll"
#elif __linux__
#define gameSharedObject "./game.so"
#define loadedgameSharedObject "./game_load.so"
#endif

void *plat_loadDynamicLib(char *dll);
void *plat_loadDynamicFun(void *dll, char *funName);
bool plat_freeDynamicLib(void *dll);
u64 plat_getFileTimestamp(char *fileName);

char *plat_readFile(char *fileName, size_t *fileSize, BumpAllocator *allocator);
bool plat_copyFile(char *fileName, char *newFileName, BumpAllocator *allocator);

bool plat_writeFile(char *fileName);
bool plat_deleteFile(char *fileName);
