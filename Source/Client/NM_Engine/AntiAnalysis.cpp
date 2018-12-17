#include "NM_Index.h"
#include "NM_Main.h"
#include "AntiDebug.h"
#include "DynamicWinapi.h"

void CAntiDebug::AntiCuckoo()
{
#ifndef _WIN64
	LPDWORD pOld;

	auto pFake = (LPDWORD)malloc(4096 * 100);
	memset(pFake, 1, 4096 * 100);

	__asm
	{
		mov eax, fs:[0x44]		// save old value
		mov pOld, eax

		mov eax, pFake			// replace with fake value
		mov fs : [0x44], eax
	}

	// this will not be logged nor executed.
	g_winapiApiTable->CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)g_winapiApiTable->Sleep, (LPVOID)1000, 0, NULL);

	__asm
	{
		mov eax, pOld		// restore old value, not reached if cuckoo
		mov fs : [0x44], eax
	}

	free(pFake);
#endif
}
