// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./engine_memory.h"
#include "./types.h"

void *plat_loadDynamicLib(char *dlName) {
    void *sharedObject = dlopen(dlName, RTLD_NOW);
    char *error = dlerror();
    if (error != NULL || !sharedObject) {
        engine_log("dlerror: %s", error);
        engine_log("Failed to load dynamic library");
        return nullptr;
    }
    return sharedObject;
}

bool plat_freeDynamicLib(void *sharedObject) {
    int res = dlclose(sharedObject);
    char *error = dlerror();
    if (error != NULL || res != 0) {
        engine_log("Failed to free dynamic library: %s", error);
        return false;
    }
    return true;
}

void *plat_loadDynamicFun(void *sharedObject, const char *funName) {
    engine_log("Loading %s from: %p", funName, sharedObject);
    void *function = dlsym(sharedObject, funName);
    char *error = dlerror();
    if (error) engine_log("dlerror: %s", error);
    if (!function) {
        engine_log("Failed to load dynamic function");
        return nullptr;
    }
    return function;
}

u64 plat_getFileTimestamp(char *fileName) {
    int fd = open(fileName, O_RDONLY);

    if (fd == -1) {
        engine_log("Failed to open file descriptor for %s", fileName);
        return 0;
    }

    struct stat filestat;
    fstat(fd, &filestat);

    close(fd);

    return filestat.st_mtime;
}

char *plat_readFile(char *fileName, size_t *fileSize, BumpAllocator *allocator) {
    int fd = open(fileName, O_RDONLY);

    if (fd == -1) {
        engine_log("Failed to open file descriptor for %s", fileName);
        return nullptr;
    }

    struct stat filestat;
    fstat(fd, &filestat);

    size_t fSize = filestat.st_size;

    // request alligned fSize bytes of memory
    u8 *memory = allocator->alloc(fSize + 1);

    size_t bytesRead = read(fd, memory, fSize);
    close(fd);

    // null-terminate
    memory[bytesRead] = 0;

    if (bytesRead != fSize) {
        engine_log("bytesRead does not match filesize for %s", fileName);
        return nullptr;
    }

    *fileSize = fSize;
    return reinterpret_cast<char *>(memory);
}

bool plat_copyFile(char *fileName, const char *newFileName, BumpAllocator *allocator) {
    size_t fSize;
    char *f1_ptr = plat_readFile(fileName, &fSize, allocator);
    if (!f1_ptr) {
        engine_log("Failed to read file: %s", fileName);
        return false;
    }

    int fd2 = open(newFileName, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd2 == -1) {
        engine_log("Failed to open file: %s", newFileName);
        return false;
    }

    ssize_t written = write(fd2, f1_ptr, fSize);
    if (written == -1 || static_cast<size_t>(written) != fSize) {
        engine_log("Failed to write file: %s", newFileName);
        close(fd2);
        return false;
    }
    engine_log("Wrote %zd bytes to %s", written, newFileName);

    close(fd2);
    return true;
}

bool plat_deleteFile(char *fileName) {
    if (unlink(fileName) == 0) {
        engine_log("File '%s' deleted successfully.", fileName);
        return true;
    } else if (errno == ENOENT) {
        perror("unlink");
        engine_log("File '%s' does not exist.", fileName);
        return true;
    } else {
        perror("unlink");
        engine_log("Error deleting file: %s", strerror(errno));
        return false;
    }
}
