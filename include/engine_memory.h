#pragma once

// Memory
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL_log.h"

#include "./types.h"
#include "engine_lib.h"

struct BumpAllocator {
    u32 size;
    u32 used;
    u8 *memory;

    explicit BumpAllocator(u32 len) {
        size = len;
        used = 0;
        memory = reinterpret_cast<u8 *>(malloc(len));

        if (!memory) {
            crash(__FILE__, __LINE__, "ERROR: Failed to allocate memory for the bumpAllocator!");
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
            crash(__FILE__, __LINE__, "ERROR: Not enough space in BumpAllocator");
            return result;
        }

        size += allignedSize;
        result = memory + used;
        used += allignedSize;

        return result;
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
