#include "NM_Index.h"
#include "NM_Main.h"
#include "ExceptionHandlers.h"
#include "DynamicWinapi.h"
#include "Defines.h"
#include <ImageHlp.h>

#include <lazy_importer.hpp>

inline bool GetSymbolName(DWORD64 ullAddress, char * szName, PDWORD64 pdw64Displacement)
{
	BYTE pBuffer[sizeof(IMAGEHLP_SYMBOL64) + 128] = { 0x0 };

	auto pSymbol64				= reinterpret_cast<IMAGEHLP_SYMBOL64*>(pBuffer);
	pSymbol64->SizeOfStruct		= sizeof(IMAGEHLP_SYMBOL64);
	pSymbol64->MaxNameLength	= 128;

	auto dwDisplacement64 = 0ULL;
	if (LI_FIND(SymGetSymFromAddr64)(NtCurrentProcess, ullAddress, &dwDisplacement64, pSymbol64))
	{
		strcpy(szName, pSymbol64->Name);
		if (pdw64Displacement) *pdw64Displacement = dwDisplacement64;
		return true;
	}

	DEBUG_LOG(LL_CRI, "SymGetSymFromAddr64 fail! Error: %u", LI_FIND(GetLastError)());
	return false;
}

DWORD CExceptionHandlers::OnExceptionThrowed(EXCEPTION_POINTERS * pExceptionInfo)
{
	DEBUG_LOG(LL_CRI, "An exception handled! Exception data: %p Current Thread: %u Current process: %u Last error: %u", 
		pExceptionInfo, LI_FIND(GetCurrentThreadId)(), LI_FIND(GetCurrentProcessId)(), LI_FIND(GetLastError)());

	if (!LI_FIND(SymSetOptions)(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS))
	{
		DEBUG_LOG(LL_CRI, "SymSetOptions fail! Error: %u",  LI_FIND(GetLastError)());
//		return EXCEPTION_EXECUTE_HANDLER;
	}

	if (!LI_FIND(SymInitialize)(NtCurrentProcess, NULL, true))
	{
		DEBUG_LOG(LL_CRI, "SymInitialize fail! Error: %u",  LI_FIND(GetLastError)());
//		return EXCEPTION_EXECUTE_HANDLER;
	}

	if (pExceptionInfo && pExceptionInfo->ExceptionRecord)
	{
		char szExceptionAddrSymbol[MAX_PATH] = { 0 };
		GetSymbolName((DWORD64)pExceptionInfo->ExceptionRecord->ExceptionAddress, szExceptionAddrSymbol, nullptr);

		DEBUG_LOG(LL_CRI, "Exception Address: 0x%08x Symbol: %s",	pExceptionInfo->ExceptionRecord->ExceptionAddress, szExceptionAddrSymbol);
		DEBUG_LOG(LL_CRI, "Exception Code: 0x%08x",					pExceptionInfo->ExceptionRecord->ExceptionCode);
		DEBUG_LOG(LL_CRI, "Exception Flags: 0x%08x",				pExceptionInfo->ExceptionRecord->ExceptionFlags);
	}

	if (pExceptionInfo && pExceptionInfo->ContextRecord)
	{
		auto pContext = *pExceptionInfo->ContextRecord;

#ifndef _M_X64
		DEBUG_LOG(LL_CRI, "Eax: 0x%08x \t Ebx: 0x%08x", pContext.Eax, pContext.Ebx);
		DEBUG_LOG(LL_CRI, "Ecx: 0x%08x \t Edx: 0x%08x", pContext.Ecx, pContext.Edx);
		DEBUG_LOG(LL_CRI, "Esi: 0x%08x \t Edi: 0x%08x", pContext.Esi, pContext.Edi);
		DEBUG_LOG(LL_CRI, "Ebp: 0x%08x \t Esp: 0x%08x", pContext.Ebp, pContext.Esp);
#else
		DEBUG_LOG(LL_CRI, "Rax: 0x%08x \t Rbx: 0x%08x", pContext.Rax, pContext.Rbx);
		DEBUG_LOG(LL_CRI, "Rcx: 0x%08x \t Rdx: 0x%08x", pContext.Rcx, pContext.Rdx);
		DEBUG_LOG(LL_CRI, "Rsi: 0x%08x \t Rdi: 0x%08x", pContext.Rsi, pContext.Rdi);
		DEBUG_LOG(LL_CRI, "Rbp: 0x%08x \t Rsp: 0x%08x", pContext.Rbp, pContext.Rsp);
#endif
	}

	// Check stack

	LPVOID lpFrames[MAX_FRAME_COUNT] = { 0x0 };
	for (auto & lpFrame : lpFrames)
		lpFrame = nullptr;

	auto wCapturedFrames = g_winapiApiTable->RtlCaptureStackBackTrace(1, MAX_FRAME_COUNT, lpFrames, nullptr);
	if (!wCapturedFrames)
	{
		DEBUG_LOG(LL_CRI, "Any frame can NOT captured! Error: %u",  LI_FIND(GetLastError)());
		return EXCEPTION_EXECUTE_HANDLER;
	}
	DEBUG_LOG(LL_CRI, "%u Frame captured!", wCapturedFrames);

	for (auto i = 0; i < wCapturedFrames; i++)
	{
		auto ullCurrFrame = reinterpret_cast<DWORD64>(lpFrames[i]);
		DEBUG_LOG(LL_CRI, "Current frame index: %d Addr: 0x%lld", i, ullCurrFrame);

		// Get module info
		IMAGEHLP_MODULE64 im64	= { 0 };
		im64.SizeOfStruct		= sizeof(IMAGEHLP_MODULE64);

		auto bGetModuleInfo = LI_FIND(SymGetModuleInfo64)(NtCurrentProcess, reinterpret_cast<DWORD64>(lpFrames[i]), &im64);
		if (bGetModuleInfo)
		{
			DEBUG_LOG(LL_CRI, "SymGetModuleInfo64: Module: 0x%lld Module Name: %s Image Name: %s Symbol name: %s",
				ullCurrFrame, im64.ModuleName, im64.ImageName, im64.LoadedImageName);
		}
		else
		{
			DEBUG_LOG(LL_CRI, "SymGetModuleInfo64 fail! Error: %u", LI_FIND(GetLastError)());
		}

		// Get symbol name
		auto dwDisplacement64		= 0ULL;
		char szSymbolName[MAX_PATH] = { 0 };
		if (GetSymbolName(ullCurrFrame, szSymbolName, &dwDisplacement64))
		{
			DEBUG_LOG(LL_CRI, "SymGetSymFromAddr64: Symbol name: %s", szSymbolName);
		}
		else
		{
			DEBUG_LOG(LL_CRI, "SymGetSymFromAddr64 fail! Error: %u", LI_FIND(GetLastError)());
		}

		// Get source filename and line
		auto dwDisplacement		= 0UL;
		IMAGEHLP_LINE64 il64	= { 0 };
		auto bGetLine = LI_FIND(SymGetLineFromAddr64)(NtCurrentProcess, ullCurrFrame, &dwDisplacement, &il64);
		if (bGetLine)
		{
			DEBUG_LOG(LL_CRI, "File name: %s File line: %u", il64.FileName, il64.LineNumber);
		}
		else
		{
			DEBUG_LOG(LL_CRI, "SymGetLineFromAddr64 fail! Error: %u", LI_FIND(GetLastError)());
		}
	}
	
	auto hModule = LI_FIND(GetModuleHandleA)(NULL);
	DEBUG_LOG(LL_CRI, "Base module: %p", hModule);

	char szModuleName[MAX_PATH] = { 0 };
	LI_FIND(GetModuleFileNameA)(hModule, szModuleName, sizeof(szModuleName));
	DEBUG_LOG(LL_CRI, "Module name: %s", szModuleName);

	auto pModuleTime = (time_t)LI_FIND(GetTimestampForLoadedLibrary)(hModule);
	DEBUG_LOG(LL_CRI, "Module time: 0x%08x - %s", pModuleTime, ctime(&pModuleTime));

	LI_FIND(SymCleanup)(NtCurrentProcess);
	return EXCEPTION_EXECUTE_HANDLER;
}
