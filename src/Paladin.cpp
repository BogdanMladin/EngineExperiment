/**
@file Paladin.cpp
@brief brief for the file.

Longer for the file.
*/


#include "Paladin.h"

#define ArrayCount(array) (sizeof(array) / sizeof(array[1]))

/**
this is an amazing documentation.
for this amazing function. Look at it, it's so simple and elegeant.

*/
int32 StrLen(const char *s)
{
    int32 result = 0;
    while (*s)
    {
        result++;
        s++;
    }
    return result;
}

inline internal int32 RoundReal32ToInt32(real32 Real32)
{
    // int32 result = (int32)lrintf(Real32); //this uses intrinsic
    int32 Result = 0;
    Result = (int32)(Real32 + 0.5f);
    return Result;
}

internal void ColorWholeBuffer(offscreen_buffer *buffer, int32 red, int32 green, int32 blue)
{
    int32 pixelCount = buffer->width * buffer->height;
    uint8 *pixelByte = (uint8 *)buffer->memory;
    for (int i = 0; i < pixelCount; i++)
    {
        *pixelByte++ = blue;
        *pixelByte++ = green;
        *pixelByte++ = red;
        pixelByte++;
    }
}
internal void DrawRectangle(offscreen_buffer *buffer,
                            real32 realMinX,
                            real32 realMinY,
                            real32 realMaxX,
                            real32 realMaxY,
                            real32 R,
                            real32 G,
                            real32 B)
{
    int32 minX = RoundReal32ToInt32(realMinX);
    int32 minY = RoundReal32ToInt32(realMinY);
    int32 maxX = RoundReal32ToInt32(realMaxX);
    int32 maxY = RoundReal32ToInt32(realMaxY);

    if (minX < 0)
        minX = 0;

    if (minY < 0)
        minY = 0;

    if (maxX > buffer->width)
        maxX = buffer->width;

    if (maxY > buffer->height)
        maxY = buffer->height;

    uint32 color = (RoundReal32ToInt32(B * 255.0f) + (RoundReal32ToInt32(G * 255.0f) << 8) +
                    (RoundReal32ToInt32(R * 255.0f) << 16));

    uint8 *row = ((uint8 *)buffer->memory) + minX * buffer->bytesPerPixel + minY * buffer->stride;
    for (int y = minY; y < maxY; y++)
    {
        uint32 *pixel = (uint32 *)row;
        for (int x = minX; x < maxX; x++)
        {
            *pixel++ = color;
        }
        row += buffer->stride;
    }
}

internal void RenderWeirdGradient(offscreen_buffer *buffer, int64 count, bool32 enable)
{
    int32 localCount = 0;
    for (int row = 0; row < buffer->height; row++)
    {
        for (int pxCount = 0; pxCount < buffer->width; pxCount++)
        {
            int32 *pixel = (int32 *)buffer->memory;
            pixel += row * buffer->width + pxCount;
            uint8 *pixelByte = (uint8 *)pixel;
            *pixelByte++ = ((row + count) * enable) % 256;
            *pixelByte++ = ((pxCount + count) * enable) % 256;
            *pixelByte++ = (row + pxCount - count) * enable % 256;
        }
    }
}

internal void UpdateAndRender(offscreen_buffer *buffer,
                              game_input *input,
                              int64 frameCount,
                              game_memory *gameMemory)
{
    assert(gameMemory->permanentStorageSize >= sizeof(game_state));
    game_state *state = (game_state *)gameMemory->permanentStorage;
    if (!gameMemory->isInitialized)
    {
        state->playerX = 10;
        state->playerY = 10;

        gameMemory->isInitialized = true;
    }

    real32 playerToMove = input->dTForFrame * 60;
    if (input->up.endedDown)
    {
        state->playerY -= playerToMove;
    }
    if (input->down.endedDown)
    {
        state->playerY += playerToMove;
    }
    if (input->left.endedDown)
    {
        state->playerX -= playerToMove;
    }
    if (input->right.endedDown)
    {
        state->playerX += playerToMove;
    }

    RenderWeirdGradient(buffer, frameCount, (true));
    DrawRectangle(buffer,
                  state->playerX,
                  state->playerY,
                  state->playerX + 20,
                  state->playerY + 20,
                  1,
                  1,
                  1);
}
