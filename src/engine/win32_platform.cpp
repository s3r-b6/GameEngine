// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include <windows.h>

#include "./engine_memory.h"
#include "./types.h"

void *plat_loadDynamicLib(char *dll) {
    HMODULE result = LoadLibraryA(dll);
    if (!result) engine_log("Failed to load dll");
    return result;
}

void *plat_loadDynamicFun(void *dll, char *funName) {
    FARPROC proc = GetProcAddress((HMODULE)dll, funName);
    if (!proc) engine_log("Failed to load dynamic function");
    return (void *)proc;
}

bool plat_freeDynamicLib(void *dll) {
    BOOL freeResult = FreeLibrary((HMODULE)dll);
    if (!freeResult) engine_log("Failed to free dynamic library");
    return (bool)freeResult;
}

u64 plat_getFileTimestamp(char *fileName) {
    HANDLE hFile;
    FILETIME ftLastWriteTime;
    ULARGE_INTEGER ull;

    hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        engine_log("Failed to open file for %s. Error code: %lu\n", fileName, GetLastError());
        return 0;
    }

    if (!GetFileTime(hFile, NULL, NULL, &ftLastWriteTime)) {
        engine_log("Failed to get file time for %s. Error code: %lu\n", fileName, GetLastError());
        CloseHandle(hFile);
        return 0;
    }

    // Close the file handle
    CloseHandle(hFile);

    // Convert FILETIME to ULONGLONG
    ull.LowPart = ftLastWriteTime.dwLowDateTime;
    ull.HighPart = ftLastWriteTime.dwHighDateTime;

    return ull.QuadPart;
}

char *plat_readFile(char *fileName, size_t *fileSize, BumpAllocator *allocator) {
    HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        engine_log("Failed to open file for reading: %s. Error: %lu", fileName, GetLastError());
        return nullptr;
    }

    LARGE_INTEGER fileSizeLarge;
    if (!GetFileSizeEx(hFile, &fileSizeLarge)) {
        engine_log("Failed to get file size for %s. Error: %lu", fileName, GetLastError());
        CloseHandle(hFile);
        return nullptr;
    }
    *fileSize = static_cast<size_t>(fileSizeLarge.QuadPart);

    char *memory = (char *)allocator->alloc(*fileSize + 1); // Allocate memory
    DWORD bytesRead;
    if (!ReadFile(hFile, memory, *fileSize, &bytesRead, NULL)) {
        engine_log("Failed to read file: %s. Error: %lu", fileName, GetLastError());
        CloseHandle(hFile);
        return nullptr;
    }
    CloseHandle(hFile);

    memory[*fileSize] = '\0'; // Null-terminate

    return memory;
}

bool plat_copyFile(char *fileName, char *newFileName, BumpAllocator *allocator) {
    size_t fileSize;
    char *fileContent = plat_readFile(fileName, &fileSize, allocator);
    if (!fileContent) { return false; }

    HANDLE hFile2 =
        CreateFile(newFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile2 == INVALID_HANDLE_VALUE) {
        engine_log("Failed to open file for writing: %s. Error: %lu", newFileName, GetLastError());
        return false;
    }

    DWORD bytesWritten;
    if (!WriteFile(hFile2, fileContent, fileSize, &bytesWritten, NULL)) {
        engine_log("Failed to write to file: %s. Error: %lu", newFileName, GetLastError());
        CloseHandle(hFile2);
        return false;
    }
    CloseHandle(hFile2);

    if (bytesWritten != fileSize) {
        engine_log("Failed to copy %s to %s. Incomplete write operation.", fileName, newFileName);
        return false;
    }

    return true;
}

bool plat_deleteFile(char *fileName) {
    if (DeleteFileA(fileName)) {
        engine_log("File '%s' deleted successfully.", fileName);
        return true;
    } else {
        DWORD errorCode = GetLastError();
        if (errorCode == ERROR_FILE_NOT_FOUND) {
            engine_log("File '%s' does not exist.", fileName);
            return true;
        } else {
            engine_log("Error deleting file '%s': %lu", fileName, errorCode);
            return false;
        }
    }
}
