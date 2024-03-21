#include "gui.h"

#include <windows.h>

#include "debug.h"

#include "../core/constants.h"
#include "utils.h"

// Constants
static const wchar_t WINDOW_CLASS_MAIN[] = L"NibblePoker_AtomUtil_Main";

// Globals
ATOM atomWindowClass;
HWND hWndMainWindow;

// Prototypes
LRESULT CALLBACK proc_mainWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int prepareGui(HINSTANCE hInstance, HINSTANCE hPrevInstance) {
    // Registering the window's class
    WNDCLASSW wc = {
            .lpfnWndProc = proc_mainWindow,
            .hInstance = hInstance,
            .lpszClassName = WINDOW_CLASS_MAIN,
    };
    atomWindowClass = RegisterClassW(&wc);
    if(atomWindowClass == ATOM_NULL) {
        fprintf(stderr, "Unable to register the main window's class '%ws' !\n", WINDOW_CLASS_MAIN);
        return 20;
    }

    hWndMainWindow = CreateWindowEx(
            WS_EX_NOPARENTNOTIFY | WS_EX_WINDOWEDGE,
            WINDOW_CLASS_MAIN,
            TEXT("Joe Mama !"),
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_DLGFRAME /* | WS_VISIBLE*/,
            CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
            NULL,
            NULL,
            NULL,
            NULL
    );
    if(hWndMainWindow == NULL) {
        fprintf(stderr, "Unable to create the main window !\n");
        return 21;
    }
    ResizeWindowClient(hWndMainWindow, 400, 300);


    ShowWindow(hWndMainWindow, SW_SHOW);

    return 0;
}

void clearGui(HINSTANCE hInstance, HINSTANCE hPrevInstance) {
    if(hWndMainWindow != NULL) {
        DestroyWindow(hWndMainWindow);
    }
    if(atomWindowClass != ATOM_NULL) {
        UnregisterClassW(WINDOW_CLASS_MAIN, hInstance);
    }
}

int guiMain(HINSTANCE hInstance, HINSTANCE hPrevInstance) {
    int guiReturnCode = 0;
    trace_println("Start of guiMain(...) !");

    guiReturnCode = prepareGui(hInstance, hPrevInstance);
    if(guiReturnCode != 0) {
        error_println("Failed to create application's window !");
        goto END_GUI;
    }

    // Running the message loop.
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    END_GUI_FREE_GUI:
    clearGui(hInstance, hPrevInstance);

    END_GUI:
    return guiReturnCode;
}

LRESULT CALLBACK proc_mainWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Painting the main background
            //ps.rcPaint.bottom -= 42;
            //FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            //ps.rcPaint.top = ps.rcPaint.bottom;
            //ps.rcPaint.bottom += 42;
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW));

            //ps.rcPaint.bottom += 10;

            EndPaint(hwnd, &ps);
            break;
        case WM_SIZE:
            //
            break;
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}
