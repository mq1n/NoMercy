#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ManualMap.h"
#include "Defines.h"
#include "MappedFile.h"
#include "ThreadFunctions.h"
#include "ReflectiveDLLRefresher\src\ApiSetMap.h"

CManualMap::CManualMap()
{
	// ok // process, kernel32, kernelbase, ntdll, user32, ws2_32
	// next // anticheat, wow64cpu, msvcrt, win32u, psapi, winsta, gdi32, advapi32, wininet, iphlpapi, winmm
	// opt // d3d8, d3d9, d3d10, d3d11, d3d12, vulcan, opengl modules
}

std::shared_ptr <SMappedModuleInfo> CManualMap::GetMappedModuleInformations(HMODULE hTargetModule)
{
	auto pCurrMapInfo = std::shared_ptr<SMappedModuleInfo>();
	for (const auto & pCurrModule : m_vMappedModuleList)
	{
		if (IS_VALID_SMART_PTR(pCurrModule))
		{
			if (hTargetModule == pCurrModule->hModule)
			{
				pCurrMapInfo = pCurrModule;
				break;
			}
		}
	}
	return pCurrMapInfo;
}


static int ProtectionFlags[2][2][2] = 
{
	{
		// not executable
		{ PAGE_NOACCESS, PAGE_WRITECOPY },
		{ PAGE_READONLY, PAGE_READWRITE },
	},
	{
		// executable
		{ PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY },
		{ PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE },
	},
};

inline HMODULE CustomGetModuleHandle(const std::string & szModule)
{
	HMODULE hModule = NULL;
	auto pPEB = GetCurrentPEB();
	auto CurrentEntry = pPEB->LoaderData->InLoadOrderModuleList.Flink;

	DEBUG_LOG(LL_TRACE, "\t[*] Searching for loaded module: %s", szModule.c_str());

	while (CurrentEntry != &pPEB->LoaderData->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		auto Current = CONTAINING_RECORD(CurrentEntry, _LDR_MODULE, InLoadOrderModuleList);

		std::wstring wszCurrentModuleFullName(Current->FullDllName.Buffer, Current->FullDllName.Length / 2);
		std::string szCurrentModuleFullName(wszCurrentModuleFullName.begin(), wszCurrentModuleFullName.end());

		if (strcmp(szCurrentModuleFullName.c_str(), szModule.c_str()) == 0)
		{
			DEBUG_LOG(LL_TRACE, "Found in memory: %p", Current->BaseAddress);
			hModule = (HMODULE)Current->BaseAddress;
			break;
		}

		CurrentEntry = CurrentEntry->Flink;
	}

	if (!hModule)
		hModule = g_winapiApiTable->LoadLibraryA(szModule.c_str());

	if (!hModule)
	{
		DEBUG_LOG(LL_ERR, "Module can not loaded: %s", szModule.c_str());
		return hModule;
	}

	return hModule;
}

inline FARPROC WINAPI CustomGetProcAddress(const HMODULE hModule, PCSTR lpProcName)
{
	UINT_PTR uiLibraryAddress = 0;
	UINT_PTR uiAddressArray = 0;
	UINT_PTR uiNameArray = 0;
	UINT_PTR uiNameOrdinals = 0;
	UINT_PTR uiFuncVA = 0;
	PCHAR cpExportedFunctionName;

	PIMAGE_NT_HEADERS pNtHeaders = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	FARPROC fpResult = NULL;
	DWORD dwCounter;

	if (hModule == NULL)
		return NULL;

	// a module handle is really its base address
	uiLibraryAddress = (UINT_PTR)hModule;

	// get the VA of the modules NT Header
	pNtHeaders = (PIMAGE_NT_HEADERS)(uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew);

	pDataDirectory = (PIMAGE_DATA_DIRECTORY)&pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	// get the VA of the export directory
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(uiLibraryAddress + pDataDirectory->VirtualAddress);

	// get the VA for the array of addresses
	uiAddressArray = (uiLibraryAddress + pExportDirectory->AddressOfFunctions);

	// get the VA for the array of name pointers
	uiNameArray = (uiLibraryAddress + pExportDirectory->AddressOfNames);

	// get the VA for the array of name ordinals
	uiNameOrdinals = (uiLibraryAddress + pExportDirectory->AddressOfNameOrdinals);

	// test if we are importing by name or by ordinal...
#pragma warning(suppress: 4311)
	if (((DWORD64)lpProcName & 0xFFFF0000) == 0x00000000)
	{
		// import by ordinal...

		// use the import ordinal (- export ordinal base) as an index into the array of addresses
#pragma warning(suppress: 4311)
		uiAddressArray += ((IMAGE_ORDINAL((DWORD64)lpProcName) - pExportDirectory->Base) * sizeof(DWORD64));

		// resolve the address for this imported function
		fpResult = (FARPROC)(uiLibraryAddress + DEREF_32(uiAddressArray));
	}
	else
	{
		// import by name...
		dwCounter = pExportDirectory->NumberOfNames;
		while (dwCounter--)
		{
			cpExportedFunctionName = (PCHAR)(uiLibraryAddress + DEREF_32(uiNameArray));

			// test if we have a match...
			if (strcmp(cpExportedFunctionName, lpProcName) == 0)
			{
				// use the functions name ordinal as an index into the array of name pointers
				uiAddressArray += (DEREF_16(uiNameOrdinals) * sizeof(DWORD));
				uiFuncVA = DEREF_32(uiAddressArray);

				// check for redirected exports
				if (pDataDirectory->VirtualAddress <= uiFuncVA && uiFuncVA < (pDataDirectory->VirtualAddress + pDataDirectory->Size))
				{
					// Not implemented - Unnecessary
					fpResult = NULL;
				}
				else
				{
					// calculate the virtual address for the function
					fpResult = (FARPROC)(uiLibraryAddress + uiFuncVA);
				}

				// finish...
				break;
			}

			// get the next exported function name
			uiNameArray += sizeof(DWORD);

			// get the next exported function name ordinal
			uiNameOrdinals += sizeof(WORD);
		}
	}

	if (!fpResult) 
	{
		DEBUG_LOG(LL_TRACE, "API can not found: %s", lpProcName);
		return fpResult;
	}

	return fpResult;
}



