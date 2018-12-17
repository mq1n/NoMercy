#include "NM_Index.h"
#include "NM_Main.h"
#include "MiniDump.h"
#include "Defines.h"
#include "ExceptionHandlers.h"
#include <DbgHelp.h>
#include <lazy_importer.hpp>

static std::string s_szCurrentPath = "";

// Custom minidump callback 
BOOL CALLBACK MyMiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
	BOOL bRet = FALSE;

	// Check parameters 
	if (!pInput)
		return FALSE;

	if (!pOutput)
		return FALSE;

	// Process the callbacks 
	switch (pInput->CallbackType)
	{
	case IncludeModuleCallback:
	{
		// Include the module into the dump 
		bRet = TRUE;
	}
	break;

	case IncludeThreadCallback:
	{
		// Include the thread into the dump 
		bRet = TRUE;
	}
	break;

	case ModuleCallback:
	{
		// Does the module have ModuleReferencedByMemory flag set ? 
		if (!(pOutput->ModuleWriteFlags & ModuleReferencedByMemory)) {
			// No, it does not - exclude it 
			// wprintf(L"Excluding module: %s \n", pInput->Module.FullPath);
			pOutput->ModuleWriteFlags &= (~ModuleWriteModule);
		}
		bRet = TRUE;
	}
	break;

	case ThreadCallback:
	{
		// Include all thread information into the minidump 
		bRet = TRUE;
	}
	break;

	case ThreadExCallback:
	{
		// Include this information 
		bRet = TRUE;
	}
	break;

	case MemoryCallback:
	{
		// We do not include any information here -> return FALSE 
		bRet = FALSE;
	}
	break;

	case CancelCallback:
		break;
	}

	return bRet;
}

bool CreateMiniDump(EXCEPTION_POINTERS* pExceptionInfo)
{
	DEBUG_LOG(LL_SYS, "Exception handled: %p", pExceptionInfo);

	time_t t;
	time(&t);
	struct tm *tinfo;
	tinfo = localtime(&t);

	char szDumpName[256];
	strftime(szDumpName, sizeof(szDumpName), xorstr("Dump%Y%m%d_%H%M%S.dmp").crypt_get(), tinfo);

	char szDumpPath[256];
	sprintf(szDumpPath, xorstr("%s/NM_%d_%s").crypt_get(), s_szCurrentPath.c_str(), g_nmApp->DataInstance()->GetAppType(), szDumpName);

	// file format MiniDump[YYYYMMDD][HH_MM_SEC]
	auto hFile = CreateFileA(szDumpPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (!IS_VALID_HANDLE(hFile))
	{
		DEBUG_LOG(LL_ERR, "Exception dump file is not created. Error code: %u Path: %s", GetLastError(), szDumpPath);
		return false;
	}

	// Create the minidump 
	MINIDUMP_EXCEPTION_INFORMATION mdei;
	mdei.ThreadId = GetCurrentThreadId();
	mdei.ExceptionPointers = pExceptionInfo;
	mdei.ClientPointers = FALSE;

	MINIDUMP_CALLBACK_INFORMATION mci;
	mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback;
	mci.CallbackParam = 0;

	MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);

	auto rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pExceptionInfo != 0) ? &mdei : 0, 0, &mci);
	if (!rv)
	{
		DEBUG_LOG(LL_ERR, "Exception dump is not created.Error code : %u", GetLastError());
	}
	else 
	{
		DEBUG_LOG(LL_SYS, "Exception dump successfully created.");
	}

	// Close the file 
	CloseHandle(hFile);
	return true;
}

LONG WINAPI ExceptionFilterSeh(EXCEPTION_POINTERS * pExceptionInfo)
{
	// Custom handler
//	CExceptionHandlers::OnExceptionThrowed(pExceptionInfo);

	// Minidump handler
	if (pExceptionInfo && pExceptionInfo->ExceptionRecord)
	{
		if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
		{
			auto hThread = g_winapiApiTable->CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(CreateMiniDump), pExceptionInfo, 0, NULL);
			if (IS_VALID_HANDLE(hThread)) 
			{
				g_winapiApiTable->WaitForSingleObject(hThread, INFINITE);
				g_winapiApiTable->CloseHandle(hThread);
			}
		}
		else 
		{
			CreateMiniDump(pExceptionInfo);
		}

		DEBUG_LOG(LL_ERR, "Seh exception triggered. Code: %p", pExceptionInfo->ExceptionRecord->ExceptionCode);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

bool CMiniDump::InitMiniDump()
{
	s_szCurrentPath = g_nmApp->DirFunctionsInstance()->ExePath();

	if (g_winapiApiTable->SetUnhandledExceptionFilter(ExceptionFilterSeh)) 
	{
		DEBUG_LOG(LL_SYS, "Mini dump generator Exception handler is succesfully created!");
		return true;
	}
	DEBUG_LOG(LL_ERR, "Mini dump generator Exception handler is NOT created!");
	return false;
}

