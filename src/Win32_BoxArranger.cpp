
// TODO: Figure out why it stops working when I grab the window
//  and why it crashes if I hold it grabbed for too long

// clang-format off
#include <stdio.h>
#include <windows.h>
#include <synchapi.h>
#include <profileapi.h>
#include <timeapi.h>
#include <memoryapi.h>
#include <urlmon.h>
#include <windef.h>
#include <errhandlingapi.h>
#include <minwindef.h>
#include <wingdi.h>
#include <winnt.h>
#include <winuser.h>
#include <debugapi.h>
#include <cassert>
// clang-format on

#include "BoxArranger.cpp"

global_variable int32 globalRunning;

typedef struct win32_offscreeen_buffer
{
    BITMAPINFO info;
    void *memory;
    int32 width;
    int32 height;
    int32 bytesPerPixel;

} win32_offscreeen_buffer;
global_variable win32_offscreeen_buffer globalBackBuffer;

internal void Win32ColorWholebuffer(win32_offscreeen_buffer *buffer,
                                    int32 red,
                                    int32 green,
                                    int32 blue)
{
    int32 pixelCount = buffer->width * buffer->height;
    RGBQUAD *pixelByte = (RGBQUAD *)buffer->memory;
    for (int i = 0; i < pixelCount; i++)
    {
        pixelByte->rgbRed = red;
        pixelByte->rgbGreen = green;
        pixelByte->rgbBlue = blue;
        pixelByte++;
    }
}

internal inline void Win32ProcessMessageInfo(bool32 isDown, game_button_state *gameButton)
{
    gameButton->endedDown = isDown;
    gameButton->halfTransitionCount++;
}

internal void Win32DisplayBufferInWindow(HDC hdc, win32_offscreeen_buffer *buffer)
{

    int hello = StretchDIBits(hdc,
                              0,              // [in] int xDest,
                              0,              // [in] int yDest,
                              buffer->width,  // [in] int DestWidth,
                              buffer->height, // [in] int DestHeight,
                              0,              // [in] int xSrc,
                              0,              // [in] int ySrc,
                              buffer->width,  // [in] int SrcWidth,
                              buffer->height, // [in] int SrcHeight,
                              buffer->memory, // [in] const VOID *lpBits,
                              &buffer->info,  // [in] const BITMAPINFO *lpbmi,
                              DIB_RGB_COLORS, // [in] UINT iUsage,
                              SRCCOPY);       // [in] DWORD rop
}