bool CManualMap::ScanAndFixSection(PCHAR szSectionName, PVOID pKnown, PVOID pSuspect, SIZE_T stLength)
{
	DEBUG_LOG(LL_SYS, "Fixing section: %s Disk: %p Memory: %p Length: %p", szSectionName, pKnown, pSuspect, stLength);

	CThreadFunctions::ChangeThreadsStatus(true);

	DEBUG_LOG(LL_WARN, "\t[!] Found modification in: %s Current: %p Correct: %p Size: %p", szSectionName, pSuspect, pKnown, stLength);

	DWORD dwOldProtect = 0;
	auto bVPRet = g_winapiApiTable->VirtualProtectEx(NtCurrentProcess, pSuspect, stLength, PAGE_READWRITE, &dwOldProtect);
	if (!bVPRet)
	{
		DEBUG_LOG(LL_ERR, "VirtualProtectEx(1) fail! Error code: %u", g_winapiApiTable->GetLastError());
		CThreadFunctions::ChangeThreadsStatus(false);
		return false;
	}

	auto bFileCanRead = !IsBadReadPtr(pKnown, stLength);
	auto bMemCanWrite = !IsBadWritePtr(pSuspect, stLength);

	DEBUG_LOG(LL_SYS, "Copy routine has been started. Query: %d - %d", bFileCanRead, bMemCanWrite);

	auto dwWrittenByteCount = 0UL;
	if (bFileCanRead && bMemCanWrite)
	{
		auto bWriteRet = g_winapiApiTable->WriteProcessMemory(NtCurrentProcess, pSuspect, pKnown, stLength, &dwWrittenByteCount);
		DEBUG_LOG(LL_SYS, "Write routine completed. Ret: %d Size: %p/%p", bWriteRet, stLength, dwWrittenByteCount);

		if (!bWriteRet)
		{
			DEBUG_LOG(LL_ERR, "WriteProcessMemory fail! Error code: %u", g_winapiApiTable->GetLastError());
			CThreadFunctions::ChangeThreadsStatus(false);
			return false;
		}
	}

	bVPRet = g_winapiApiTable->VirtualProtectEx(NtCurrentProcess, pSuspect, stLength, dwOldProtect, &dwOldProtect);
	if (!bVPRet)
	{
		DEBUG_LOG(LL_ERR, "VirtualProtectEx(2) fail! Error code: %u", g_winapiApiTable->GetLastError());
		CThreadFunctions::ChangeThreadsStatus(false);
		return false;
	}

	CThreadFunctions::ChangeThreadsStatus(false);

	DEBUG_LOG(LL_SYS, "Completed!");
	return true;
}


