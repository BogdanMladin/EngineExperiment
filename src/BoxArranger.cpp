#include "BoxArranger.h"

internal int32 StrLen(const char *s)
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
internal void ColorPartBuffer(
    offscreen_buffer *buffer, int32 red, int32 green, int32 blue, int64 count)
{
    int32 localCount = 0;
    for (int row = 0; row < buffer->height; row++)
    {
        for (int pxCount = 0; pxCount < buffer->width; pxCount++)
        {
            int32 *pixel = (int32 *)buffer->memory;
            pixel += row * buffer->width + pxCount;
            uint8 *pixelByte = (uint8 *)pixel;
            *pixelByte++ = (row+ count) % 256;
            *pixelByte++ = (pxCount+ count) % 256;
            *pixelByte++ = count % 256;
        }
    }
}

internal void UpdateAndRender(offscreen_buffer *buffer, int64 frameCount)
{
    int32 red = 255;
    int32 green = 0;
    int32 blue = 0;
    ColorPartBuffer(buffer, red, green, blue, frameCount);
}
