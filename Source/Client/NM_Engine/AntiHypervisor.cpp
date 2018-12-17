#include "NM_Index.h"
#include "NM_Main.h"
#include "AntiDebug.h"

bool CAntiDebug::CheckCPUId()
{
	INT CPUInfo[4] = { -1 };

	/* Query hypervisor precense using CPUID (EAX=1), BIT 31 in ECX */
	__cpuid(CPUInfo, 1);
	return ((CPUInfo[2] >> 31) & 1);
}

bool CAntiDebug::HypervisorCheckTriggered()
{
	auto bRet = FALSE;
#ifndef _M_X64
	__asm
	{
		pushad
			pushfd
			pop eax
			or eax, 0x00200000
			push eax
			popfd
			pushfd
			pop eax
			and eax, 0x00200000
			jz CPUID_NOT_SUPPORTED
			xor eax, eax
			xor edx, edx
			xor ecx, ecx
			xor ebx, ebx
			inc eax
			cpuid
			test ecx, 0x80000000
			jnz Hypervisor
			mov bRet, 0
			jmp bye
		Hypervisor :
		mov bRet, 1
			jmp bye
		CPUID_NOT_SUPPORTED :
		mov bRet, 2
		bye :
			popad
	}
#endif
	return (bRet == TRUE);
}

bool CAntiDebug::CheckUdpPorts()
{
	auto bRet = false;
	auto pUdpTable = (PMIB_UDPTABLE)g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(sizeof(PMIB_UDPTABLE), __FUNCTION__);

	auto dwNeedSize = 0UL;
	auto dwUdpRet = GetUdpTable(pUdpTable, &dwNeedSize, TRUE);

	if (dwUdpRet == ERROR_INSUFFICIENT_BUFFER)
	{
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pUdpTable);
		pUdpTable = (PMIB_UDPTABLE)g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(dwNeedSize, __FUNCTION__);

		dwUdpRet = GetUdpTable(pUdpTable, &dwNeedSize, TRUE);
	}

	if (dwUdpRet == NO_ERROR)
	{
		if (pUdpTable->dwNumEntries > 0)
		{
			auto iPortCount = 0UL;
			for (auto i = 0UL; i < pUdpTable->dwNumEntries; i++)
			{
				switch (pUdpTable->table[i].dwLocalPort)
				{
					case 67: // DHCP
					case 68: // DHCP
					case 69: // TFTP
					case 4011: // PXE
						iPortCount++;
						break;
				}
			}

			if (iPortCount >= 3)
			{
				bRet = true;
			}
		}
	}

	free(pUdpTable);
	return bRet;
}

