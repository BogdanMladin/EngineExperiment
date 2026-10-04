/**
@file Paladin.h
@brief brief for the file.

Longer for the file.
*/

#include <cassert>
#include <math.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

typedef struct offscreen_buffer
{
    void *memory;
    int32 width;
    int32 height;
    int32 bytesPerPixel;
    int32 stride;
} offscreen_buffer;

typedef struct game_button_state
{
    int32 halfTransitionCount;
    bool32 endedDown;
} game_button_state;

typedef struct game_input
{
    union {
        game_button_state buttons[4];
        struct
        {
            game_button_state up;
            game_button_state down;
            game_button_state left;
            game_button_state right;
        };
    };

    real32 dTForFrame;
} game_input;

typedef struct game_memory{
    
    uint64 permanentStorageSize;
    void *permanentStorage;

    bool32 isInitialized;
    
} game_memory;

typedef struct game_state{
    real32 playerX;
    real32 playerY;
    
}game_state;
