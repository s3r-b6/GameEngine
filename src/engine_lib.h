#pragma once

#include <GL/glew.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define KB(x) 1024 * x
#define MB(x) KB(x) * 1024
#define GB(x) MB(x) * 1024

struct BumpAllocator {
    size_t size;
    size_t used;
    u8 *memory;

    BumpAllocator(size_t size);
    u8 *alloc(size_t size);
    void freeBumpAlloc();

    char *readFile(const char *fileName, size_t *fileSize);
};

BumpAllocator::BumpAllocator(size_t len) {
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

void BumpAllocator::freeBumpAlloc() {
    free(memory);
    size = 0;
    used = 0;
}

u8 *BumpAllocator::alloc(size_t len) {
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

char *BumpAllocator::readFile(const char *fileName, size_t *fileSize) {
    int fd = open(fileName, O_RDONLY);

    if (fd == -1) {
        SDL_Log("Failed to open file descriptor for %s", fileName);
        return nullptr;
    }

    struct stat filestat;
    fstat(fd, &filestat);

    size_t fSize = filestat.st_size;
    u8 *memory = alloc(fSize);

    size_t bytesRead = read(fd, memory, fSize);

    close(fd);

    if (bytesRead != fSize) {
        SDL_Log("bytesRead does not match filesize for %s", fileName);
        return nullptr;
    }

    *fileSize = fSize;
    return (char *)memory;
}
