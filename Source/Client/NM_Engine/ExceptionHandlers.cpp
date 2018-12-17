#include "NM_Index.h"
#include "NM_Main.h"
#include "ExceptionHandlers.h"
#include "DynamicWinapi.h"
#include "Defines.h"

LONG WINAPI vehFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo && ExceptionInfo->ContextRecord)
	{
		if (ExceptionInfo->ContextRecord->Dr0 != 0 || ExceptionInfo->ContextRecord->Dr1 != 0 || ExceptionInfo->ContextRecord->Dr2 != 0 ||
			ExceptionInfo->ContextRecord->Dr3 != 0 || ExceptionInfo->ContextRecord->Dr7 != 0)
		{
			DEBUG_LOG(LL_ERR, "VehFilter: Debug registers cleaned!");

			ExceptionInfo->ContextRecord->Dr0 = 0;
			ExceptionInfo->ContextRecord->Dr1 = 0;
			ExceptionInfo->ContextRecord->Dr2 = 0;
			ExceptionInfo->ContextRecord->Dr3 = 0;
			ExceptionInfo->ContextRecord->Dr7 = 0;
		}
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord)
	{
		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
		{
			DEBUG_LOG(LL_CRI, "VehFilter: EXCEPTION_SINGLE_STEP detected!!");
			g_nmApp->OnCloseRequest(EXIT_ERR_VEH_SINGLE_STEP_EXCEPTION, g_winapiApiTable->GetLastError());
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
		{
			DEBUG_LOG(LL_CRI, "VehFilter: EXCEPTION_BREAKPOINT detected!!");
			g_nmApp->OnCloseRequest(EXIT_ERR_VEH_BREAKPOINT_EXCEPTION, g_winapiApiTable->GetLastError());
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
		{
			DEBUG_LOG(LL_CRI, "VehFilter: EXCEPTION_GUARD_PAGE detected!!");
			g_nmApp->OnCloseRequest(EXIT_ERR_VEH_PAGE_GUARD_EXCEPTION, g_winapiApiTable->GetLastError());
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI sehFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo && ExceptionInfo->ContextRecord)
	{
		if (ExceptionInfo->ContextRecord->Dr0 != 0 || ExceptionInfo->ContextRecord->Dr1 != 0 || ExceptionInfo->ContextRecord->Dr2 != 0 ||
			ExceptionInfo->ContextRecord->Dr3 != 0 || ExceptionInfo->ContextRecord->Dr7 != 0)
		{
			DEBUG_LOG(LL_ERR, "SehFilter: Debug registers cleaned!");

			ExceptionInfo->ContextRecord->Dr0 = 0;
			ExceptionInfo->ContextRecord->Dr1 = 0;
			ExceptionInfo->ContextRecord->Dr2 = 0;
			ExceptionInfo->ContextRecord->Dr3 = 0;
			ExceptionInfo->ContextRecord->Dr7 = 0;
		}
	}

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord)
	{
		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION)
		{
#ifndef _M_X64
			ExceptionInfo->ContextRecord->Eip++;
#else
			ExceptionInfo->ContextRecord->Rip++;
#endif
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
		{
			DEBUG_LOG(LL_CRI, "SehFilter: EXCEPTION_SINGLE_STEP detected!!");
			g_nmApp->OnCloseRequest(EXIT_ERR_SEH_SINGLE_STEP_EXCEPTION, g_winapiApiTable->GetLastError());
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
		{
			DEBUG_LOG(LL_CRI, "SehFilter: EXCEPTION_BREAKPOINT detected!!");
			g_nmApp->OnCloseRequest(EXIT_ERR_SEH_BREAKPOINT_EXCEPTION, g_winapiApiTable->GetLastError());
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
		{
			DEBUG_LOG(LL_CRI, "SehFilter: EXCEPTION_GUARD_PAGE detected!!");
			g_nmApp->OnCloseRequest(EXIT_ERR_SEH_PAGE_GUARD_EXCEPTION, g_winapiApiTable->GetLastError());
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

__forceinline bool InitializeSEH()
{
	return (g_winapiApiTable->SetUnhandledExceptionFilter(sehFilter) != NULL);
}

__forceinline bool InitializeVEH()
{
	return (g_winapiApiTable->AddVectoredExceptionHandler(0, vehFilter) != NULL);
}

bool CExceptionHandlers::InitExceptionHandlers()
{
#ifdef _DEBUG
	DEBUG_LOG(LL_SYS, "Exception handlers initilization skipped on debug build!");
	return true;
#endif

	if (!InitializeSEH() || !InitializeVEH()) 
	{
		DEBUG_LOG(LL_ERR, "Exception handlers can not created!");
		return false;
	}
	return true;
}
