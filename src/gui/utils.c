#include "utils.h"

// Source: https://stackoverflow.com/a/431548
BOOL ResizeWindowClient(HWND hWnd, int nWidth, int nHeight) {
	//if(IsIconic(hWnd)) {
	//	SetLastError(ERROR_INVALID_HANDLE);
	//	return FALSE;
	//}
	
	// We check them independently to be able to pass the error along.
	// It should've worked if I used a "||", but I've had nasty surprises in the past with that assumption.
	RECT wndClientRect, wndRect;
	if(!GetClientRect(hWnd, &wndClientRect)) {
		return FALSE;
	}
	if(!GetWindowRect(hWnd, &wndRect)) {
		return FALSE;
	}
	
	return MoveWindow(hWnd,wndRect.left, wndRect.top,
					  nWidth + (wndRect.right - wndRect.left) - wndClientRect.right,
					  nHeight + (wndRect.bottom - wndRect.top) - wndClientRect.bottom,
					  TRUE);
}
