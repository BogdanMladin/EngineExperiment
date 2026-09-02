
// clang-format off
#include <windows.h>
#include <errhandlingapi.h>
#include <minwindef.h>
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

        EndPaint(windowHandle, &ps);
        break;
    }

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

    while ((getMessageReturn = GetMessage(&message, NULL, 0, 0)) != 0)
    {
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