bool CManualMap::ScanAndFixModule(PVOID pKnown, PVOID pSuspect, const std::string & szBaseDllName)
{
	DEBUG_LOG(LL_SYS, "Scanning module: %s", szBaseDllName.c_str());

	auto pFileIDH = (PIMAGE_DOS_HEADER)pKnown;
	if (!pFileIDH || pFileIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "File DOS Header is NOT valid! Base: %p", pFileIDH);
		return false;
	}
	auto pFileINH = (PIMAGE_NT_HEADERS)((DWORD)pKnown + pFileIDH->e_lfanew);
	if (pFileINH->Signature != IMAGE_NT_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "File NT Header is NOT valid! Base: %p", pFileINH);
		return false;
	}
	DEBUG_LOG(LL_SYS, "File Module headers are valid!");

	auto pMemIDH = (PIMAGE_DOS_HEADER)pSuspect;
	if (!pMemIDH || pMemIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "Memory DOS Header is NOT valid! Base: %p", pMemIDH);
		return false;
	}
	auto pMemINH = (PIMAGE_NT_HEADERS)((DWORD)pSuspect + pMemIDH->e_lfanew);
	if (pMemINH->Signature != IMAGE_NT_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "Memory NT Header is NOT valid! Base: %p", pMemINH);
		return false;
	}
	DEBUG_LOG(LL_SYS, "Memory Module headers are valid!");

	if (pFileINH->FileHeader.NumberOfSections != pMemINH->FileHeader.NumberOfSections)
	{
		DEBUG_LOG(LL_ERR, "Section count mismatch! File: %u Mem: %u", pFileINH->FileHeader.NumberOfSections, pMemINH->FileHeader.NumberOfSections);
		return false;
	}
	DEBUG_LOG(LL_SYS, "Section counts are valid! PE Header refresh has been start!");

	// Scan PE header
	std::string szHeaderTag = xorstr("Header").crypt_get();
	if (ScanAndFixSection((char*)szHeaderTag.c_str(), pKnown, pSuspect, pFileINH->OptionalHeader.SizeOfHeaders) == false)
	{
		DEBUG_LOG(LL_ERR, "PE Header refresh routine fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Module PE header refresh routine completed!");

	// Scan each section
	auto pMemSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pSuspect + pMemIDH->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	auto pFileSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pKnown + pFileIDH->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for (auto dwIdx = 0UL; dwIdx < pFileINH->FileHeader.NumberOfSections; dwIdx++)
	{
		DEBUG_LOG(LL_SYS, "[%u] Checking section: %s-%s Characteristics: %p Section size: %p | File: %p - %p(%p) | Memory: %p - %p(%p)", 
			dwIdx, pFileSectionHeader[dwIdx].Name, pMemSectionHeader[dwIdx].Name, pFileSectionHeader[dwIdx].Characteristics, pFileSectionHeader[dwIdx].Misc.VirtualSize,
			pKnown,
			(DWORD)pKnown + pFileSectionHeader[dwIdx].VirtualAddress,
			(DWORD)pKnown + pFileSectionHeader[dwIdx].SizeOfRawData,
			pSuspect,
			(DWORD)pSuspect + pMemSectionHeader[dwIdx].VirtualAddress,
			(DWORD)pSuspect + pMemSectionHeader[dwIdx].SizeOfRawData
		);

		if (pFileSectionHeader[dwIdx].SizeOfRawData == 0)
		{
			DEBUG_LOG(LL_WARN, "NULL sized section detected!");
		}

		if (pFileSectionHeader[dwIdx].Misc.VirtualSize != pMemSectionHeader[dwIdx].Misc.VirtualSize)
		{
			DEBUG_LOG(LL_WARN, "Section size mismatch! File: %p Mem: %p", pFileSectionHeader[dwIdx].Misc.VirtualSize, pMemSectionHeader[dwIdx].Misc.VirtualSize);
		}

		if ((pFileSectionHeader[dwIdx].Characteristics & IMAGE_SCN_MEM_EXECUTE) && (pFileSectionHeader[dwIdx].Characteristics & IMAGE_SCN_CNT_CODE) &&
			(pFileSectionHeader[dwIdx].Characteristics & IMAGE_SCN_MEM_READ)	&& !(pFileSectionHeader[dwIdx].Characteristics & IMAGE_SCN_MEM_WRITE))
		{
			DEBUG_LOG(LL_SYS, "Characteristics is OK for checking");

			auto dwFileReadByteCount = 0UL;
			auto dwMemReadByteCount = 0UL;

			std::vector <BYTE> vFileBuf(pFileSectionHeader[dwIdx].Misc.VirtualSize);
			auto bFileReadRet = g_winapiApiTable->ReadProcessMemory(NtCurrentProcess, reinterpret_cast<LPCVOID>((DWORD)pKnown + pFileSectionHeader[dwIdx].VirtualAddress), vFileBuf.data(), vFileBuf.size(), &dwFileReadByteCount);

			std::vector <BYTE> vMemBuf(pMemSectionHeader[dwIdx].Misc.VirtualSize);
			auto bMemReadRet = g_winapiApiTable->ReadProcessMemory(NtCurrentProcess, reinterpret_cast<LPCVOID>((DWORD)pSuspect + pMemSectionHeader[dwIdx].VirtualAddress), vMemBuf.data(), vMemBuf.size(), &dwMemReadByteCount);

			if (!bFileReadRet || !bMemReadRet || dwFileReadByteCount != vFileBuf.size() || dwMemReadByteCount != vMemBuf.size())
			{
				DEBUG_LOG(LL_ERR, "Memory read fail! File: %d(%p/%p) Mem: %d(%p/%p)",
					bFileReadRet, dwFileReadByteCount, vFileBuf.size(),
					bMemReadRet, dwMemReadByteCount, vMemBuf.size());
				continue;
			}

			if (!memcmp(vFileBuf.data(), vMemBuf.data(), vFileBuf.size()))
			{
				DEBUG_LOG(LL_SYS, "Sections are valid!");
				continue;
			}

			auto bSectionFixRet = ScanAndFixSection((PCHAR)pFileSectionHeader[dwIdx].Name, vFileBuf.data(), reinterpret_cast<LPVOID>((DWORD)pSuspect + pMemSectionHeader[dwIdx].VirtualAddress), vFileBuf.size());
			if (bSectionFixRet)
			{
				std::vector <BYTE> vMemBuf2(pMemSectionHeader[dwIdx].Misc.VirtualSize);
				bMemReadRet = g_winapiApiTable->ReadProcessMemory(NtCurrentProcess, reinterpret_cast<LPCVOID>((DWORD)pSuspect + pMemSectionHeader[dwIdx].VirtualAddress), vMemBuf2.data(), vMemBuf2.size(), &dwMemReadByteCount);

				if (!bMemReadRet || dwMemReadByteCount != vMemBuf2.size() || memcmp(vFileBuf.data(), vMemBuf2.data(), vFileBuf.size()))
				{
					DEBUG_LOG(LL_ERR, "Section fixed but still is NOT valid!");
					return false;
				}
			}

		}
	}
	return true;
}

