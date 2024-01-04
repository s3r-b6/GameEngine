#include "memory.h"

#include "engine_lib.h"

#include <cstdint>
#include <windows.h>

void *plat_loadDyinamicLib(char *dll) {
    HMODULE result = LoadLibraryA(dll);
    if (!result) crash("Failed to load dll");
    return result;
}

void *plat_loadDynamicFun(void *dll, char *funName) {
    FARPROC proc = GetProcAddress((HMODULE)dll, funName);
    if (!proc) crash("Failed to load dynamic function");
    return (void *)proc;
}

bool plat_freeDynamicLib(void *dll) {
    BOOL freeResult = FreeLibrary((HMODULE)dll);
    if (!freeResult) crash("Failed to free dynamic library");
    return (bool)freeResult;
}

char *BumpAllocator::plat_readFile(char const *fileName, size_t *fileSize) {
    HANDLE hFile = CreateFile(fileName,        // file name
                              GENERIC_READ,    // desired access (read-only)
                              FILE_SHARE_READ, // share mode
                              NULL,            // security attributes
                              OPEN_EXISTING,   // open only if exists
                              FILE_ATTRIBUTE_NORMAL, // file attributes
                              NULL                   // template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        SDL_Log("Failed to open file for reading: %s. Error: %lu", fileName,
                GetLastError());
        return nullptr;
    }

    LARGE_INTEGER fileSizeLarge;
    if (!GetFileSizeEx(hFile, &fileSizeLarge)) {
        SDL_Log("Failed to get file size for %s. Error: %lu", fileName,
                GetLastError());
        CloseHandle(hFile);
        return nullptr;
    }
    *fileSize = static_cast<size_t>(fileSizeLarge.QuadPart);

    char *memory = (char *)alloc(*fileSize + 1); // Allocate memory
    DWORD bytesRead;
    if (!ReadFile(hFile, memory, *fileSize, &bytesRead, NULL)) {
        SDL_Log("Failed to read file: %s. Error: %lu", fileName,
                GetLastError());
        CloseHandle(hFile);
        return nullptr;
    }
    CloseHandle(hFile);

    memory[*fileSize] = '\0'; // Null-terminate

    return memory;
}

bool BumpAllocator::plat_copyFile(char *const fileName,
                                  char *const newFileName) {
    size_t fileSize;
    char *fileContent = plat_readFile(fileName, &fileSize);
    if (!fileContent) { return false; }

    HANDLE hFile2 = CreateFile(newFileName,   // file name
                               GENERIC_WRITE, // desired access (write-only)
                               0,             // share mode
                               NULL,          // security attributes
                               CREATE_ALWAYS, // create or overwrite if exists
                               FILE_ATTRIBUTE_NORMAL, // file attributes
                               NULL                   // template file
    );

    if (hFile2 == INVALID_HANDLE_VALUE) {
        SDL_Log("Failed to open file for writing: %s. Error: %lu", newFileName,
                GetLastError());
        return false;
    }

    DWORD bytesWritten;
    if (!WriteFile(hFile2, fileContent, fileSize, &bytesWritten, NULL)) {
        SDL_Log("Failed to write to file: %s. Error: %lu", newFileName,
                GetLastError());
        CloseHandle(hFile2);
        return false;
    }
    CloseHandle(hFile2);

    if (bytesWritten != fileSize) {
        SDL_Log("Failed to copy %s to %s. Incomplete write operation.",
                fileName, newFileName);
        return false;
    }

    return true;
}
