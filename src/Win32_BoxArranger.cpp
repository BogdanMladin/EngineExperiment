
// clang-format off
#include <windows.h>
#include <minwindef.h>
#include <winuser.h>
#include <debugapi.h>
#include <cassert>
// clang-format on
 
#include "BoxArranger.cpp"

// open a window

LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (message){
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

    RegisterClass(&windowClass);

    HWND windowHandle = CreateWindowExA(0,
                                        windowClass.lpszClassName,
                                        "RenderThing",
                                        WS_VISIBLE || WS_OVERLAPPED,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        0,
                                        0,
                                        hInstance,
                                        0);

    MSG message;
    BOOL getMessageReturn;

    while ((getMessageReturn = GetMessage(&message, windowHandle, 0, 0)) != 0)
    {
        if (getMessageReturn == -1)
        {
            assert(false);
        }
        else
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
};
