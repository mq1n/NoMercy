#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Quarentine.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "WindowEnumerator.h"
#include "ProcessFunctions.h"


void IScanner::IsFileMappingExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto hMapFile = OpenFileMappingA(SYNCHRONIZE, FALSE, szTargetName.c_str());
	if (IS_VALID_HANDLE(hMapFile))
	{
		// report back to server
		CloseHandle(hMapFile);
	}
}

void IScanner::IsMutexExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto hMutex = OpenMutexA(SYNCHRONIZE, FALSE, szTargetName.c_str());
	if (IS_VALID_HANDLE(hMutex))
	{
		// report back to server
		CloseHandle(hMutex);
	}
}

void IScanner::IsEventExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto hEvent = OpenEventA(SYNCHRONIZE, FALSE, szTargetName.c_str());
	if (IS_VALID_HANDLE(hEvent))
	{
		// report back to server
		CloseHandle(hEvent);
	}
}

void IScanner::IsSemaphoreExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto hSemaphore = OpenSemaphoreA(SYNCHRONIZE, FALSE, szTargetName.c_str());
	if (IS_VALID_HANDLE(hSemaphore))
	{
		// report back to server
		CloseHandle(hSemaphore);
	}
}

void IScanner::IsJobObjectExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto hJobObject = OpenJobObjectA(SYNCHRONIZE, FALSE, szTargetName.c_str());
	if (IS_VALID_HANDLE(hJobObject))
	{
		// report back to server
		CloseHandle(hJobObject);
	}
}

void IScanner::IsSymLinkExist(const std::string & szReqId, const std::string & szTargetName) //  \\.\BlackBone  "\\\\.\\Htsysm72FB"(capcom)
{
	std::string szDeviceName = "\\\\.\\" + szTargetName;
	auto hDevice = g_winapiApiTable->CreateFileA(szDeviceName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (IS_VALID_HANDLE(hDevice))
	{
		// report back to server
		CloseHandle(hDevice);
	}
}

void IScanner::IsModuleExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto wszName = std::wstring(szTargetName.begin(), szTargetName.end());
	if (g_nmApp->DynamicWinapiInstance()->GetModuleAddressFromName(wszName.c_str(), false))
	{
		// report back to server
	}
}

void IScanner::IsWindowNameExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator)) 
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return;
	}

	auto hWnd = windowEnumerator->FindWindowWithTitleName(szTargetName, true);
	if (IsWindow(hWnd))
	{
		// report back to server
	}
}

void IScanner::IsClassNameExist(const std::string & szReqId, const std::string & szTargetName)
{
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator))
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return;
	}

	auto hWnd = windowEnumerator->FindWindowWithClassName(szTargetName, true);
	if (IsWindow(hWnd))
	{
		// report back to server
	}
}

void IScanner::IsFileExist(const std::string & szReqId, const std::string & szTargetName)
{
	if (g_nmApp->DirFunctionsInstance()->IsFileExist(szTargetName))
	{
		// report back to server
	}
}

void IScanner::IsMemChecksumCorrupted(const std::string & szReqId, LPVOID lpAddress, DWORD cbSize, const std::string & szCorrectHash)
{
	auto dwReadByteCount = 0UL;
	auto pBuffer = malloc(cbSize);
	if (NT_SUCCESS(g_winapiApiTable->NtReadVirtualMemory(NtCurrentProcess, lpAddress, &pBuffer, cbSize, &dwReadByteCount)) && cbSize == dwReadByteCount)
	{
		auto szCurrentHash = g_nmApp->DirFunctionsInstance()->GetSHA256(pBuffer, cbSize);
		if (szCurrentHash.empty() == false && szCurrentHash != szCorrectHash)
		{
			// report back to server
		}
	}
	free(pBuffer);
}

void IScanner::IsProcessRunning(const std::string & szReqId, const std::string & szTargetName)
{
	if (CProcessFunctions::GetProcessCountFromProcessName(szTargetName.c_str()) > 0)
	{
		// report back to server
	}
}

void IScanner::IsMemCorrupted(const std::string & szReqId, LPVOID lpAddress, DWORD dwOffsetList[], std::size_t cbOffsetListSize, const std::string & szCorrectChecksum)
{
	// https://github.com/mq1n/AOE2-HD-Trainer/blob/master/AOE2_Trainer/main.cpp#L20
}

void IScanner::IsEbpContextCorrupted(const std::string & szReqId, DWORD dwOffset, const std::string & szCorrectChecksum)
{
	/*
	- Iterate all threads
	- Scan for curr ebp + given offset == szCorrectChecksum otherwise throw violation
*/
}

void IScanner::CheckForegroundWindowOwner(const std::string & szReqId)
{
//	*check special window process name, owner module name from window owner thread id (GetWindowModuleFileName)
	/*
	
GetDesktopWindow
GetForegroundWindow
GetShellWindow
GetTopWindow
GetActiveWindow
g_winapiApiTable->FindWindowExA(0, 0, xorstr("Progman"), xorstr("Program Manager"));
	*/
}

void IScanner::CheckAPIModuleBound(const std::string & szReqId, const std::string & szModuleName, const std::string & szAPIName)
{
	/*
	is apiName in moduleName area
	*/
}

void IScanner::CheckTimeManipulation(const std::string & szReqId)
{
	/*
	GetTickCount
	QueryPerformanceCounter
	timeGetTime
	GetSystemTime
	*/
}

void IScanner::CheckGameAlteration(const std::string & szReqId)
{
	// title, file hash
}

void IScanner::CheckSystemModuleHashs(const std::string & szReqId)
{
	// Send g_winapiModuleTable bind'd module name, hashs and os informations(winver,spver,buildver) to master server
}
