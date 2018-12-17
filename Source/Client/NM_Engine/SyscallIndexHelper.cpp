#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "SyscallIndexHelper.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "PEHelper.h"
#include "LDasm.h"

#include <rewolf-wow64ext/src/wow64ext.h>


CSyscallIndexHelper::CSyscallIndexHelper()
{
	BuildSyscallList();
}
CSyscallIndexHelper::~CSyscallIndexHelper()
{
	DestroySyscallList();
}

DWORD ParseFromNtdllMemory(DWORD dwAddress)
{
	if (dwAddress && *reinterpret_cast<byte*>(dwAddress) == 0xB8)
	{
		return *reinterpret_cast<DWORD*>(dwAddress + 1);
	}

	return 0;
}

bool ParseFromNtdllFile(const std::string & szAPIName, LPDWORD pdwSysIndex)
{
	auto bIsX64 = g_nmApp->FunctionsInstance()->IsX64System();

	auto dwAPIAddr = 0ULL;
	if (bIsX64)
	{
		dwAPIAddr =  (DWORD64)GetProcAddress64(GetModuleHandle64(xorstr(L"ntdll.dll").crypt_get()), szAPIName.c_str());
		dwAPIAddr -= (DWORD64)GetModuleHandle64(xorstr(L"ntdll.dll").crypt_get());
	}
	else
	{
		dwAPIAddr = (DWORD64)GetProcAddress(GetModuleHandleA(xorstr("ntdll.dll").crypt_get()), szAPIName.c_str());
		dwAPIAddr -= (DWORD64)GetModuleHandleA(xorstr("ntdll.dll").crypt_get());
	}

	auto dwFileOffset = 0UL;
	if (bIsX64)
	{
		BYTE pNtdllHeader[PE_HEADER_SIZE] = { 0x0 };
		getMem64(pNtdllHeader, GetModuleHandle64(xorstr(L"ntdll.dll").crypt_get()), PE_HEADER_SIZE);

		auto pIDH = (PIMAGE_DOS_HEADER)pNtdllHeader;
		if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
			return false;

		auto pINH = (PIMAGE_NT_HEADERS64)((ULONG_PTR)pIDH + pIDH->e_lfanew);
		if (pINH->Signature != IMAGE_NT_SIGNATURE)
			return false;

		auto pISH = (PIMAGE_SECTION_HEADER)(sizeof(IMAGE_NT_SIGNATURE) + sizeof(IMAGE_FILE_HEADER) + pINH->FileHeader.SizeOfOptionalHeader + (ULONG_PTR)pINH);
		for (std::size_t i = 0; i < pINH->FileHeader.NumberOfSections; i++)
		{
			if (pISH->VirtualAddress <= dwAPIAddr && dwAPIAddr <= (pISH->VirtualAddress + pISH->Misc.VirtualSize))
			{
				break;
			}
			pISH++;
		}
		dwFileOffset = (DWORD)(dwAPIAddr - pISH->VirtualAddress + pISH->PointerToRawData);
	}
	else
	{
		auto pIDH = (PIMAGE_DOS_HEADER)GetModuleHandleA(xorstr("ntdll.dll").crypt_get());
		if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
			return false;

		auto pINH = (PIMAGE_NT_HEADERS)((char*)pIDH + pIDH->e_lfanew);
		if (pINH->Signature != IMAGE_NT_SIGNATURE)
			return false;

		auto pISH = (PIMAGE_SECTION_HEADER)(sizeof(IMAGE_NT_SIGNATURE) + sizeof(IMAGE_FILE_HEADER) + pINH->FileHeader.SizeOfOptionalHeader + (ULONG_PTR)pINH);
		for (std::size_t i = 0; i < pINH->FileHeader.NumberOfSections; i++)
		{
			if (pISH->VirtualAddress <= dwAPIAddr && dwAPIAddr <= (pISH->VirtualAddress + pISH->Misc.VirtualSize))
			{
				break;
			}
			pISH++;
		}
		dwFileOffset = (DWORD)(dwAPIAddr - pISH->VirtualAddress + pISH->PointerToRawData);
	}
	if (dwFileOffset == 0)
	{
		return false;
	}

	PVOID OldValue = NULL;
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &OldValue))
		return false;

	auto szNtdll = g_nmApp->DirFunctionsInstance()->SystemPath() + xorstr("\\ntdll.dll").crypt_get();
	auto hNtdll = g_winapiApiTable->CreateFileA(szNtdll.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!IS_VALID_HANDLE(hNtdll))
	{
		return false;
	}

	SetFilePointer(hNtdll, dwFileOffset, NULL, FILE_CURRENT);

	auto dwReadByteCount	= 0UL;
	BYTE pOpcode[64]		= { 0 };
	if (!ReadFile(hNtdll, pOpcode, sizeof(pOpcode), &dwReadByteCount, NULL))
	{
		return false;
	}

	CloseHandle(hNtdll);

	auto dwEax = 0UL;
	auto pEip  = pOpcode;
	while (TRUE)
	{
		ldasm_data ld = { 0 };
		auto dwLength = ldasm(pEip, &ld, bIsX64);

		if (dwLength == 5 && pEip[0] == 0xB8)
		{
			dwEax = *(DWORD*)(&pEip[1]);
			break;
		}
		pEip += dwLength;
	}

	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, OldValue, nullptr))
		return false;

	if (pdwSysIndex) *pdwSysIndex = dwEax;

	return true;
}

