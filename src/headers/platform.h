#pragma once

#include "types.h"

#ifdef __linux__
#define EXPORT_FN
#include <unistd.h>
#define platform_sleep(ms) sleep(ms)
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#define EXPORT_FN __declspec(dllexport)
#define platform_sleep(ms) Sleep(ms)
#include <windows.h>
#endif

#include "memory.h"

void *plat_loadDynamicLib(char *dll);
void *plat_loadDynamicFun(void *dll, char *funName);
bool plat_freeDynamicLib(void *dll);
u64 plat_getFileTimestamp(char *fileName);

char *plat_readFile(char *fileName, size_t *fileSize, BumpAllocator *allocator);
bool plat_copyFile(char *fileName, char *newFileName, BumpAllocator *allocator);