bool CManualMap::ManualMapModule(const std::string & szFullDllName, const std::string & szBaseDllName, bool bRun, LPDWORD pdwBase, ULONG_PTR pDllBase)
{
	bool bRet = false;
	std::wstring wszDllName_;
	std::string szDllName_;

	PCHAR szDllName;
	size_t stDllName;
	PWSTR wszDllName = NULL;
	PWCHAR wsRedir = NULL;
	SIZE_T stRedirName;
	size_t stSize;

	HMODULE hModule;
	PIMAGE_THUNK_DATA pThunkData;
	FARPROC* pIatEntry;

	if (szFullDllName.empty())
	{
		DEBUG_LOG(LL_ERR, "Module full name is empty!");
		return bRet;
	}

	if (!g_nmApp->DirFunctionsInstance()->IsFileExist(szFullDllName))
	{
		DEBUG_LOG(LL_ERR, "Target module: %s file is NOT exist!", szFullDllName.c_str());
		return bRet;
	}

	// Step 1: Map the file into memory

	CMappedFile pMappedFile;
	LPVOID pMappedFileData;
	if (!pMappedFile.Create(szFullDllName.c_str(), &pMappedFileData, 0, 0))
	{
		pMappedFile.Destroy();
		return bRet;
	}
	auto pFile = (PBYTE)pMappedFileData;

	// Step 2: Parse the file headers and load it into memory

	auto pDosHeader = (PIMAGE_DOS_HEADER)pFile;
	if (!pDosHeader || pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "DOS header is NOT valid!");

		pMappedFile.Destroy();
		return bRet;
	}

	auto pNtHeader = (PIMAGE_NT_HEADERS)(pFile + pDosHeader->e_lfanew);
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "NT header is NOT valid!");
		
		pMappedFile.Destroy();
		return bRet;
	}

	// Allocate memory to copy DLL into
	auto pLibraryAddr = (PBYTE)VirtualAlloc(NULL, pNtHeader->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pLibraryAddr)
	{
		DEBUG_LOG(LL_ERR, "Memory can NOT allocated for map module copy! Last error: %u", g_winapiApiTable->GetLastError());

		pMappedFile.Destroy();
		return bRet;
	}

	if (pDllBase == NULL)
		pDllBase = (ULONG_PTR)pLibraryAddr;

	// Copy PE header
	memcpy(pLibraryAddr, pFile, pNtHeader->OptionalHeader.SizeOfHeaders);

	// Copy sections
	auto pSectionHeader = (PIMAGE_SECTION_HEADER)(pFile + pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for (std::size_t dwIdx = 0; dwIdx < pNtHeader->FileHeader.NumberOfSections; dwIdx++)
	{
		auto dwWritten = 0UL;
		auto bret = WriteProcessMemory(NtCurrentProcess,
			pLibraryAddr + pSectionHeader[dwIdx].VirtualAddress,
			pFile + pSectionHeader[dwIdx].PointerToRawData,
			pSectionHeader[dwIdx].SizeOfRawData,
			&dwWritten);
	}

	// Update our pointers to the loaded image
	pDosHeader = (PIMAGE_DOS_HEADER)pLibraryAddr;
	pNtHeader = (PIMAGE_NT_HEADERS)(pLibraryAddr + pDosHeader->e_lfanew);

	// Step 3: Calculate relocations

	auto pDataDir = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	auto pInitialImageBase = pNtHeader->OptionalHeader.ImageBase;

	// set the ImageBase to the already loaded module's base
	pNtHeader->OptionalHeader.ImageBase = pDllBase;

	// check if their are any relocations present
	if (pDataDir->Size)
	{
		// calculate the address of the first IMAGE_BASE_RELOCATION entry
		auto pBaseReloc = (PIMAGE_BASE_RELOCATION)(pLibraryAddr + pDataDir->VirtualAddress);

		// iterate through each relocation entry
//		while ((PBYTE)pBaseReloc < (pLibraryAddr + pDataDir->VirtualAddress + pDataDir->Size) && pBaseReloc->SizeOfBlock > 0)
		while (pBaseReloc->SizeOfBlock)
		{
			// the VA for this relocation block
			auto pReloc = (ULONG_PTR)(pLibraryAddr + pBaseReloc->VirtualAddress);

			// number of entries in this relocation block
			auto dwNumRelocs = (pBaseReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_RELOC);
//			auto dwNumRelocs = (pBaseReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) >> 1;

			// first entry in the current relocation block
			auto pImageReloc = (PIMAGE_RELOC)((PCHAR)pBaseReloc + sizeof(IMAGE_BASE_RELOCATION));

			// iterate through each entry in the relocation block
			while (dwNumRelocs--)
			{
				// perform the relocation, skipping IMAGE_REL_BASED_ABSOLUTE as required.
				// we subtract the initial ImageBase and add in the original dll base
				if (pImageReloc->type == IMAGE_REL_BASED_DIR64)
				{
					*(ULONG_PTR *)(pReloc + pImageReloc->offset) -= pInitialImageBase;
					*(ULONG_PTR *)(pReloc + pImageReloc->offset) += pDllBase;
				}
				else if (pImageReloc->type == IMAGE_REL_BASED_HIGHLOW)
				{
					*(DWORD *)(pReloc + pImageReloc->offset) -= (DWORD)pInitialImageBase;
					*(DWORD *)(pReloc + pImageReloc->offset) += (DWORD)pDllBase;
				}
				else if (pImageReloc->type == IMAGE_REL_BASED_HIGH)
				{
					*(WORD *)(pReloc + pImageReloc->offset) -= HIWORD(pInitialImageBase);
					*(WORD *)(pReloc + pImageReloc->offset) += HIWORD(pDllBase);
				}
				else if (pImageReloc->type == IMAGE_REL_BASED_LOW)
				{
					*(WORD *)(pReloc + pImageReloc->offset) -= LOWORD(pInitialImageBase);
					*(WORD *)(pReloc + pImageReloc->offset) += LOWORD(pDllBase);
				}

				// get the next entry in the current relocation block
				pImageReloc = (PIMAGE_RELOC)((PCHAR)pImageReloc + sizeof(IMAGE_RELOC));
			}

			// get the next entry in the relocation directory
			pBaseReloc = (PIMAGE_BASE_RELOCATION)((PCHAR)pBaseReloc + pBaseReloc->SizeOfBlock);
		}
	}

	// ----
	// Step 4: Update import table
	// ----

	pDataDir = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (pDataDir->Size)
	{
		auto pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pLibraryAddr + pDataDir->VirtualAddress);
		while (pImportDesc->Characteristics)
		{
			hModule = NULL;
			wszDllName = NULL;
			szDllName = (PCHAR)(pLibraryAddr + pImportDesc->Name);
			stDllName = strnlen(szDllName, MAX_PATH);
			wszDllName = (PWSTR)calloc(stDllName + 1, sizeof(WCHAR));

			if (wszDllName == NULL)
				goto next_import;

			mbstowcs_s(&stSize, wszDllName, stDllName + 1, szDllName, stDllName);

			// If the DLL starts with api- or ext-, resolve the redirected name and load it
			if (_wcsnicmp(wszDllName, L"api-", 4) == 0 || _wcsnicmp(wszDllName, L"ext-", 4) == 0)
			{
				// wsRedir is not null terminated
				std::wstring wszBaseDllName(szBaseDllName.begin(), szBaseDllName.end());
				wsRedir = GetRedirectedName((PWSTR)wszBaseDllName.c_str(), wszDllName, &stRedirName);
				if (wsRedir)
				{
					// Free the original wszDllName and allocate a new buffer for the redirected dll name
					free(wszDllName);
					wszDllName = (PWSTR)calloc(stRedirName + 1, sizeof(WCHAR));
					if (wszDllName == NULL)
						goto next_import;

					memcpy(wszDllName, wsRedir, stRedirName * sizeof(WCHAR));
				}
			}

			// Load the module

			wszDllName_ = wszDllName;
			szDllName_ = std::string(wszDllName_.begin(), wszDllName_.end());
			hModule = CustomGetModuleHandle(szDllName);

			// Ignore libraries that fail to load
			if (hModule == NULL)
				goto next_import;

			if (pImportDesc->OriginalFirstThunk)
				pThunkData = (PIMAGE_THUNK_DATA)(pLibraryAddr + pImportDesc->OriginalFirstThunk);
			else
				pThunkData = (PIMAGE_THUNK_DATA)(pLibraryAddr + pImportDesc->FirstThunk);

			pIatEntry = (FARPROC*)(pLibraryAddr + pImportDesc->FirstThunk);

			// loop through each thunk and resolve the import
			for (; DEREF(pThunkData); pThunkData++, pIatEntry++)
			{
				if (IMAGE_SNAP_BY_ORDINAL(pThunkData->u1.Ordinal))
					*pIatEntry = CustomGetProcAddress(hModule, (PCHAR)IMAGE_ORDINAL(pThunkData->u1.Ordinal));
				else
					*pIatEntry = CustomGetProcAddress(hModule, (PCHAR)((PIMAGE_IMPORT_BY_NAME)(pLibraryAddr + DEREF(pThunkData)))->Name);
			}

		next_import:
			if (wszDllName != NULL)
			{
				free(wszDllName);
				wszDllName = NULL;
			}
			pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((PCHAR)pImportDesc + sizeof(IMAGE_IMPORT_DESCRIPTOR));

		}
	}

	// ----
	// Step 5: Update section protection flags
	// ----

	pSectionHeader = (PIMAGE_SECTION_HEADER)(pFile + pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for (std::size_t dwIdx = 0; dwIdx < pNtHeader->FileHeader.NumberOfSections; dwIdx++)
	{
		DWORD protect, oldProtect, size;
		int executable = (pSectionHeader[dwIdx].Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		int readable = (pSectionHeader[dwIdx].Characteristics & IMAGE_SCN_MEM_READ) != 0;
		int writeable = (pSectionHeader[dwIdx].Characteristics & IMAGE_SCN_MEM_WRITE) != 0;

		protect = ProtectionFlags[executable][readable][writeable];
		if (pSectionHeader[dwIdx].Characteristics & IMAGE_SCN_MEM_NOT_CACHED)
			protect |= PAGE_NOCACHE;

		// determine size of region
		size = pSectionHeader[dwIdx].SizeOfRawData;
		if (size == 0)
		{
			if (pSectionHeader[dwIdx].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
				size = pNtHeader->OptionalHeader.SizeOfInitializedData;
			else if (pSectionHeader[dwIdx].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
				size = pNtHeader->OptionalHeader.SizeOfUninitializedData;
		}

		if (size > 0)
		{
 			VirtualProtect((LPVOID)(pLibraryAddr + pSectionHeader[dwIdx].VirtualAddress), pSectionHeader[dwIdx].SizeOfRawData, protect, &oldProtect);
		}
	}

	// TODO: X86 SafeSEH
//#ifdef _WIN64
#if 0
	// register exception handlers
	pDataDir = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
	if (pDataDir->Size != 0)
	{
		// auto RtlAddFunctionTable = (lpRtlAddFunctionTable)GetProcAddress(LoadLibrary("ntdll"), "RtlAddFunctionTable");
		RtlAddFunctionTable((PRUNTIME_FUNCTION)(pLibraryAddr + pDataDir->VirtualAddress), pDataDir->Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY), (DWORD_PTR)pLibraryAddr);
	}
#endif

	// Step 6: Run TLS & DLLMain
	if (bRun)
	{	
		//    NtFlushInstructionCache( (HANDLE)-1, NULL, 0 );

		pDataDir = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
		if (pDataDir->VirtualAddress != 0)
		{
			auto tls = (PIMAGE_TLS_DIRECTORY)(pLibraryAddr + pDataDir->VirtualAddress);
			auto callback = (PIMAGE_TLS_CALLBACK *)tls->AddressOfCallBacks;

			if (callback)
			{
				while (*callback)
				{
					(*callback)((LPVOID)pLibraryAddr, DLL_PROCESS_ATTACH, NULL);
					callback++;
				}
			}
		}

		if (pNtHeader->OptionalHeader.AddressOfEntryPoint)
		{
			auto pEntryPoint = (lpDLLMain)((LPBYTE)pLibraryAddr + pNtHeader->OptionalHeader.AddressOfEntryPoint);
			if (pEntryPoint)
			{
				pEntryPoint((HMODULE)pLibraryAddr, DLL_PROCESS_ATTACH, NULL);
			}
		}
	}

	bRet = true;

	if (!pMappedFile.IsNull())
		pMappedFile.Destroy();

	if (pdwBase)
		*pdwBase = (DWORD)pLibraryAddr;

	DEBUG_LOG(LL_SYS, "Module: %s mapped on: %p", szBaseDllName.c_str(), pLibraryAddr);
	return bRet;
}


bool CManualMap::MapSystemModules()
{
	auto pBaseModuleInfo = std::make_shared<SMappedModuleInfo>();
	if (IS_VALID_SMART_PTR(pBaseModuleInfo))
	{
		pBaseModuleInfo->hModule	= g_winapiModuleTable->hBaseModule;
		pBaseModuleInfo->bOptional	= true;
		strcpy(pBaseModuleInfo->szModuleBaseName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pBaseModuleInfo->hModule, false).c_str());
		strcpy(pBaseModuleInfo->szModuleFullName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pBaseModuleInfo->hModule, true).c_str());
	}
	m_vMappedModuleList.push_back(pBaseModuleInfo);

	auto pNtdllModuleInfo = std::make_shared<SMappedModuleInfo>();
	if (IS_VALID_SMART_PTR(pNtdllModuleInfo))
	{
		pNtdllModuleInfo->hModule	= g_winapiModuleTable->hNtdll;
		pNtdllModuleInfo->bOptional	= false;
		strcpy(pNtdllModuleInfo->szModuleBaseName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pNtdllModuleInfo->hModule, false).c_str());
		strcpy(pNtdllModuleInfo->szModuleFullName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pNtdllModuleInfo->hModule, true).c_str());
	}
	m_vMappedModuleList.push_back(pNtdllModuleInfo);

	auto pKernelbaseModuleInfo = std::make_shared<SMappedModuleInfo>();
	if (IS_VALID_SMART_PTR(pKernelbaseModuleInfo))
	{
		pKernelbaseModuleInfo->hModule		= g_winapiModuleTable->hKernelbase;
		pKernelbaseModuleInfo->bOptional	= true;
		strcpy(pKernelbaseModuleInfo->szModuleBaseName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pKernelbaseModuleInfo->hModule, false).c_str());
		strcpy(pKernelbaseModuleInfo->szModuleFullName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pKernelbaseModuleInfo->hModule, true).c_str());
	}
	m_vMappedModuleList.push_back(pKernelbaseModuleInfo);

	auto pKernel32ModuleInfo = std::make_shared<SMappedModuleInfo>();
	if (IS_VALID_SMART_PTR(pKernel32ModuleInfo))
	{
		pKernel32ModuleInfo->hModule	= g_winapiModuleTable->hKernel32;
		pKernel32ModuleInfo->bOptional	= false;
		strcpy(pKernel32ModuleInfo->szModuleBaseName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pKernel32ModuleInfo->hModule, false).c_str());
		strcpy(pKernel32ModuleInfo->szModuleFullName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pKernel32ModuleInfo->hModule, true).c_str());
	}
	m_vMappedModuleList.push_back(pKernel32ModuleInfo);

	auto pUser32ModuleInfo = std::make_shared<SMappedModuleInfo>();
	if (IS_VALID_SMART_PTR(pUser32ModuleInfo))
	{
		pUser32ModuleInfo->hModule		= g_winapiModuleTable->hUser32;
		pUser32ModuleInfo->bOptional	= false;
		strcpy(pUser32ModuleInfo->szModuleBaseName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pUser32ModuleInfo->hModule, false).c_str());
		strcpy(pUser32ModuleInfo->szModuleFullName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pUser32ModuleInfo->hModule, true).c_str());
	}
	m_vMappedModuleList.push_back(pUser32ModuleInfo);

	auto pWinsock2ModuleInfo = std::make_shared<SMappedModuleInfo>();
	if (IS_VALID_SMART_PTR(pWinsock2ModuleInfo))
	{
		pWinsock2ModuleInfo->hModule	= g_winapiModuleTable->hWs2_32;
		pWinsock2ModuleInfo->bOptional	= false;
		strcpy(pWinsock2ModuleInfo->szModuleBaseName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pWinsock2ModuleInfo->hModule, false).c_str());
		strcpy(pWinsock2ModuleInfo->szModuleFullName, g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD_PTR)pWinsock2ModuleInfo->hModule, true).c_str());
	}
	m_vMappedModuleList.push_back(pWinsock2ModuleInfo);