internal void Win32ResizeDIBSection(win32_offscreeen_buffer *buffer, int32 width, int32 height)
{

    if (buffer->memory)
    {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    BITMAPINFOHEADER *bmiHeader = &buffer->info.bmiHeader;
    bmiHeader->biSize = sizeof(*bmiHeader);
    bmiHeader->biWidth = width;
    bmiHeader->biHeight = -height;
    bmiHeader->biPlanes = 1; // must be set to 1
    bmiHeader->biBitCount = 32;
    bmiHeader->biCompression = BI_RGB;
    bmiHeader->biSizeImage = 0;
    bmiHeader->biXPelsPerMeter = 0;
    bmiHeader->biYPelsPerMeter = 0;
    bmiHeader->biClrUsed = 0;
    bmiHeader->biClrImportant = 0;

    buffer->width = width;
    buffer->height = height;
    buffer->bytesPerPixel = 4;

    // TODO switch this to another type of memory allocation

    int32 sizeInBytes = width * height * buffer->bytesPerPixel;
    buffer->memory = VirtualAlloc(0,                        // [ in, optional ] LPVOID lpAddress,
                                  sizeInBytes,              // [in] SIZE_T dwSize,
                                  MEM_RESERVE | MEM_COMMIT, // [in] DWORD flAllocationType,
                                  PAGE_READWRITE);          // [in] DWORD flProtect);
}

LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (message)
    {
    case WM_SIZE: {
        break;
    }

    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(windowHandle, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));

        Win32DisplayBufferInWindow(hdc, &globalBackBuffer);

        EndPaint(windowHandle, &ps);
        break;
    }
    case WM_CLOSE: {
        globalRunning = false;
        break;
    }
    case WM_DESTROY: {
        globalRunning = false;
        break;
    }

    default: {
        result = DefWindowProc(windowHandle, message, wParam, lParam);
        break;
    }
    }

    return result;
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    // Initialize BackBuffer
    Win32ResizeDIBSection(&globalBackBuffer, 1000, 600);

    WNDCLASSA windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = "WindowClassName";

    // TODO: Make this not hello
    int hello = RegisterClass(&windowClass);

    HWND windowHandle = CreateWindowExA(0,
                                        windowClass.lpszClassName,
                                        "RenderThing",
                                        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        0,
                                        0,
                                        hInstance,
                                        0);

    // TODO: Propperly handle this error
    assert(windowHandle);

    // TODO: Propperly handle this error
    assert(timeBeginPeriod(1) == TIMERR_NOERROR);

    LARGE_INTEGER performanceFrequency;
    QueryPerformanceFrequency(&performanceFrequency); // given in ticks per second
    LARGE_INTEGER startingTime;
    LARGE_INTEGER endTime;
    LARGE_INTEGER elapsedMiliseconds;

    game_memory gameMemory = {};
    gameMemory.permanentStorageSize = Megabytes(100);
    gameMemory.permanentStorage =
        VirtualAlloc(0, gameMemory.permanentStorageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    int32 targetFPS = 60;
    int64 frameCount = 0;
    game_input gameInput = {};

    globalRunning = 1;
    while (globalRunning)
    {
        QueryPerformanceCounter(&startingTime);

        gameInput.dTForFrame = 1.0f / targetFPS;

        for (int i = 0; i < ArrayCount(gameInput.buttons); i++)
        {
            gameInput.buttons[i].halfTransitionCount = 0;
        }
        MSG message;
        while (PeekMessage(&message, windowHandle, 0, 0, PM_REMOVE))
        {
            switch (message.message)
            {
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP: {

                // TODO: This is not easy to read
                LPARAM lParam = message.lParam;
                bool32 isDown = ((lParam & (1 << 31)) == 0);
                bool32 wasDown = ((lParam & (1 << 30)) != 0);

                if (isDown != wasDown)
                {
                    OutputDebugString("passed\n");

                    if (message.wParam == 'W')
                    {
                        Win32ProcessMessageInfo(isDown, &gameInput.up);
                    }
                    if (message.wParam == 'A')
                    {
                        Win32ProcessMessageInfo(isDown, &gameInput.left);
                    }
                    if (message.wParam == 'S')
                    {
                        Win32ProcessMessageInfo(isDown, &gameInput.down);
                    }
                    if (message.wParam == 'D')
                    {
                        Win32ProcessMessageInfo(isDown, &gameInput.right);
                    }

                    if (message.wParam == 'Q')
                    {
                        globalRunning = false;
                    }

                }
            }
            default:
                TranslateMessage(&message);
                DispatchMessage(&message); // this internally calls the correct window procedure
            }
        }

        {
            OutputDebugString("=====\n");
            for (int i = 0; i < 4; i++)
            {
                char textBuffer[255];
                sprintf_s(textBuffer,
                          sizeof(textBuffer),
                          "endedDown: %d ; halfTransitionCount: %d\n",
                          gameInput.buttons[i].endedDown,
                          gameInput.buttons[i].halfTransitionCount);
                OutputDebugString(textBuffer);
            }
            OutputDebugString("=====\n");
        }

        offscreen_buffer offscreenBuffer = {};
        offscreenBuffer.width = globalBackBuffer.width;
        offscreenBuffer.height = globalBackBuffer.height;
        offscreenBuffer.bytesPerPixel = globalBackBuffer.bytesPerPixel;
        offscreenBuffer.memory = globalBackBuffer.memory;
        offscreenBuffer.stride = offscreenBuffer.width * offscreenBuffer.bytesPerPixel;
        UpdateAndRender(&offscreenBuffer, &gameInput, frameCount, &gameMemory);
        frameCount++;

        // TODO: maybe get our own DC that we can keep for the whole program
        HDC hdc = GetDC(windowHandle);
        Win32DisplayBufferInWindow(hdc, &globalBackBuffer);
        ReleaseDC(windowHandle, hdc);

        QueryPerformanceCounter(&endTime);
        real32 secondsElapsed = (real32)(endTime.QuadPart - startingTime.QuadPart) /
                                (real32)performanceFrequency.QuadPart;
        real32 secondsPerFrame = (1.0f / (real32)targetFPS);

        if (secondsElapsed < secondsPerFrame)
        {
            DWORD msToSleep = (DWORD)((secondsPerFrame - secondsElapsed) * 1000.0f);
            if (msToSleep > 0)
            {
                Sleep(msToSleep);
            }
        }
        else
        {
            OutputDebugString("Missed Framerate");
        }

        LARGE_INTEGER totalEndTime;
        QueryPerformanceCounter(&totalEndTime);

        // #if 0
        {
            char textBuffer[255];
            real32 msToSleep = (secondsPerFrame - secondsElapsed) * 1000.0f;
            real32 totalFrameMs = ((real32)(totalEndTime.QuadPart - startingTime.QuadPart) /
                                   (real32)performanceFrequency.QuadPart) *
                                  1000.0f;
            sprintf_s(textBuffer,
                      sizeof(textBuffer),
                      "MsElapsed: %f ; msToSleep : %f ; actuallySlept: %d ; totalFrameTime : %f\n",
                      secondsElapsed * 1000.0f,
                      msToSleep,
                      (DWORD)((secondsPerFrame - secondsElapsed) * 1000.0f),
                      totalFrameMs);
            OutputDebugString(textBuffer);
        }
        // #endif
    }
};
