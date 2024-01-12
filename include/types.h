#pragma once

#define KB(x) 1024 * x
#define MB(x) KB(x) * 1024
#define GB(x) MB(x) * 1024

#define global static
#define local_persist static

// Int types
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef unsigned int uint;

// Forward declaration of common types
struct BumpAllocator;
struct GLContext;
struct RenderData;
struct ProgramState;
struct ImguiState;
struct GlobalState;
struct KeyState;

// KeyCodes
#include "SDL2/SDL_keycode.h"

// Math
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
