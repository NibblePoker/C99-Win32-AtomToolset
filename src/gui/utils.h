#pragma once

#include "platform.h"

#if defined(NP_WIN_GOODIES_EXPORT_DIALOGS) || defined(NP_WIN_GOODIES_EXPORT_ALL)
	#define DLL_EXP_WIN32_DIALOGS NP_DLL_EXPORT
#else
	#define DLL_EXP_WIN32_DIALOGS
#endif

#include <windows.h>

DLL_EXP_WIN32_DIALOGS BOOL ResizeWindowClient(HWND hWnd, int nWidth, int nHeight);
