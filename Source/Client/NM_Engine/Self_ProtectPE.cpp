#include "NM_Index.h"
#include "NM_Main.h"
#include "SelfProtection.h"
#include "DynamicWinapi.h"
#include "Defines.h"

void CSelfProtection::ProtectSelfPE(HMODULE hModule)
{
	auto pIDH = reinterpret_cast<IMAGE_DOS_HEADER*>(hModule);
	if (pIDH->e_magic == IMAGE_DOS_SIGNATURE)
	{
		auto dwOldProtect = 0UL;
		if (g_winapiApiTable->VirtualProtect(pIDH, 1024, PAGE_READWRITE, &dwOldProtect))
		{
			pIDH->e_magic = 0;

			auto pINH = reinterpret_cast<IMAGE_NT_HEADERS*>(pIDH + 1);
			pINH->Signature = 0;

			g_winapiApiTable->VirtualProtect(pIDH, 1024, dwOldProtect, &dwOldProtect);
		}
	}
}