_scanLoop:
	for (std::size_t i = 0; i < m_vMappedModuleList.size(); ++i)
	{
		auto pCurrModule = m_vMappedModuleList[i];
		if (IS_VALID_SMART_PTR(pCurrModule))
		{
			if (pCurrModule->dwMappedBase)
			{
				DEBUG_LOG(LL_SYS, "Current module: %s(%s) already mapped!", pCurrModule->szModuleBaseName, pCurrModule->szModuleFullName);
				continue;
			}

			DEBUG_LOG(LL_SYS, "Current module: %s(%s) Mem: %p Mapped: %p Optional: %d",
				pCurrModule->szModuleBaseName, pCurrModule->szModuleFullName, pCurrModule->hModule, pCurrModule->dwMappedBase, pCurrModule->bOptional);

			auto bLoadRet = ManualMapModule(pCurrModule->szModuleFullName, pCurrModule->szModuleBaseName, false, &pCurrModule->dwMappedBase, NULL);
			if (!bLoadRet || !pCurrModule->dwMappedBase)
			{
				DEBUG_LOG(LL_CRI, "Module: %s(%s) %p can NOT mapped! Error: %u Opt: %d",  
					pCurrModule->szModuleBaseName, pCurrModule->szModuleFullName, pCurrModule->hModule, g_winapiApiTable->GetLastError(), pCurrModule->bOptional);

				if (pCurrModule->bOptional == false)
					return false;
				
				m_vMappedModuleList.erase(std::remove(m_vMappedModuleList.begin(), m_vMappedModuleList.end(), pCurrModule), m_vMappedModuleList.end());
				goto _scanLoop;
			}

			DEBUG_LOG(LL_SYS, "Module: %s File copy mapped to memory at %p", pCurrModule->szModuleBaseName, pCurrModule->dwMappedBase);

#if 0
			if (pCurrModule->hModule == g_winapiModuleTable->hBaseModule)
				g_winapiModuleTable->hBaseModule = reinterpret_cast<HMODULE>(pCurrModule->dwMappedBase);

			else if (pCurrModule->hModule == g_winapiModuleTable->hNtdll)
				g_winapiModuleTable->hNtdll = reinterpret_cast<HMODULE>(pCurrModule->dwMappedBase);

			else if (pCurrModule->hModule == g_winapiModuleTable->hKernelbase)
				g_winapiModuleTable->hKernelbase = reinterpret_cast<HMODULE>(pCurrModule->dwMappedBase);

			else if (pCurrModule->hModule == g_winapiModuleTable->hKernel32)
				g_winapiModuleTable->hKernel32 = reinterpret_cast<HMODULE>(pCurrModule->dwMappedBase);

			else if (pCurrModule->hModule == g_winapiModuleTable->hUser32)
				g_winapiModuleTable->hUser32 = reinterpret_cast<HMODULE>(pCurrModule->dwMappedBase);

			else if (pCurrModule->hModule == g_winapiModuleTable->hWs2_32)
				g_winapiModuleTable->hWs2_32 = reinterpret_cast<HMODULE>(pCurrModule->dwMappedBase);
#endif
		}
	}

