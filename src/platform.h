#pragma once

#ifdef __linux__
#define EXPORT_FN
#include "linux_platform.h"
#elif _WIN32
#define EXPORT_FN __declspec(dllexport)
#endif

u64 plat_getFileTimestamp(char const *fileName);

bool plat_load_dynamic_lib(char *const funName);
bool plat_freeDynamicLib(void *sharedObject);
bool plat_load_dynamic_fun(void *sharedObject, char *const funName);

// TODO: Write this for win32
// char *BumpAllocator::readFile(const char *fileName, size_t *fileSize);
// bool BumpAllocator::copyFile(char *const fileName, char *const
// newFileName);
