#pragma once

#ifdef __linux__
#define EXPORT_FN
#include "linux_platform.h"
#elif _WIN32
#define EXPORT_FN __declspec(dllexport)
#endif

bool plat_load_dynamic_lib(char *const funName);
bool plat_free_dynamic_lib(void *sharedObject);
bool plat_load_dynamic_fun(void *sharedObject, char *const funName);
