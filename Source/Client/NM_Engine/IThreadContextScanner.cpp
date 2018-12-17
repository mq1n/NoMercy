#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "ThreadFunctions.h"

bool ContextScanRoutine(DWORD dwThreadId)
{
#ifndef _M_X64
	auto bRet				= false;
	auto dwProcessId		= 0UL;
	auto hOwnerProcess		= HANDLE(INVALID_HANDLE_VALUE);
	auto hTargetThread		= HANDLE(INVALID_HANDLE_VALUE);
	auto dwReadAmount		= 0UL;
	auto dwStackPtr			= 0UL;
	auto bReadRet			= FALSE;
	char pStackBuffer[32]   = { 0 };
	auto iFailCount			= 0;
	auto ctx				= CONTEXT();
	auto bDetected			= false;

	if (!dwThreadId)
		goto _complete;

	if (!CThreadFunctions::ThreadIsItAlive(dwThreadId))
		goto _complete;

	dwProcessId = CThreadFunctions::GetThreadOwnerProcessId(dwThreadId);
	if (!dwProcessId)
		goto _complete;

	if (dwProcessId == g_winapiApiTable->GetCurrentProcessId())
		hOwnerProcess = NtCurrentProcess;
	else
		hOwnerProcess = g_winapiApiTable->OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

	if (!hOwnerProcess)
		goto _complete;

	hTargetThread = g_winapiApiTable->OpenThread(THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT, FALSE, dwThreadId);
	if (!hTargetThread)
		goto _complete;

	ctx.ContextFlags = CONTEXT_FULL;
	if (!g_winapiApiTable->GetThreadContext(hTargetThread, &ctx))
		goto _complete;

//	Logf(CUSTOM_LOG_FILENAME, "Stack scan started! Target tid: %u pid: %u", dwThreadId, dwProcessId);

	bDetected = false;	
	for (auto dwCurrEBP = 0UL; dwCurrEBP < 0x1000 && !bDetected; dwCurrEBP += 0x4)
	{
		if (iFailCount > 3)
		{
//			Logf(CUSTOM_LOG_FILENAME, "Read failed more than 3 times, scan will stop. Last offset: 0x%X", dwCurrEBP);
			break;
		}

		bReadRet = g_winapiApiTable->ReadProcessMemory(hOwnerProcess, reinterpret_cast<LPCVOID>(ctx.Ebp + dwCurrEBP), &dwStackPtr, sizeof(dwStackPtr), &dwReadAmount);
//		Logf(CUSTOM_LOG_FILENAME, "Current offset: 0x%X Target: %p Ret: %d(%d)", dwCurrEBP, ctx.Ebp + dwCurrEBP, bReadRet, dwReadAmount == sizeof(dwStackPtr));
		if (!bReadRet || dwReadAmount != sizeof(dwStackPtr))
		{
			iFailCount++;
			continue;
		}
		
		iFailCount = 0;

		bReadRet = g_winapiApiTable->ReadProcessMemory(hOwnerProcess, reinterpret_cast<LPCVOID>(dwStackPtr), &pStackBuffer, sizeof(pStackBuffer), &dwReadAmount);
//		Logf(CUSTOM_LOG_FILENAME, "Stack addr: %p Ret: %d(%d)", dwStackPtr, bReadRet, dwReadAmount == sizeof(pStackBuffer));
		if (bReadRet && dwReadAmount == sizeof(pStackBuffer))
		{
//			Logf(CUSTOM_LOG_FILENAME, "Stack Info -> Address: %p Data: %s Bytes: 0x%x, 0x%x, 0x%x, 0x%x",
//				dwStackPtr, pStackBuffer, pStackBuffer[0], pStackBuffer[1], pStackBuffer[2], pStackBuffer[3]);
		}

		g_winapiApiTable->Sleep(10);
	}

_complete:
	if (hOwnerProcess)
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hOwnerProcess);

	return bRet;
#else
	return true;
#endif
}

bool IScanner::InitializeContextScan(DWORD dwThreadId)
{
	auto bRet = ContextScanRoutine(dwThreadId);
	return bRet;
}