void CSyscallIndexHelper::RegisterSyscall(const std::string & szFunction,
	DWORD dwXPsp0idx, DWORD dwXPsp1idx, DWORD dwXPsp2idx, DWORD dwXPsp3idx,
	DWORD dwVistaSp0idx, DWORD dwVistaSp1idx, DWORD dwVistaSp2idx, 
	DWORD dwSevenSp0idx, DWORD dwSevenSp1idx, 
	DWORD dwEightidx, DWORD dwEightPointOneIdx, 
	DWORD dwTenBuild1507Idx, DWORD dwTenBuild1511Idx, DWORD dwTenBuild1607Idx, DWORD dwTenBuild1703Idx, DWORD dwTenBuild1709Idx, DWORD dwTenBuild1803Idx)
{
	auto pSyscallIndexContainer = std::make_shared<SSyscallIndexList>();

	if (IS_VALID_SMART_PTR(pSyscallIndexContainer))
	{
		pSyscallIndexContainer->dwFunc = g_nmApp->FunctionsInstance()->GetStringHash((LPVOID)szFunction.c_str(), FALSE, szFunction.size());

		pSyscallIndexContainer->dwWinXPSP0 = dwXPsp0idx;
		pSyscallIndexContainer->dwWinXpSP1 = dwXPsp1idx;
		pSyscallIndexContainer->dwWinXpSP2 = dwXPsp2idx;
		pSyscallIndexContainer->dwWinXPSP3 = dwXPsp3idx;

		pSyscallIndexContainer->dwWinVistaSP0 = dwVistaSp0idx;
		pSyscallIndexContainer->dwWinVistaSP1 = dwVistaSp1idx;
		pSyscallIndexContainer->dwWinVistaSP2 = dwVistaSp2idx;

		pSyscallIndexContainer->dwWinSevenSP0 = dwSevenSp0idx;
		pSyscallIndexContainer->dwWinSevenSP1 = dwSevenSp1idx;

		pSyscallIndexContainer->dwWinEight = dwEightidx;
		pSyscallIndexContainer->dwWinEightPointOne = dwEightPointOneIdx;

		pSyscallIndexContainer->dwWinTenBuild1507 = dwTenBuild1507Idx;
		pSyscallIndexContainer->dwWinTenBuild1511 = dwTenBuild1511Idx;
		pSyscallIndexContainer->dwWinTenBuild1607 = dwTenBuild1607Idx;
		pSyscallIndexContainer->dwWinTenBuild1703 = dwTenBuild1703Idx;
		pSyscallIndexContainer->dwWinTenBuild1709 = dwTenBuild1709Idx;
		pSyscallIndexContainer->dwWinTenBuild1803 = dwTenBuild1803Idx;

		m_vSyscallIndexContainer.push_back(pSyscallIndexContainer);
	}
}

