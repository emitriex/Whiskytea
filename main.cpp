#include <Windows.h>
#include <stdio.h>
#include <assert.h>

// Global Variables
HWND g_GlobalWindowHandle;

// Convenient utility function to convert Windows error codes to string
const char* GetLastErrorAsString()
{
    // Get the error message, if any.
    const DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return ""; // No error message has been recorded

    thread_local char buffer[1024]{};
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, sizeof(buffer), NULL);

    return buffer;
}

// Windows Message Callback
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // Window is terminating. Part 1 of shutting down.
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

        // Window is being destroyed after the being removed from the screen. Part 2 of shutting down.
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

        // Message not handled
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void InitWindow()
{
    static const char* s_AppName = "WhiskyTilapia"; // control panel string

    ::HINSTANCE hInstance = 0;//GetModuleHandle(NULL);

    ::WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (::HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = s_AppName;

    // Register the window class. 
    if (FAILED(RegisterClass(&wc)))
    {
        const char* err = GetLastErrorAsString();
        printf("RegisterClass Error: %s\n", err);

        // Assert for windows if register class fails
        assert(false); //trigger assert
    }

    const int scrnWidth = 1280;
    const int scrnHeight = 720;
    const int posX = (GetSystemMetrics(SM_CXSCREEN) - scrnWidth) / 2;
    const int posY = (GetSystemMetrics(SM_CYSCREEN) - scrnHeight) / 2;

    const ::DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
    ::RECT rect{ 0, 0, scrnWidth, scrnHeight };
    ::AdjustWindowRect(&rect, style, false);

    // Create the main window
    g_GlobalWindowHandle = CreateWindow(s_AppName,
                                        "Whisky",               // title-bar string 
                                        style,                  // top-level window 
                                        posX, posY,             // default positions
                                        scrnWidth, scrnHeight,  // default width/height 
                                        0, 0, hInstance, 0);

    if (!g_GlobalWindowHandle)
    {
        const char* err = GetLastErrorAsString();
        printf("CreateWindow error: %s\n", err);

        assert(false);
    }
}

int main()
{
    InitWindow();

    // Windows Message Pump
    MSG msg;
    BOOL fGotMessage;
    while ((fGotMessage = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0 && fGotMessage != -1)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Do application update here...
    }

    return 0;
}
