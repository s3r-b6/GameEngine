#pragma once

// Memory
#include <stdlib.h>
#include <string.h>

#include "./types.h"
#include "engine_lib.h"

struct BumpAllocator {
    u64 size;
    u64 used;
    u8 *memory;

    explicit BumpAllocator(u64 len) {
        size = len;
        used = 0;
        memory = reinterpret_cast<u8 *>(malloc(len));

        if (!memory) {
            crash("ERROR: Failed to allocate memory for the bumpAllocator!");
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
            crash("Not enough space in BumpAllocator");
            return result;
        }

        result = memory + used;
        used += allignedSize;

        return result;
    }

    // The trick is that the used attrib is used to calculate the offset.
    // An offset of 0 means just rewriting everything.
    void freeMemory() {
        memset(memory, 0, size);
        used = 0;
    }

    void destroy() {
        free(memory);
        size = 0;
        used = 0;
    }
};
