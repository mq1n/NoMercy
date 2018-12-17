#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "WinVerHelper.h"

#ifndef _M_X64

inline wchar_t* GetBaseName(wchar_t* string)
{
	unsigned long i = wcslen(string);
	while (string[i - 1] != '\\') i--;
	return &string[i];
}

unsigned short cs_ = 0;
unsigned char* WOW32Reserved = 0;
unsigned long PEB64 = 0;
__forceinline void GetWow32ReservedInfo()
{
	__asm
	{
		pushad
			mov eax, dword ptr fs : [0xC0]
			mov WOW32Reserved, eax
			mov eax, dword ptr fs : [0x30]
			add eax, 0x1000
			mov PEB64, eax
			mov cs_, cs
			popad
	}
}

#endif

inline bool Wow32ReservedIsHooked()
{
#ifndef _M_X64
	bool bIsHooked = false;

	if (g_nmApp->FunctionsInstance()->IsSysWow64() == false)
	{
		DEBUG_LOG(LL_SYS, "IsSysWow64 returned as false, Skipped Wow32reserved hook check");
		return bIsHooked;
	}

	GetWow32ReservedInfo();

	if (!WOW32Reserved)
	{
		DEBUG_LOG(LL_SYS, "WOW32Reserved returned as false, Skipped Wow32reserved hook check");
		return bIsHooked;  //not 64-bit system
	}

	if ((*WOW32Reserved == 0xEA) && (*(unsigned short*)(WOW32Reserved + 5) != cs_))
	{
		unsigned long CpupReturnFromSimulatedCode = *(unsigned long*)(WOW32Reserved + 1);
		MEMORY_BASIC_INFORMATION MBI = { 0 };
		g_winapiApiTable->VirtualQuery((void*)CpupReturnFromSimulatedCode, &MBI, sizeof(MBI));

		if (MBI.Type == MEM_IMAGE)
		{
			char* p = (char*)g_winapiApiTable->LocalAlloc(LMEM_ZEROINIT, 0x1000);
			if (NT_SUCCESS(g_winapiApiTable->NtQueryVirtualMemory(NtCurrentProcess, (void*)CpupReturnFromSimulatedCode, MemoryMappedFilenameInformation /*filename*/, p, 0x1000, 0) >= 0))
			{
				if (((UNICODE_STRING*)p)->Length)
				{
					std::string szWow64Cpu = xorstr("wow64cpu.dll").crypt_get();
					std::wstring wszWow64Cpu(szWow64Cpu.begin(), szWow64Cpu.end());

					if (lstrcmpiW(wszWow64Cpu.c_str(), GetBaseName(((UNICODE_STRING*)p)->Buffer)) == 0)
					{
						g_winapiApiTable->LocalFree(p);
						return bIsHooked; // not hooked
					}
				}
			}
			g_winapiApiTable->LocalFree(p);
		}
	}

	return bIsHooked; // hooked
#else
	return false;
#endif
}

void IScanner::CheckWow32ReservedHook()
{
	DEBUG_LOG(LL_SYS, "AntiWow32ReservedHook event started!");

	if (Wow32ReservedIsHooked()) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_WOW32RESERVED_HOOK_DETECTED, g_winapiApiTable->GetLastError());
		return;
	}

	DEBUG_LOG(LL_SYS, "AntiWow32ReservedHook event completed!");
}
