#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "PEHelper.h"
#include "ProcessFunctions.h"
#include "MappedFile.h"

#include <rewolf-wow64ext/src/wow64ext.h>

#define ENUM_PROCESS_MODULES_LIMIT 0x800

bool CheckModulePEHeaderHash(HANDLE hProcess, const std::string & szModuleName, PVOID pModuleDump, std::size_t cbHeaderSize)
{
	auto szCurrHash = g_nmApp->DirFunctionsInstance()->GetSHA256(pModuleDump, cbHeaderSize);
	if (szCurrHash.empty())
	{
		// ...
	}

//	g_nmApp->QuarentineInstance()->CheckModulePeHeaderHash(szCurrHash);
	return true;
}
bool CheckModulePEHeaderWipe(HANDLE hProcess, const std::string & szModuleName, PVOID pModuleDump, std::size_t cbHeaderSize)
{
	BYTE pNullBuf[] = { 0x0 };
	if (!memcmp(pModuleDump, pNullBuf, cbHeaderSize))
	{
//		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();
	}

	return true;
}
bool CheckModulePEHeaderFill(HANDLE hProcess, const std::string & szModuleName, PVOID pModuleDump, std::size_t cbHeaderSize)
{
	// Disables file system redirection for the calling thread.
	PVOID OldValue = NULL;
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &OldValue))
		return true;

	auto hFile			= g_winapiApiTable->CreateFileA(szModuleName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	auto dwLastError	= g_winapiApiTable->GetLastError();

	// Restore file system redirection for the calling thread.
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, OldValue, nullptr))
		return true;

	if (!IS_VALID_HANDLE(hFile))
	{
		SCANNER_LOG(LL_ERR, "CreateFileA fail! File: '%s' Error: %u", szModuleName.c_str(), dwLastError);

		if (dwLastError != ERROR_ACCESS_DENIED && dwLastError != ERROR_SHARING_VIOLATION)
			return false;
		return true;
	}

	LARGE_INTEGER liFileSize;
	if (!GetFileSizeEx(hFile, &liFileSize))
	{
		SCANNER_LOG(LL_ERR, "GetFileSizeEx fail! Error: %u", g_winapiApiTable->GetLastError());

		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pFileBuffer = g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(liFileSize.LowPart, __FUNCTION__);
	if (!pFileBuffer)
	{
		SCANNER_LOG(LL_ERR, "Memory can NOT allocated! Error: %u", g_winapiApiTable->GetLastError());

		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	DWORD dwBytesRead;
	if (!ReadFile(hFile, pFileBuffer, liFileSize.LowPart, &dwBytesRead, NULL) || dwBytesRead != liFileSize.LowPart)
	{
		SCANNER_LOG(LL_ERR, "ReadFile fail! Error: %u", g_winapiApiTable->GetLastError());
		
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pFileIDH = (IMAGE_DOS_HEADER*)pFileBuffer;
	if (pFileIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		SCANNER_LOG(LL_ERR, "File dos signature check fail!");
		
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pFileINH = (IMAGE_NT_HEADERS*)((BYTE*)pFileIDH + pFileIDH->e_lfanew);
	if (pFileINH->Signature != IMAGE_NT_SIGNATURE)
	{
		SCANNER_LOG(LL_ERR, "File nt signature check fail!");
		
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pFileHeader = (IMAGE_OPTIONAL_HEADER32*)&pFileINH->OptionalHeader;
	if (!pFileHeader)
	{
		SCANNER_LOG(LL_ERR, "File optional header not found!");
		
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pMemIDH = (IMAGE_DOS_HEADER*)pModuleDump;
	if (pMemIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		SCANNER_LOG(LL_ERR, "Mem dos signature check fail!");

		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pMemINH = (IMAGE_NT_HEADERS*)((BYTE*)pMemIDH + pMemIDH->e_lfanew);
	if (pMemINH->Signature != IMAGE_NT_SIGNATURE)
	{
		SCANNER_LOG(LL_ERR, "Mem nt signature check fail!");

		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pMemHeader = (IMAGE_OPTIONAL_HEADER32*)&pMemINH->OptionalHeader;
	if (!pMemHeader)
	{
		SCANNER_LOG(LL_ERR, "File optional header not found!");

		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	if (pFileINH->FileHeader.TimeDateStamp != pMemINH->FileHeader.TimeDateStamp)
	{
//		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();
	}

	if (pFileHeader->SizeOfImage != pMemHeader->SizeOfImage)
	{
//		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();
	}

	if (pFileHeader->ImageBase != pMemHeader->ImageBase)
	{
//		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();
	}

	if (pFileHeader->SizeOfInitializedData != pMemHeader->SizeOfInitializedData)
	{
//		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();
	}

	g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hFile);

	return true;
}
bool CheckModulePEHeaderDestroy(HANDLE hProcess, const std::string & szModuleName, DWORD64 pModuleBase, std::size_t cbHeaderSize)
{
	auto ntStat = NTSTATUS(0x0);

	MEMORY_BASIC_INFORMATION64 mbi = { 0 };
	if (!VirtualQueryEx64(hProcess, pModuleBase, &mbi, sizeof(mbi)))
	{
		SCANNER_LOG(LL_ERR, "NtQueryVirtualMemory fail! Target process: %p Status: %p", hProcess, ntStat);
		return false;
	}

	if (mbi.State == MEM_FREE)
	{
//		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();
	}
	return true;
}


bool ScanModuleHeader(HANDLE hProcess, const std::string & szModuleName, DWORD64 pModuleBase, std::size_t cbModuleSize)
{
	SCANNER_LOG(LL_SYS, "Module header scan started! Target process: %p(%u) Module name: %s Module data: %p-%u", 
		hProcess, g_winapiApiTable->GetProcessId(hProcess), szModuleName.c_str(), pModuleBase, cbModuleSize);

	auto bRet				= false;
	auto ntStat				= DWORD64(0x0);
#ifdef _M_X64
	auto dwReadByteCount	= 0ULL;
#else
	auto dwReadByteCount	= 0UL;
#endif
	auto dwReadSize			= (cbModuleSize < 0x10000) ? cbModuleSize : 0x10000;
	auto dwPEHeaderSize		= 0UL;

	auto pModuleDump = g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(dwReadSize, __FUNCTION__);
	if (!pModuleDump)
	{
		SCANNER_LOG(LL_ERR, "Memory can NOT allocated! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

	ntStat = ReadProcessMemory64(hProcess, pModuleBase, pModuleDump, dwReadSize, &dwReadByteCount);
	if (!NT_SUCCESS(ntStat))
	{
		SCANNER_LOG(LL_ERR, "ReadProcessMemory64 fail! Target process: %p Status: %p", hProcess, ntStat);
		goto _Complete;
	}

	dwPEHeaderSize = CPEFunctions::GetPEHeaderSize((DWORD)pModuleDump);
	if (!dwPEHeaderSize)
	{
		SCANNER_LOG(LL_ERR, "PE Header size find fail!");
		goto _Complete;
	}

	SCANNER_LOG(LL_SYS, "Module dump succesfully created! Base: %p Size: %u", pModuleDump, dwPEHeaderSize);

	if (false == CheckModulePEHeaderDestroy(hProcess, szModuleName, pModuleBase, cbModuleSize))
	{
		SCANNER_LOG(LL_ERR, "PE Header was destroyed!");
		goto _Complete;
	}
	if (false == CheckModulePEHeaderWipe(hProcess, szModuleName, pModuleDump, dwPEHeaderSize))
	{
		SCANNER_LOG(LL_ERR, "PE Header was wiped!");
		goto _Complete;
	}
	if (false == CheckModulePEHeaderFill(hProcess, szModuleName, pModuleDump, dwPEHeaderSize))
	{
		SCANNER_LOG(LL_ERR, "PE Header was fill'd with fake data!");
		goto _Complete;
	}
	if (false == CheckModulePEHeaderHash(hProcess, szModuleName, pModuleDump, dwPEHeaderSize))
	{
		SCANNER_LOG(LL_ERR, "PE Header hash blacklisted!");
		goto _Complete;
	}

	bRet = true;

_Complete:
	if (pModuleDump)
	{
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pModuleDump);
	}
	return bRet;
}

bool ScanModuleInformations(HANDLE hProcess, const std::string & szModuleName, DWORD64 dwModuleBase, DWORD dwModuleSize)
{
	auto bRet					= false;
	auto ntStat					= DWORD64(0x0);
	auto dwReadFileByteCount	= ULONG(0);
#ifdef _M_X64
	auto dwReadMemByteCount		= 0ULL;
#else
	auto dwReadMemByteCount		= 0UL;
#endif
	auto szMemoryHash			= std::string("");
	auto szMappedHash			= std::string("");
	auto pOldFSValue			= PVOID(nullptr);
	auto pFileBuffer			= PVOID(nullptr);
	auto hFile					= HANDLE(INVALID_HANDLE_VALUE);
	auto dwLastError			= 0UL;
	LARGE_INTEGER liFileSize	= { 0 };

	// Dump module

	auto pModuleDump = g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(dwModuleSize, __FUNCTION__);
	if (!pModuleDump)
	{
		SCANNER_LOG(LL_ERR, "Memory can NOT allocated! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

	ntStat = ReadProcessMemory64(hProcess, dwModuleBase, pModuleDump, dwModuleSize, &dwReadMemByteCount);
	if (!NT_SUCCESS(ntStat) || dwModuleSize != dwReadMemByteCount)
	{
		SCANNER_LOG(LL_ERR, "ReadProcessMemory64 fail! Target process: %p Status: %p", hProcess, ntStat);
		goto _Complete;
	}

	// Get module's memory hash

	szMemoryHash = g_nmApp->DirFunctionsInstance()->GetSHA256(pModuleDump, dwModuleSize);
	if (szMemoryHash.empty())
	{
		SCANNER_LOG(LL_ERR, "Memory hash can NOT generated! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

//	g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();

	// Get module's mapped hash

	// Disables file system redirection for the calling thread.
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &pOldFSValue))
		goto _Complete;

	hFile = g_winapiApiTable->CreateFileA(szModuleName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	dwLastError = g_winapiApiTable->GetLastError();

	// Restore file system redirection for the calling thread.
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, pOldFSValue, nullptr))
		goto _Complete;

	if (!IS_VALID_HANDLE(hFile))
	{
		SCANNER_LOG(LL_ERR, "CreateFileA fail! File: '%s' Error: %u", szModuleName.c_str(), dwLastError);

		if (dwLastError == ERROR_ACCESS_DENIED)
			bRet = true;

		goto _Complete;
	}

	if (!GetFileSizeEx(hFile, &liFileSize))
	{
		SCANNER_LOG(LL_ERR, "GetFileSizeEx fail! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

	pFileBuffer = g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(liFileSize.LowPart, __FUNCTION__);
	if (!pFileBuffer)
	{
		SCANNER_LOG(LL_ERR, "Memory can NOT allocated! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

	if (!ReadFile(hFile, pFileBuffer, liFileSize.LowPart, &dwReadFileByteCount, NULL) || dwReadFileByteCount != liFileSize.LowPart)
	{
		SCANNER_LOG(LL_ERR, "ReadFile fail! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

	szMappedHash = g_nmApp->DirFunctionsInstance()->GetSHA256(pFileBuffer, liFileSize.LowPart);
	if (szMappedHash.empty())
	{
		SCANNER_LOG(LL_ERR, "Memory hash can NOT generated! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

//	g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer();

	bRet = true;

_Complete:
	if (pModuleDump)
	{
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pModuleDump);
	}
	if (pFileBuffer)
	{
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBuffer);
	}
	if (IS_VALID_HANDLE(hFile))
	{
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hFile);
	}
	return bRet;
}

bool ScanModuleBase(HANDLE hProcess, const std::string & szModuleName, DWORD64 dwModuleBase, DWORD dwModuleSize)
{
	auto bRet					= false;
	auto ntStat					= DWORD64(0x0);
#ifdef _M_X64
	auto dwReadByteCount		= 0ULL;
#else
	auto dwReadByteCount		= 0UL;
#endif

	auto pModuleDump = g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(dwModuleSize, __FUNCTION__);
	if (!pModuleDump)
	{
		SCANNER_LOG(LL_ERR, "Memory can NOT allocated! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

	ntStat = ReadProcessMemory64(hProcess, dwModuleBase, pModuleDump, 12, &dwReadByteCount);
	if (!NT_SUCCESS(ntStat) || 12 != dwReadByteCount)
	{
		SCANNER_LOG(LL_ERR, "ReadProcessMemory64 fail! Target process: %p Status: %p", hProcess, ntStat);
		goto _Complete;
	}

//	g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(pModuleDump, 12);

	bRet = true;

_Complete:
	if (pModuleDump)
	{
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pModuleDump);
	}
	return bRet;
}

bool IScanner::ScanModuleLinks(HANDLE hProcess, DWORD64 dwModuleBase)
{
	auto bFound = false;

	if (!IS_VALID_HANDLE(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target handle is NOT valid!");
		return true;
	}

	if (!g_nmApp->DynamicWinapiInstance()->IsValidHandle(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target process is NOT active!");
		return true;
	}

	static auto vModuleList = GetModuleList(hProcess);
	if (vModuleList.empty())
	{
		SCANNER_LOG(LL_ERR, "Module list is NULL!");
		return false;
	}

	for (const auto & pCurrModule : vModuleList)
	{
		if (IS_VALID_SMART_PTR(pCurrModule))
		{
			if (pCurrModule->ullBaseAddress == dwModuleBase)
			{
				bFound = true;
			}
		}
	}

	if (bFound == false) // check alloc base and base is it equal 
	{
		// manually mapped or unlinked module found
//		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer
	}

	return true;
}



bool IScanner::IsScannedModule(const std::string & szModuleName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	return HasInVector(m_vScannedModuleNames, szModuleName);
}

void IScanner::ScanModule(HANDLE hProcess, const std::string & szModuleName, DWORD64 dwModuleBase, DWORD dwModuleSize)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	SCANNER_LOG(LL_SYS, "Module scanner has been started! Target module: %s(%p-%p) Target proc: %p(%u)",
		szModuleName.c_str(), dwModuleBase, dwModuleSize, hProcess, g_winapiApiTable->GetProcessId(hProcess));

	if (IsScannedModule(szModuleName))
	{
		SCANNER_LOG(LL_SYS, "Module already scanned!");
		return;
	}

	// Add to checked list
	m_vScannedModuleNames.push_back(szModuleName);

	// Scan routine

	auto bRet = false;

	// TODO: Scan export names

	/// 1
	// Scan module file
	bRet = ScanFile(hProcess, FILE_TYPE_MODULE);
//	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Module file scan routine completed! Result: %d", bRet);

	/// 2
	// Scan module entrypoint
	bRet = ScanModuleBase(hProcess, szModuleName, dwModuleBase, dwModuleSize);
//	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Module link scan routine completed! Result: %d", bRet);

	/// 3
	// Scan module informations
	bRet = ScanModuleInformations(hProcess, szModuleName, dwModuleBase, dwModuleSize);
//	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Module information scan routine completed! Result: %d", bRet);

	/// 4
	// Scan module pe header
	bRet = ScanModuleHeader(hProcess, szModuleName, dwModuleBase, dwModuleSize);
//	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Module header scan routine completed! Result: %d", bRet);
}


std::vector < std::shared_ptr <SModuleEnumContext> > IScanner::GetModuleList(HANDLE hProcess)
{
	auto vOutput		= std::vector < std::shared_ptr <SModuleEnumContext> >();
	auto iModuleCount	= 0;
	auto iRetryCount	= 0;

	auto szProcessName = CProcessFunctions::GetProcessName(hProcess);
	if (szProcessName.empty())
	{
		SCANNER_LOG(LL_ERR, "Process name read fail! Target process: %p Error: %u", hProcess, g_winapiApiTable->GetLastError());
		return vOutput;
	}

	if (g_nmApp->FunctionsInstance()->IsWow64Process(NtCurrentProcess))
	{
		auto ntStat  = DWORD64(0x0);
		PROCESS_BASIC_INFORMATION_WOW64 pPBI = { 0 };
		ntStat = g_winapiApiTable->NtWow64QueryInformationProcess64(hProcess, ProcessBasicInformation, &pPBI, sizeof(pPBI), NULL);
		if (!NT_SUCCESS(ntStat))
		{
			SCANNER_LOG(LL_ERR, "NtWow64QueryInformationProcess64(ProcessBasicInformation) fail! Target process: %p Status: %p", hProcess, ntStat);
			return vOutput;
		}

		PEB64 pPEB = { 0 };
		SIZE_T ulReadBytes;
		ntStat = ReadProcessMemory64(hProcess, (DWORD64)pPBI.PebBaseAddress, &pPEB, sizeof(pPEB), &ulReadBytes);
		if (!NT_SUCCESS(ntStat))
		{
			SCANNER_LOG(LL_ERR, "ReadProcessMemory64(1) fail! Target process: %p Status: %p", hProcess, ntStat);
			return vOutput;
		}

		if (!pPEB.Ldr)
		{
			SCANNER_LOG(LL_ERR, "Peb Loader data is null!");
			return vOutput;
		}

		PEB_LDR_DATA64 pPebLdrData = { 0 };
		ntStat = ReadProcessMemory64(hProcess, (DWORD64)pPEB.Ldr, &pPebLdrData, sizeof(pPebLdrData), &ulReadBytes);
		if (!NT_SUCCESS(ntStat))
		{
			SCANNER_LOG(LL_ERR, "ReadProcessMemory64(2) fail! Target process: %p Status: %p", hProcess, ntStat);
			return vOutput;
		}

		auto Head = pPebLdrData.InLoadOrderModuleList.Flink;
		auto Node = Head;

		Head -= sizeof(LIST_ENTRY64);
	
		do
		{
			LDR_DATA_TABLE_ENTRY64 pCurrModule = { 0 };
			ntStat = ReadProcessMemory64(hProcess, (DWORD64)Node, &pCurrModule, sizeof(pCurrModule), NULL);
			if (NT_SUCCESS(ntStat))
			{
				if (pCurrModule.DllBase)
				{
					++iModuleCount;

					WCHAR wstrModuleName[MAX_PATH] = { L'\0' };
					ntStat = ReadProcessMemory64(hProcess, (DWORD64)pCurrModule.FullDllName.Buffer, &wstrModuleName, pCurrModule.FullDllName.Length, &ulReadBytes);
					if (!NT_SUCCESS(ntStat))
					{
						SCANNER_LOG(LL_ERR, "ReadProcessMemory64(4) fail! Target process: %p Status: %p", hProcess, ntStat);
						continue;
					}

					auto pCurrModuleCtx = std::make_shared<SModuleEnumContext>();
					if (!IS_VALID_SMART_PTR(pCurrModuleCtx))
					{
						SCANNER_LOG(LL_ERR, "Module enum context container can NOT allocated! Error: %u", g_winapiApiTable->GetLastError());
						return vOutput;
					}

					pCurrModuleCtx->ullBaseAddress	= (DWORD64)pCurrModule.DllBase;
					pCurrModuleCtx->uiModuleSize	= pCurrModule.SizeOfImage;
					wcscpy(pCurrModuleCtx->wszModuleName, wstrModuleName);

					vOutput.push_back(pCurrModuleCtx);
				}
			}
			else
			{
				if (++iRetryCount == 3)
				{
					break;
				}
			}

			Node = pCurrModule.InLoadOrderLinks.Flink;

		} while (Head != Node && iModuleCount < ENUM_PROCESS_MODULES_LIMIT);
	}
	else // non wow64
	{
		TODO_MSG; // Implement Non-wow64 routine

#ifdef _WIN32
		// ...
#else
		// ...
#endif

	}
	return vOutput;
}

bool IScanner::EnumerateModules(HANDLE hProcess)
{
	SCANNER_LOG(LL_SYS, "Module enumerator has been started!");

	auto vModuleList = GetModuleList(hProcess);
	if (vModuleList.empty())
	{
		SCANNER_LOG(LL_ERR, "Module list is NULL!");
		return false;
	}

	for (const auto & pCurrModule : vModuleList)
	{
		if (IS_VALID_SMART_PTR(pCurrModule))
		{
			auto wszModuleName	= std::wstring(pCurrModule->wszModuleName);
			auto szModuleName	= std::string(wszModuleName.begin(), wszModuleName.end());

			ScanModule(hProcess, szModuleName, pCurrModule->ullBaseAddress, pCurrModule->uiModuleSize);
		}
	}

	return true;
}

bool IScanner::ScanProcessModules(HANDLE hProcess)
{
	SCANNER_LOG(LL_SYS, "Module scanner has been started! Target process: %u(%p)", g_winapiApiTable->GetProcessId(hProcess), hProcess);

	if (!IS_VALID_HANDLE(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target handle is NOT valid!");
		return true;
	}

	if (!g_nmApp->DynamicWinapiInstance()->IsValidHandle(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target process is NOT active!");
		return true;
	}

	auto szProcessName = CProcessFunctions::GetProcessName(hProcess);
	if (szProcessName.empty())
	{
		SCANNER_LOG(LL_ERR, "Process name read fail! Target process: %p Error: %u", hProcess, g_winapiApiTable->GetLastError());
		return false;
	}
	SCANNER_LOG(LL_SYS, "Process image name: %s", szProcessName.c_str());

	return EnumerateModules(hProcess);
}

