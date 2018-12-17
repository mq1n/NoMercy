#include "NM_Main.h"
#include "NM_Index.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "WindowEnumerator.h"

#if 0
- Scan own windows
* Window count
* Visible window count
* Window owner module
#endif

#if 0
- Scan windows
- Scan own process

* Title name
* Class name
* Overlay
* Style, StyleEx, Rect
* GetWindowLongPtr(GL_WNDPROC(4)) base + first 12 byte
* enumchildwindows, getwindowinternaltext
#endif


bool IScanner::ScanWindows()
{
	return true;
}

bool IScanner::ScanProcessWindows(HANDLE hProcess)
{
	return true;
}

void IScanner::OnScanWindow(HWND hWnd)
{
}

bool IScanner::IsScannedWindow(HWND hWnd)
{
	return false;
}


