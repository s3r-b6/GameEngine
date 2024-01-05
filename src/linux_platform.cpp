#include "memory.h"
#include "types.h"
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dlfcn.h>

#include "SDL2/SDL_log.h"

void *plat_loadDynamicLib(char *dlName) {
    void *sharedObject = dlopen(dlName, RTLD_NOW);
    if (!sharedObject) {
        SDL_Log("Failed to load dynamic library: %s", dlerror());
        return nullptr;
    }
    return sharedObject;
}

bool plat_freeDynamicLib(void *sharedObject) {
    int res = dlclose(sharedObject);
    if (res != 0) {
        SDL_Log("Failed to free dynamic library: %s", dlerror());
        return false;
    }
    return true;
}

void *plat_loadDynamicFun(void *sharedObject, char *funName) {
    void *function = dlsym(sharedObject, funName);
    if (!function) {
        SDL_Log("Failed to load dynamic function: %s", dlerror());
        return nullptr;
    }
    return function;
}

u64 plat_getFileTimestamp(char *fileName) {
    int fd = open(fileName, O_RDONLY);

    if (fd == -1) {
        SDL_Log("Failed to open file descriptor for %s", fileName);
        return 0;
    }

    struct stat filestat;
    fstat(fd, &filestat);

    close(fd);

    return filestat.st_mtime;
}

char *plat_readFile(char *fileName, size_t *fileSize,
                    BumpAllocator *allocator) {
    int fd = open(fileName, O_RDONLY);

    if (fd == -1) {
        SDL_Log("Failed to open file descriptor for %s", fileName);
        return nullptr;
    }

    struct stat filestat;
    fstat(fd, &filestat);

    size_t fSize = filestat.st_size;
    u8 *memory =
        allocator->alloc(fSize + 1); // request fSize (alligned) bytes of memory

    size_t bytesRead = read(fd, memory, fSize);
    close(fd);

    memory[bytesRead] = 0; // null-terminate

    if (bytesRead != fSize) {
        SDL_Log("bytesRead does not match filesize for %s", fileName);
        return nullptr;
    }

    *fileSize = fSize;
    return (char *)memory;
}

bool plat_copyFile(char *fileName, char *newFileName,
                   BumpAllocator *allocator) {
    size_t fSize;
    char *f1_ptr = plat_readFile(fileName, &fSize, allocator);

    int fd2 = open(newFileName, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd2 == -1) {
        SDL_Log("Failed to open file descriptor for %s", newFileName);
        return false;
    }

    int written = write(fd2, f1_ptr, fSize);

    if (written != fSize) {
        SDL_Log("Failed to copy %s to %s", fileName, newFileName);
        return false;
    }

    close(fd2);

    return true;
}