#if 0
	g_nmApp->DynamicWinapiInstance()->BindAPIs();
#endif

	return true;
}

bool CManualMap::RefreshModules()
{
#if 0
	for (const auto & pCurrModule : m_vMappedModuleList)
	{
		if (IS_VALID_SMART_PTR(pCurrModule))
		{
			DEBUG_LOG(LL_SYS, "Target module: %s(%s) -> Mem: %p Mapped: %p", pCurrModule->szModuleBaseName, pCurrModule->szModuleFullName, pCurrModule->hModule, pCurrModule->dwMappedBase);

			if (ScanAndFixModule((LPVOID)pCurrModule->dwMappedBase, pCurrModule->hModule, pCurrModule->szModuleBaseName) == false)
			{
				DEBUG_LOG(LL_ERR, "Module: %s(p) reload fail!", pCurrModule->szModuleBaseName, pCurrModule->hModule);
				return false;
			}
			DEBUG_LOG(LL_SYS, "DLL reload routine completed for: %s", pCurrModule->szModuleBaseName);
		}
	}
#endif

	return true;
}

DWORD WINAPI CheckModuleModificationsRoutine(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Manual map thread event has been started!");

	auto pMMapRoutineTimer	= CTimer<std::chrono::milliseconds>();

	auto currentThread		= std::shared_ptr<CThread>();
	do
	{
		if (pMMapRoutineTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_MANUALMAP_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_MMAPMODULES);

	} while (!IS_VALID_SMART_PTR(currentThread));

	// Initilization routine
	if (g_nmApp->DataInstance()->GetAppType() == NM_CLIENT)
	{
		if (g_nmApp->SelfHooksInstance()->InitializeSelfAPIHooks() == false)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_SELF_API_HOOK_INIT_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}

		if (g_nmApp->DynamicWinapiInstance()->PatchWow64Redirection() == false)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_WOW64_FIX_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}

		if (g_nmApp->AccessHelperInstance()->SetMitigationPolicys() == false)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_MITIGATION_INIT_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}
	}

	// Loop routine
	static auto iIndex = 0;
	while (true)
	{
		if (iIndex >= INT_MAX - 1) 
			iIndex = 1;

		auto pLimitTimer = CTimer<std::chrono::milliseconds>();

		if (g_nmApp->DataInstance()->GetAppType() == NM_CLIENT && g_nmApp->SelfHooksInstance()->HooksIsInitialized() == false)
		{
			if (pLimitTimer.diff() > 10000)
			{
				g_nmApp->OnCloseRequest(EXIT_ERR_MMAP_HOOK_CHECK_TIMEOUT, g_winapiApiTable->GetLastError());
				return 0;
			}

			DEBUG_LOG(LL_SYS, "Hooks not initialized yet!");

			// Update ticks
			g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_MMAPMODULES);
			g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_MMAPMODULES, GetCurrentTime());

			g_winapiApiTable->Sleep(1000);
			continue;
		}

		if (g_nmApp->ManualMapInstance()->RefreshModules() == false)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_MMAP_ROUTINE_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}

		DEBUG_LOG(LL_SYS, "[%d] Manual map reload module routine completed!", iIndex++);

		// Update ticks
		g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_MMAPMODULES);
		g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_MMAPMODULES, GetCurrentTime());

		g_winapiApiTable->Sleep(5000);
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc4()
{
	__asm
	{
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
	}
}
#endif

bool CNM_App::InitCheckModuleModifications()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)CheckModuleModificationsRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_MMAPMODULES, CheckModuleModificationsRoutine, ulFuncSize, nullptr, 5000 + 1000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}

