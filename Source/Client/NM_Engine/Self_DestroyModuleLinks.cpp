#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "SelfProtection.h"
#include "DynamicWinapi.h"
#include "Defines.h"
#include "ThreadFunctions.h"

void OnThreadAttached()
{
	if (IS_VALID_SMART_PTR(g_winapiApiTable) && g_winapiApiTable->GetCurrentThreadId)
	{
		if (IS_VALID_SMART_PTR(g_nmApp) && g_nmApp->AppIsInitiliazed())
		{
			DEBUG_LOG(LL_SYS, "Thread attached! TID: %u", g_winapiApiTable->GetCurrentThreadId());

			auto dwErrorType = 0UL;
			if (g_nmApp->AnalyserInstance()->OnThreadCreated(g_winapiApiTable->GetCurrentThreadId(), 0, 0, &dwErrorType) == false)
			{
				g_nmApp->OnCloseRequest(EXIT_ERR_UNKNOWN_THREAD_ATTACHED, g_winapiApiTable->GetLastError(), (void*)dwErrorType);
				return;
			}
		}
	}
}

BOOL APIENTRY FakeMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
//	if (dwReason == DLL_THREAD_ATTACH)
//		OnThreadAttached();

	return TRUE;
}
#ifndef _M_X64

typedef struct LDR_DATA_ENTRY {
	LIST_ENTRY              InMemoryOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_DATA_ENTRY, *PLDR_DATA_ENTRY;
__declspec(naked) PLDR_DATA_ENTRY GetLdrDataEntry() 
{
	__asm
	{
		mov eax, fs:[0x30]
		mov eax, [eax + 0x0C]
		mov eax, [eax + 0x1C]
		retn
	}
}
#endif

void CSelfProtection::HideModuleLinks(HMODULE hModule)
{
#ifndef _M_X64
	auto cursor = GetLdrDataEntry();
	while (cursor->BaseAddress)
	{
		cursor = (PLDR_DATA_ENTRY)cursor->InMemoryOrderModuleList.Flink;
		if (cursor->BaseAddress == hModule)
		{
			auto prev = (PLDR_DATA_ENTRY)cursor->InMemoryOrderModuleList.Blink;

			cursor->BaseDllName = prev->BaseDllName;
			cursor->FullDllName = prev->FullDllName;

			cursor->SizeOfImage = 0;
			cursor->EntryPoint	= &FakeMain;

			// TODO: unlink from hash table
			cursor->HashTableEntry.Blink->Flink = cursor->HashTableEntry.Flink;
			cursor->HashTableEntry.Flink->Blink = cursor->HashTableEntry.Blink;
		}
	}
#endif
}
