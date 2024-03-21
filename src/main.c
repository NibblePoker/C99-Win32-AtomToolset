#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

// Allows the use of Common Controls version 6
// Source: https://stackoverflow.com/a/33125299
#include <sdkddkver.h>
#if(NTDDI_VERSION >= NTDDI_WINXP)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <locale.h>

#include "debug.h"

#include "./cli/cli.h"
#include "./gui/gui.h"
#include "text.h"


bool isRanFromConsole() {
    DWORD procIDs[2];
    return GetConsoleProcessList((LPDWORD) procIDs, 2) != 1;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
    // Attempting to determine the context the program is executed in.
    DWORD procIDs[2];
    BOOL isRanFromConsole = GetConsoleProcessList((LPDWORD) procIDs, 2) != 1;

    // We hide the console for the desktop experience.
    //if(isRanFromConsole) {
    //    ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
    //    ShowWindow(GetConsoleWindow(), SW_HIDE);
    //}

    // Attempting to attach to the parent console if possible.
    // Barely works when using the "WINDOWS" subsystem, it also returns a ACCESS_DENIED error with AllocConsole !
    // Sources:
    // * https://stackoverflow.com/a/57241985
    // * https://stackoverflow.com/a/3448740
    /*if(AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE *fDummy;
        if(freopen_s(&fDummy, "CONIN$", "r", stdin)) {
            return 1;
        }
        if(freopen_s(&fDummy, "CONOUT$", "w", stderr)) {
            return 2;
        }
        if(freopen_s(&fDummy, "CONOUT$", "w", stdout)) {
            return 3;
        }
        trace_println("Reattached to standard console streams !");
        hasConsole = TRUE;
    }*/

    // Dumping some startup values.
    // We're using '__argc' and '__wargv' instead of 'lpCmdLine' to avoids using weird LPWSTR to wchar_t converters.
    trace_println("Start of wWinMain(...) !");

    // Allows emojis in given filenames names and arguments.
    // It also allows the original code page to be restored when exiting.
    trace_println("Setting console's code page %d...", CP_UTF8);

    setlocale(LC_ALL, "en_US.UTF-8");

    UINT originalCodePage = GetConsoleCP();
    UINT originalOutputCodePage = GetConsoleOutputCP();

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    /*trace_println("Dumping launch arguments & variables...");
    //trace_println("> '%p'", hInstance);
    //trace_println("> '%p'", hPrevInstance);
    trace_println("> lpCmdLine: '%ls'", lpCmdLine);
    trace_println("> nShowCmd: '%d'", nShowCmd);
    trace_println("> __argc: '%d'", __argc);
    trace_println("> __wargv:");
    for(int i = 0; i < __argc; i++) {
        trace_println("> [%d]: '%ls'", i, __wargv[i]);
    }*/

    trace_println("Checking for the 'gui' verb manually...");
    for(int i = 0; i < __argc; i++) {
        if(text_areStringsEqualW(L"gui", __wargv[i])) {
            trace_println("> Found it, manually forcing the usage of the GUI !");
            isRanFromConsole = FALSE;
            break;
        }
    }

    trace_println("Switching over to context-specific main functions.");
    int returnCode = isRanFromConsole ? cliMain(__argc, __wargv) : guiMain(hInstance, hPrevInstance);
    //returnCode++; // Force trigger an error !
    trace_println("Returned to wWinMain !");

    END_CLEAN_CONSOLE:
    trace_println("Setting console's code page back to %d & %d...", originalCodePage, originalOutputCodePage);
    SetConsoleCP(originalCodePage);
    SetConsoleOutputCP(originalOutputCodePage);

    // Fixing the cursor's position when in a non-scrolling context. (ex: You did 'cls', and it hasn't scrolled yet)
    // TODO: THIS !, and after the error check too !

    if(returnCode != 0) {
        if(isRanFromConsole) {
            // FIXME: Add a flag to enable this behaviour, should be off by default !
            error_println("The game exited with a non-zero exit code !");
            error_println("Feel free to analyse the logs and press enter to quit !");
            getchar();
        } else {
            MessageBox(NULL,
                       L"The application exited with an error code !\nPlease check the console logs for more information.",
                       L"Error",
                       MB_ICONERROR | MB_OK);
        }
    }

    return returnCode;
}
