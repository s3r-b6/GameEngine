#pragma once

#include <SDL2/SDL.h>

#include "types.h"

// Memory
#include <stdlib.h>
#include <string.h>

// Files
#include <fcntl.h>    // TODO: This is linux specific
#include <sys/stat.h> // TODO: This is linux specific
#include <unistd.h>   // TODO: This is linux specific

struct BumpAllocator {
    u32 size;
    u32 used;
    u8 *memory;

    BumpAllocator(u32 len) {
        size = len;
        used = 0;
        memory = (u8 *)malloc(len);

        if (!memory) {
            SDL_Log("ERROR: Failed to allocate memory for the bumpAllocator!");
            size = 0;
            return;
        }

        memset(memory, 0, size);
    }

    u8 *alloc(size_t len) {
        u8 *result = nullptr;

        // ( l+7 ) & ~7 -> First 3 bits are empty, i.e., it is a multiple of 8.
        size_t allignedSize = (len + 7) & ~7;

        if (used + allignedSize > size) {
            SDL_Log("ERROR: Not enough space in BumpAllocator");
            return result;
        }

        size += allignedSize;
        result = memory + used;
        used += allignedSize;

        return result;
    }

    // TODO: This is linux specific
    char *readFile(const char *fileName, size_t *fileSize) {
        int fd = open(fileName, O_RDONLY);

        if (fd == -1) {
            SDL_Log("Failed to open file descriptor for %s", fileName);
            return nullptr;
        }

        struct stat filestat;
        fstat(fd, &filestat);

        size_t fSize = filestat.st_size;
        u8 *memory =
            alloc(fSize + 1); // request fSize (alligned) bytes of memory

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

    // TODO: This is linux specific
    bool copyFile(char *const fileName, char *const newFileName) {
        size_t fSize;
        char *f1_ptr = readFile(fileName, &fSize);

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

    // The trick is that the used attrib is used to calculate the offset.
    // An offset of 0 means just rewriting everything.
    void freeMemory() { used = 0; }

    void destroy() {
        free(memory);
        size = 0;
        used = 0;
    }
};

// TODO: This is linux specific
inline u64 getFileTimestamp(char const *fileName) {
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