DWORD CSyscallIndexHelper::GetSyscallId(DWORD dwFunctionStrHash)
{
	DEBUG_LOG(LL_SYS, "Syscall index search started for: %p", dwFunctionStrHash);

	auto dwWinMajorVer	= GetWindowsMajorVersion();
	auto dwWinMinorVer	= GetWindowsMinorVersion();
	auto dwWinSPVer		= GetWindowsServicePackVersion();
	auto dwWinBuildVer	= GetWindowsBuildNumber();

	for (const auto & pCurrSyscall : m_vSyscallIndexContainer)
	{
		if (IS_VALID_SMART_PTR(pCurrSyscall))
		{
			if (pCurrSyscall->dwFunc == dwFunctionStrHash)
			{
				if (dwWinMajorVer == 10) // Windows Ten
				{
					if (dwWinBuildVer == 10240)
						return pCurrSyscall->dwWinTenBuild1507;

					else if (dwWinBuildVer == 10586)
						return pCurrSyscall->dwWinTenBuild1511;

					else if (dwWinBuildVer == 14393)
						return pCurrSyscall->dwWinTenBuild1607;

					else if (dwWinBuildVer == 15063)
						return pCurrSyscall->dwWinTenBuild1703;
	
					else if (dwWinBuildVer == 16299)
						return pCurrSyscall->dwWinTenBuild1709;
				
					else if (dwWinBuildVer == 17133 || dwWinBuildVer == 17134)
						return pCurrSyscall->dwWinTenBuild1803;
				}

				else if (dwWinMajorVer == 6) // Windows Vista, Seven, Eight
				{
					if (dwWinMinorVer == 0) // Vista
					{
						if (dwWinSPVer == 2)
							return pCurrSyscall->dwWinVistaSP2;

						else if (dwWinSPVer == 1)
							return pCurrSyscall->dwWinVistaSP1;

						else if (dwWinSPVer == 0)
							return pCurrSyscall->dwWinVistaSP0;
					}

					else if (dwWinMinorVer == 1) // Seven
					{
						if (dwWinSPVer == 1)
							return pCurrSyscall->dwWinSevenSP1;

						else if (dwWinSPVer == 0)
							return pCurrSyscall->dwWinSevenSP0;
					}

					else if (dwWinMinorVer == 2) // Eight
						return pCurrSyscall->dwWinEight;

					else if (dwWinMinorVer == 3) // Eight point one
						return pCurrSyscall->dwWinEightPointOne;
				}

				else if (dwWinMajorVer == 5 && dwWinMinorVer == 1) // Windows XP
				{
					if (dwWinSPVer == 3)
						return pCurrSyscall->dwWinXPSP3;

					else if (dwWinSPVer == 2)
						return pCurrSyscall->dwWinXpSP2;

					else if (dwWinSPVer == 1)
						return pCurrSyscall->dwWinXpSP1;

					else if (dwWinSPVer == 0)
						return pCurrSyscall->dwWinXPSP0;
				}
			}
		}
	}

	DEBUG_LOG(LL_SYS, "Syscall index not found! Func: %p Win: %u-%u-%u(%u)", dwFunctionStrHash, dwWinMajorVer, dwWinMinorVer, dwWinSPVer, dwWinSPVer);
	return 0;
}

DWORD CSyscallIndexHelper::GetSyscallId(const std::string & szFunction)
{
	DEBUG_LOG(LL_SYS, "Syscall index search started for: %s", szFunction.c_str());

	auto dwFunctionHash = g_nmApp->FunctionsInstance()->GetStringHash((LPVOID)szFunction.c_str(), FALSE, szFunction.size());
	if (!dwFunctionHash)
	{
		DEBUG_LOG(LL_ERR, "String hash can NOT created!");
		return 0;
	}

	return GetSyscallId(dwFunctionHash);
}

DWORD CSyscallIndexHelper::GetSyscallId(const std::wstring & wszFunction)
{
	DEBUG_LOG(LL_SYS, "Syscall index search started for: %ls", wszFunction.c_str());
	
	auto dwFunctionHash = g_nmApp->FunctionsInstance()->GetStringHash((LPVOID)wszFunction.c_str(), TRUE, wszFunction.size());
	if (!dwFunctionHash)
	{
		DEBUG_LOG(LL_ERR, "String hash can NOT created!");
		return 0;
	}

	return GetSyscallId(dwFunctionHash);
}


void CSyscallIndexHelper::BuildSyscallList()
{
	/*
	Function name,
	XP Index List,
	Vista Index List,
	7 Index List,
	8 Index List,
	10 Index List
	*/

	RegisterSyscall(xorstr("NtProtectVirtualMemory").crypt_get(),
		0x004d, 0x004d, 0x004d, 0x004d,
		0x004d, 0x004d, 0x004d,
		0x004d, 0x004d,
		0x004e, 0x004f,
		0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050
	);
	RegisterSyscall(xorstr("NtWriteVirtualMemory").crypt_get(), 
		0x0037, 0x0037, 0x0037, 0x0037,
		0x0037, 0x0037, 0x0037, 
		0x0037, 0x0037,
		0x0038, 0x0039,
		0x003a, 0x003a, 0x003a, 0x003a, 0x003a, 0x003a
	);
	RegisterSyscall(xorstr("NtSuspendProcess").crypt_get(),
		0x0117, 0x0117, 0x0117, 0x0117,
		0x0178, 0x0171, 0x0171,
		0x017a, 0x017a,
		0x0192, 0x0197,
		0x019f, 0x01a2, 0x01a8, 0x01ae, 0x01b1, 0x01b3
	);
}

void CSyscallIndexHelper::DestroySyscallList()
{
	m_vSyscallIndexContainer.clear();
}

