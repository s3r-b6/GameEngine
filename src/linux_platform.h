#include "memory.h"
#include "types.h"
#include <unistd.h>

#include <dlfcn.h>

inline void *plat_load_dynamic_lib(const char *dlName) {
    void *sharedObject = dlopen(dlName, RTLD_NOW);
    if (!sharedObject) {
        SDL_Log("Failed to load dynamic library: %s", dlerror());
        return nullptr;
    }
    return sharedObject;
}

inline bool plat_free_dynamic_lib(void *sharedObject) {
    int res = dlclose(sharedObject);
    if (res != 0) {
        SDL_Log("Failed to free dynamic library: %s", dlerror());
        return false;
    }
    return true;
}

inline void *plat_load_dynamic_fun(void *sharedObject, const char *funName) {
    void *function = dlsym(sharedObject, funName);
    if (!function) {
        SDL_Log("Failed to load dynamic function: %s", dlerror());
        return nullptr;
    }
    return function;
}
