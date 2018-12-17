#include "NM_Main.h"
#include "NM_Index.h"
#include "Quarentine.h"
#include "ScannerInterface.h"
#include "Defines.h"

void IScanner::OnWatcherWindowScan(HWND hWnd)
{
	//	DEBUG_LOG(LL_SYS, "Window creation detected! Wnd: %p", hWnd);

#if 0
	if (hWnd)
	{
		auto windowInfos = std::make_shared<SWindowInfos>();
		if (IS_VALID_SMART_PTR(windowInfos))
		{
			auto dwProcessId = 0UL;
			auto dwThreadId = g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwProcessId);
			if (dwThreadId && dwProcessId)
			{
				windowInfos->hWnd = hWnd;
				windowInfos->dwPID = dwProcessId;
				windowInfos->dwTID = dwThreadId;

				char szClass[MAX_PATH] = { 0x0 };
				if (g_winapiApiTable->GetClassNameA(hWnd, szClass, MAX_PATH))
				{
					windowInfos->szClassName = szClass;
				}

				char szTitle[MAX_PATH] = { 0x0 };
				if (g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH))
				{
					windowInfos->szTitle = szTitle;
				}

				OnWindowCheck(windowInfos, true); // redirect scanner interface
			}
		}
	}
#endif
}
