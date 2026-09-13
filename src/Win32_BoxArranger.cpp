
// clang-format off
#include <urlmon.h>
#include <windef.h>
#include <windows.h>
#include <errhandlingapi.h>
#include <minwindef.h>
#include <wingdi.h>
#include <winuser.h>
#include <debugapi.h>
#include <cassert>
// clang-format on

#include "BoxArranger.cpp"

// open a window

LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (message)
    {
    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(windowHandle, &ps);
        

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        BITMAPINFOHEADER bmiheader = {};
        bmiheader.biSize = sizeof(BITMAPINFOHEADER);
        bmiheader.biWidth = 10;
        bmiheader.biHeight = -10;
        bmiheader.biPlanes = 1; // must be set to 1
        bmiheader.biBitCount = 32;
        bmiheader.biCompression = BI_BITFIELDS;
        bmiheader.biSizeImage = 0;
        bmiheader.biXPelsPerMeter = 0;
        bmiheader.biYPelsPerMeter = 0;
        bmiheader.biClrUsed = 0;
        bmiheader.biClrImportant = 0;

        BYTE byteOfOnes = 0;
        byteOfOnes = ~byteOfOnes;

        RGBQUAD bmiColors[3] = {};
        bmiColors[0].rgbRed = byteOfOnes;
        bmiColors[1].rgbBlue = byteOfOnes;
        bmiColors[2].rgbGreen = byteOfOnes;

        BITMAPINFO bitmapinfo = {};
        bitmapinfo.bmiHeader = bmiheader;
        bitmapinfo.bmiColors[0].rgbRed = byteOfOnes;
        bitmapinfo.bmiColors[1].rgbGreen = byteOfOnes;
        bitmapinfo.bmiColors[2].rgbBlue = byteOfOnes;

        void *bits = 0;

        CreateDIBSection(hdc, &bitmapinfo, DIB_RGB_COLORS, &bits, 0, 0);
        RGBQUAD *morebits = (RGBQUAD *)bits;
        int stride = 10;
        morebits[0].rgbBlue = byteOfOnes;
        morebits[1].rgbRed = byteOfOnes;
        morebits[2].rgbGreen = byteOfOnes;

        int hello = StretchDIBits(hdc,
                                  0,              // [in] int xDest,
                                  0,              // [in] int yDest,
                                  10,             // [in] int DestWidth,
                                  10,             // [in] int DestHeight,
                                  0,              // [in] int xSrc,
                                  0,              // [in] int ySrc,
                                  10,             // [in] int SrcWidth,
                                  10,             // [in] int SrcHeight,
                                  morebits,       // [in] const VOID *lpBits,
                                  &bitmapinfo,    // [in] const BITMAPINFO *lpbmi,
                                  DIB_RGB_COLORS, // [in] UINT iUsage,
                                  SRCCOPY);       // [in] DWORD rop

        EndPaint(windowHandle, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(windowHandle);
        PostQuitMessage(0);

    default:
        result = DefWindowProc(windowHandle, message, wParam, lParam);
    }

    return result;
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    WNDCLASSA windowClass = {};

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = "WindowClassName";

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
    if (windowHandle == 0)
    {
        assert(false);
    }

    MSG message;
    BOOL getMessageReturn;

    int running = 1;
    while (running)
    {

        getMessageReturn = GetMessage(&message, NULL, 0, 0);
        if (getMessageReturn == 0)
        {
            running = false;
        }

        if (getMessageReturn == -1)
        {
            int i = GetLastError();
            return i;
        }
        else
        {
            TranslateMessage(&message);
            DispatchMessage(&message); // this internally calls the correct window procedure
        }
    }
};
