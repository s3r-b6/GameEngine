#pragma once

#include "types.h"

#ifdef __linux__
#define EXPORT_FN
#include "linux_platform.h"
#define platform_sleep(ms) sleep(ms)
#elif _WIN32
#include "win32_platform.h"
#define EXPORT_FN __declspec(dllexport)
#define platform_sleep(ms) Sleep(ms)
#endif

u64 plat_getFileTimestamp(char const *fileName);

void* plat_loadDynamicLib(char *const funName);
bool plat_freeDynamicLib(void *sharedObject);
void *plat_loadDynamicFun(void *sharedObject, char *const funName);

// TODO: Write this for win32
// char *BumpAllocator::readFile(const char *fileName, size_t *fileSize);
// bool BumpAllocator::copyFile(char *const fileName, char *const
// newFileName);
