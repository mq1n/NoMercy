#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_SDK_Manager.h"
#include "Metin2_SDK_Manager.h"
#include "Defines.h"
#include "PEHelper.h"

// TODO:
// check 1000x damage msa file hash, add wolfman support, add map file check for wallhack
// create callback for py script map func
// PyString_InternFromString, Py_InitModule hook

CMetin2SDKMgr::CMetin2SDKMgr() :
	m_bInitialized(false), m_bEterPackCheckEnabled(true),
	m_pSend(nullptr), m_pSendSequence(nullptr),
	m_pGetVID(nullptr), m_pGetPhase(nullptr), m_pGetPlayerName(nullptr),
	m_pGetMappedFileIsExist(nullptr), m_pGetMappedFileHash(nullptr)
{
	m_vFunctions.clear();
}

void CMetin2SDKMgr::OnClientMessage(int Code, const void* lpMessage)
{
	if (Code == NM_DATA_SEND_NET_SEND_PROXY)
		m_pSend = (lpSendWrapper)lpMessage;
	if (Code == NM_DATA_SEND_NET_SEND_SEQ)
		m_pSendSequence = (lpSendSequenceFunc)lpMessage;

	if (Code == NM_DATA_SEND_VID)
		m_pGetVID = (lpGetVID)lpMessage;
	if (Code == NM_DATA_SEND_PHASE)
		m_pGetPhase = (lpGetPhase)lpMessage;
	if (Code == NM_DATA_SEND_PLAYER_NAME)
		m_pGetPlayerName = (lpGetPlayerName)lpMessage;

	if (Code == NM_DATA_SEND_MAPPED_FILE_EXIST)
		m_pGetMappedFileIsExist = (lpIsMappedFileExist)lpMessage;
	if (Code == NM_DATA_SEND_MAPPED_FILE_HASH)
		m_pGetMappedFileHash = (lpGetMappedFileHash)lpMessage;

	auto pData = std::make_shared<SM2SDKFuncData>();
	if (IS_VALID_SMART_PTR(pData))
	{
		pData->c_pFuncAddr	= lpMessage;
		pData->iCode		= Code;

		m_vFunctions.push_back(pData);
	}

	if (m_pSend && m_pSendSequence && m_pGetVID && m_pGetPhase && m_pGetPhase && m_pGetPlayerName && m_pGetMappedFileIsExist && m_pGetMappedFileHash)
	{
		m_bInitialized = true;

		if (m_pGetMappedFileHash("") == -2)
			m_bEterPackCheckEnabled = false;
//		else
//			LPThreads->InitMetin2PackHashCheck();
	}
}

void CMetin2SDKMgr::OnGameTick()
{

}


bool CMetin2SDKMgr::FunctionIsInGameArea(DWORD dwAddress)
{
	DWORD dwBase = 0;
	DWORD dwSize = 0;
	if (!CPEFunctions::GetTextSectionInformation((DWORD)g_winapiModuleTable->hBaseModule, &dwBase, &dwSize))
		return false;

	auto dwCodeHi = dwBase + dwSize;
	if (dwAddress >= dwBase && dwAddress <= dwCodeHi)
		return true;

	return false;
}

bool CMetin2SDKMgr::VerifyFunctionModules() // TODO: Call when everything is ready
{
	for (const auto & pCurrFunc : m_vFunctions)
	{
		if (IS_VALID_SMART_PTR(pCurrFunc))
		{
			if (pCurrFunc->iCode >= NM_DATA_SEND_TRACEERROR /* min */ && pCurrFunc->iCode <= NM_DATA_SEND_NET_SEND_SEQ /* max */)
			{
				if (!pCurrFunc->c_pFuncAddr)
				{
					DEBUG_LOG(LL_CRI, "Null func? Code: %d", pCurrFunc->iCode);
					return false;
				}

				if (FunctionIsInGameArea((DWORD)pCurrFunc->c_pFuncAddr) == false)
				{
					DEBUG_LOG(LL_CRI, "Hooked func? Code: %d Adr: %p", pCurrFunc->iCode, pCurrFunc->c_pFuncAddr);
					return false;
				}
			}
		}
	}
	return true;
}

void CMetin2SDKMgr::DumpMappedFileHashes()
{
#if 0
	char szXoredName[1024];
	for (int i = 0; strcmp(st_FileHashList_normal[i].c_szFileName, "XXX"); i++)
	{
		sprintf(szXoredName, "xorstr('%s').crypt_get()", st_FileHashList_normal[i].c_szFileName);
		const char* c_szCurrentFileName = st_FileHashList_normal[i].c_szFileName;
		bool bIsExist = G_isExist(c_szCurrentFileName);

		DWORD dwHash = G_GetMappedFileHash(c_szCurrentFileName);
		if (dwHash == -1) {
			LPLog->M2HashLog(0, "*** ERROR: File: %s can not readed IsExist: %d!", c_szCurrentFileName, bIsExist);
			continue;
		}

		LPLog->M2HashLog(0, "{ '%s',		0x%x }, // %d", szXoredName, dwHash, i);
	}

	for (i = 0; strcmp(st_FileHashList_map_standart[i].c_szFileName, "XXX"); i++)
	{
		Thread: %u", sprintf(szXoredName, "xorstr('%s').crypt_get()", st_FileHashList_map_standart[i].c_szFileName);
		const char* c_szCurrentFileName = st_FileHashList_map_standart[i].c_szFileName;
		bool bIsExist = G_isExist(c_szCurrentFileName);

		DWORD dwHash = G_GetMappedFileHash(c_szCurrentFileName);
		if (dwHash == -1) {
			LPLog->M2HashLog(0, "*** ERROR: File: %s can not readed IsExist: %d!", c_szCurrentFileName, bIsExist);
			continue;
		}

		LPLog->M2HashLog(0, "{ '%s',		0x%x }, // %d", szXoredName, dwHash, i);
	}

	LPLog->AddLog(0, "File Hash dump event completed.");
#endif
}


void CMetin2SDKMgr::CheckMappedFiles()
{
#if 0
	for (int i = 0; strcmp(st_FileHashList_normal[i].c_szFileName, xorstr("XXX").crypt_get()); i++)
	{
		const char* c_szCurrentFileName = st_FileHashList_normal[i].c_szFileName;
		DWORD dwCurrentCRC32 = st_FileHashList_normal[i].dwCRC32;
		bool bIsExist = G_isExist(c_szCurrentFileName);


		CHAR __wolfman[] = { 'w', 'o', 'l', 'f', 'm', 'a', 'n', 0x0 }; // wolfman
		if (!bIsExist && strstr(c_szCurrentFileName, __wolfman))
			continue;

		if (!bIsExist) {
			// char cRealWarn[1024];
			// CHAR _warn[] = { 'G', 'a', 'm', 'e', ' ', 'f', 'i', 'l', 'e', ' ', 'n', 'o', 't', ' ', 'f', 'o', 'u', 'n', 'd', '!', ' ', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'c', 'o', 'n', 't', 'a', 'c', 't', ' ', 'w', 'i', 't', 'h', ' ', 's', 'e', 'r', 'v', 'e', 'r', ' ', 'a', 'd', 'm', 'i', 'n', '.', ' ', 'E', 'r', 'r', 'o', 'r', ' ', 'c', 'o', 'd', 'e', ':', ' ', '%', 'd', 0x0 }; // Game file not found! Please contact with server admin. Error code: %d
			// sprintf(cRealWarn, _warn, i);

			// CFunctions lpFuncs;
#ifndef _DEBUG
			// LPFunctions->CloseProcess(cRealWarn, false, "");
			continue;
#else
			LPLog->AddLog(0, "ERROR! File hash check event failed! Real game hashes now dumping...");
			DumpRealHashes();
			break;
#endif
		}

		DWORD dwRealHash = G_GetMappedFileHash(c_szCurrentFileName);
		if (dwRealHash == -1 && bIsExist) {
			char cRealWarn[1024];
			CHAR _warn[] = { 'G', 'a', 'm', 'e', ' ', 'f', 'i', 'l', 'e', ' ', 'c', 'a', 'n', ' ', 'n', 'o', 't', ' ', 'r', 'e', 'a', 'd', 'e', 'd', '!', ' ', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'd', 'i', 's', 'a', 'b', 'l', 'e', ' ', 'a', 'n', 't', 'i', 'v', 'i', 'r', 'u', 's', ' ', 's', 'o', 'f', 't', 'w', 'a', 'r', 'e', 's', ' ', 'a', 'n', 'd', ' ', 't', 'r', 'y', ' ', 'a', 'g', 'a', 'i', 'n', '.', ' ', 'E', 'r', 'r', 'o', 'r', ' ', 'c', 'o', 'd', 'e', ':', ' ', '%', 'd', 0x0 }; // Game file can not readed! Please disable antivirus softwares and try again. Error code: %d
			sprintf(cRealWarn, _warn, i);

#ifndef _DEBUG
			LPFunctions->CloseProcess(cRealWarn, false, "");
#else
			LPLog->AddLog(0, "ERROR! File hash check event failed! Real game hashes now dumping...");
			DumpRealHashes();
			break;
#endif
		}

		if (dwCurrentCRC32 != dwRealHash) {
			char cRealWarn[1024];
			CHAR _warn[] = { 'G', 'a', 'm', 'e', ' ', 'f', 'i', 'l', 'e', ' ', 'm', 'o', 'd', 'i', 'f', 'i', 'c', 'a', 't', 'i', 'o', 'n', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '!', ' ', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'r', 'e', 'i', 'n', 's', 't', 'a', 'l', 'l', ' ', 'g', 'a', 'm', 'e', '.', ' ', 'E', 'r', 'r', 'o', 'r', ' ', 'c', 'o', 'd', 'e', ':', ' ', '%', 'd', 0x0 }; // Game file modification detected! Please reinstall game. Error code: %d
			sprintf(cRealWarn, _warn, i);

#ifndef _DEBUG
			LPFunctions->CloseProcess(cRealWarn, false, "");
#else
			LPLog->AddLog(0, "ERROR! File hash check event failed! Real game hashes now dumping...");
			DumpRealHashes();
			break;
#endif
		}

		// BetaFunctionTable->Sleep(1);
	}

#endif
}

DWORD WINAPI MappedFileCheckRoutine(LPVOID)
{
#if 0
	do {
		BetaFunctionTable->Sleep(2000);
		lpPlugin.CheckIngameHashes();

		BetaFunctionTable->Sleep(15000);
	} while (lpPlugin.G_IsInitialized() &&
		IsInitializedTrigger(TRIGGER_ETERPACK_ISEXIST) && IsInitializedTrigger(TRIGGER_NETWORKSTREAM_GETHASHFROMMAPPEDFILE)
	);
#endif

	return 0;
}

#if 0
HANDLE CPluginMetin2::InitCheckIngame()
{
#ifdef _DEBUG
	LPLog->AddLog(0, "Metin2 file hash check event thread creation has been started!");
#endif

	KARMA_MACRO_1

	HANDLE hThread = LPThreads->_CreateThread((LPTHREAD_START_ROUTINE)InitializeCheckIngame, 0, 11);
	if (!hThread) {
		CHAR __warn[] = { 'E', 'R', 'R', 'O', 'R', '!', ' ', 'T', 'h', 'r', 'e', 'a', 'd', ' ', 'c', 'r', 'e', 'a', 't', 'i', 'o', 'n', ' ', 'f', 'a', 'i', 'l', 'e', 'd', ',', ' ', 'T', 'h', 'r', 'e', 'a', 'd', ' ', 'C', 'o', 'd', 'e', ' ', '0', 'x', '1', '1', '!', 0x0 }; /* ERROR! Thread creation failed, Thread Code 0x11! */
		LPFunctions->CloseProcess(__warn, false, "");
	}

#ifdef _DEBUG
	LPLog->AddLog(0, "Metin2 file hash check event thread creation completed!");
#endif
	KARMA_MACRO_2

	return hThread;
}
#endif





















#if 0

__forceinline bool IsMilesModule(const char* c_szFileName)
{
	CHAR __mss32[] = { 'm', 's', 's', '3', '2', '.', 'd', 'l', 'l', 0x0 }; //mss32.dll
	CHAR __mix[] = { '.', 'm', 'i', 'x', 0x0 }; //".mix"
	CHAR __asi[] = { '.', 'a', 's', 'i', 0x0 }; //".asi"
	CHAR __m3d[] = { '.', 'm', '3', 'd', 0x0 }; //".m3d"
	CHAR __flt[] = { '.', 'f', 'l', 't', 0x0 }; //".flt"

	return strstr(c_szFileName, __mss32) || strstr(c_szFileName, __mix) || strstr(c_szFileName, __asi) || strstr(c_szFileName, __m3d) || strstr(c_szFileName, __flt);
}

int CDirFunctions::GetDirFileCount(std::string szDir)
{
	struct dirent * de;

	auto my_Dir = szDir.c_str();

	auto dir = opendir(my_Dir);
	if (!dir)
		return -1; // Probably dir is not exist

	int count = 0;
	while (de = readdir(dir)) {
		if (IsMilesModule(de->d_name)) {
			++count;
		}
	}

	closedir(dir);

	return count;
}

void CDirFunctions::MilesCountCheck()
{
	CHAR __miles[] = { '\\', '\\', 'm', 'i', 'l', 'e', 's', '\\', '\\', 0x0 }; // "\\miles\\"
	auto milessay = GetDirFileCount(ExePath() + __miles);
	if (milessay == -1)
		return;
	
	if (milessay > 10)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_1_FAIL, g_winapiApiTable->GetLastError());
		return;
	}
}


void CDirFunctions::CheckDirectory(std::string Directory)
{
	auto szTargetDirectory = Directory;
	if (Directory.length() == 0)
		Directory = "*";
	else
		Directory = Directory + "\\*";

	WIN32_FIND_DATAA FindData;
	auto Find = g_winapiApiTable->FindFirstFileA(Directory.c_str(), &FindData);
	if (!Find || Find == INVALID_HANDLE_VALUE)
		return;

	int py = 0;
	int m3d = 0;
	int asi = 0;
	int flt = 0;
	int mix = 0;

	// text list 
	CHAR __miles[] = { 'm', 'i', 'l', 'e', 's', 0x0 }; //__miles

	CHAR __logininfo[] = { 'l', 'o', 'g', 'i', 'n', 'i', 'n', 'f', 'o', '.', 'p', 'y', 0x0 }; //"logininfo.py"
	CHAR __mssa3d[] = { 'm', 's', 's', 'a', '3', 'd', '.', 'm', '3', 'd', 0x0 }; //"mssa3d.m3d"
	CHAR __mssds3d[] = { 'm', 's', 's', 'd', 's', '3', 'd', '.', 'm', '3', 'd', 0x0 }; //"mssds3d.m3d"
	CHAR __mssdx7[] = { 'm', 's', 's', 'd', 'x', '7', '.', 'm', '3', 'd', 0x0 }; //"mssdx7.m3d"
	CHAR __msseax[] = { 'm', 's', 's', 'e', 'a', 'x', '.', 'm', '3', 'd', 0x0 }; //"msseax.m3d"
	CHAR __mssrsx[] = { 'm', 's', 's', 'r', 's', 'x', '.', 'm', '3', 'd', 0x0 }; //"mssrsx.m3d"
	CHAR __msssoft[] = { 'm', 's', 's', 's', 'o', 'f', 't', '.', 'm', '3', 'd', 0x0 }; //"msssoft.m3d"
	CHAR __mssmp3[] = { 'm', 's', 's', 'm', 'p', '3', '.', 'a', 's', 'i', 0x0 }; //"mssmp3.asi"
	CHAR __mssvoice[] = { 'm', 's', 's', 'v', 'o', 'i', 'c', 'e', '.', 'a', 's', 'i', 0x0 }; //"mssvoice.asi"
	CHAR __mssdsp[] = { 'm', 's', 's', 'd', 's', 'p', '.', 'f', 'l', 't', 0x0 }; //"mssdsp.flt"

	CHAR __py[] = { '.', 'p', 'y', 0x0 }; //".py"
	CHAR __mix[] = { '.', 'm', 'i', 'x', 0x0 }; //".mix"
	CHAR __asi[] = { '.', 'a', 's', 'i', 0x0 }; //".asi"
	CHAR __m3d[] = { '.', 'm', '3', 'd', 0x0 }; //".m3d"
	CHAR __flt[] = { '.', 'f', 'l', 't', 0x0 }; //".flt"

	if (szTargetDirectory == __miles) {
		while (g_winapiApiTable->FindNextFileA(Find, &FindData) != 0) {
			if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__logininfo) == 0) { py = 1; break; }
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__mssa3d) != 0 && \
					g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__mssds3d) != 0 && \
					g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__mssdx7) != 0 && \
					g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__msseax) != 0 && \
					g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__mssrsx) != 0 && \
					g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__msssoft) != 0) {
					if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__m3d) != std::string::npos) { m3d = 1; break; }
				}
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__mssmp3) != 0 && \
					g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__mssvoice)) {
					if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__asi) != std::string::npos) { asi = 1; break; }
				}
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).compare(__mssdsp)) {
					if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__flt) != std::string::npos) { flt = 1; break; }
				}
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__mix) != std::string::npos) { mix = 1; break; }
			}
		}
	}
	else {
		while (g_winapiApiTable->FindNextFileA(Find, &FindData) != 0) {
			if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__m3d) != std::string::npos) { m3d = 1; break; }
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__asi) != std::string::npos) { asi = 1; break; }
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__flt) != std::string::npos) { flt = 1; break; }
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__mix) != std::string::npos) { mix = 1; break; }
				if (g_nmApp->FunctionsInstance()->szLower(FindData.cFileName).find(__py) != std::string::npos) { py = 1; break; }
			}
		}
	}

	if (!(py == 0 && m3d == 0 && asi == 0 && flt == 0 && mix == 0)) {
		std::string is;
		if (py == 1) is = __py;
		if (m3d == 1) is = __m3d;
		if (asi == 1) is = __asi;
		if (flt == 1) is = __flt;
		if (mix == 1) is = __mix;

		auto szDetectedFile = Directory + is;
		CHAR __Badfiledetected[] = { 'B', 'a', 'd', ' ', 'f', 'i', 'l', 'e', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', ' ', 'i', 'n', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', '.', ' ', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'r', 'e', 'm', 'o', 'v', 'e', ':', ' ', '%', 's', 0x0 }; // Bad file detected in game folder. Please remove: %s
		char tmpBuf[500];
		sprintf(tmpBuf, __Badfiledetected, szDetectedFile.c_str());

		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_2_FAIL, g_winapiApiTable->GetLastError());
	}
}

void CDirFunctions::MainFolderCheck()
{
	CHAR __logininfo1[] = { 'l', 'o', 'g', 'i', 'n', 'i', 'n', 'f', 'o', '.', 'p', 'y', 0x0 }; //"logininfo.py"
	CHAR __logininfo2[] = { 'l', 'o', 'g', 'i', 'n', 'I', 'n', 'f', 'o', '.', 'p', 'y', 0x0 }; //"loginInfo.py"
	CHAR __d3d8thkdef[] = { 'd', '3', 'd', '8', 't', 'h', 'k', '.', 'd', 'e', 'f', 0x0 }; // d3d8thk.def
	CHAR __d3d8thk64def[] = { 'd', '3', 'd', '8', 't', 'h', 'k', '6', '4', '.', 'd', 'e', 'f', 0x0 }; // d3d8thk64.def

	if (IsFileExist(__logininfo1) || IsFileExist(__logininfo2))
	{
		CHAR __Logininfoinjection[] = { 'L', 'o', 'g', 'i', 'n', 'i', 'n', 'f', 'o', ' ', 'i', 'n', 'j', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // Logininfo injection detected.
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_3_FAIL, g_winapiApiTable->GetLastError());
	}

	if (IsFileExist(__d3d8thkdef) || IsFileExist(__d3d8thk64def)) 
	{
		CHAR __d3d8thkdefwarn[] = { 'U', 'n', 'a', 'l', 'l', 'o', 'w', 'e', 'd', ' ', 'f', 'i', 'l', 'e', ' ', 'd', '3', 'd', '8', 't', 'h', 'k', '.', 'd', 'e', 'f', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '!', 0x0 }; // Unallowed file d3d8thk.def detected!
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_4_FAIL, g_winapiApiTable->GetLastError());
	}
}

void CDirFunctions::PackCheck()
{
	CHAR __antiflydmg[] = { '\\', '\\', 'p', 'a', 'c', 'k', '\\', '\\', 'a', 'n', 't', 'i', 'f', 'l', 'y', 'd', 'm', 'g', '.', 'e', 'p', 'k', 0x0 }; // "\\pack\\antiflydmg.epk"
	CHAR __waitdmg[] = { '\\', '\\', 'p', 'a', 'c', 'k', '\\', '\\', 'w', 'a', 'i', 't', 'd', 'm', 'g', '.', 'e', 'p', 'k', 0x0 }; // "\\pack\\waitdmg.epk"
	CHAR __antifly[] = { '\\', '\\', 'p', 'a', 'c', 'k', '\\', '\\', 'a', 'n', 't', 'i', 'f', 'l', 'y', '.', 'e', 'p', 'k', 0x0 }; // "\\pack\\antifly.epk"
	CHAR __ymirpc[] = { 'd', ':', '\\', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', '\\', 'p', 'c', 0x0 }; // "d:\\ymir work\\pc"
	CHAR __ymirpc_2[] = { 'd', ':', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', 'p', 'c', 0x0 }; // "d:\\ymir work\\pc"
	CHAR __ymirpc2[] = { 'd', ':', '\\', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', '\\', 'p', 'c', '2', 0x0 }; // "d:\\ymir work\\pc2"
	CHAR __ymirpc2_2[] = { 'd', ':', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', 'p', 'c', '2', 0x0 }; // "d:\\ymir work\\pc2"
	CHAR __ymirpc3[] = { 'd', ':', '\\', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', '\\', 'p', 'c', '3', 0x0 }; // "d:\\ymir work\\pc3"
	CHAR __ymirpc3_2[] = { 'd', ':', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', 'p', 'c', '3', 0x0 }; // "d:\\ymir work\\pc3"
	CHAR __ymirmonster[] = { 'd', ':', '\\', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', '\\', 'm', 'o', 'n', 's', 't', 'e', 'r', 0x0 }; // d:/ymir work/monster
	CHAR __ymirmonster_2[] = { 'd', ':', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', 'm', 'o', 'n', 's', 't', 'e', 'r', 0x0 }; // d:/ymir work/monster
	CHAR __ymirmonster2[] = { 'd', ':', '\\', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', '\\', 'm', 'o', 'n', 's', 't', 'e', 'r', '2', 0x0 }; // d:/ymir work/monster2
	CHAR __ymirmonster2_2[] = { 'd', ':', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '\\', 'm', 'o', 'n', 's', 't', 'e', 'r', '2', 0x0 }; // d:/ymir work/monster2
	CHAR __ymir[] = { '\\', '\\', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', 0x0 }; // "\\ymir work"
	CHAR __pc[] = { '\\', '\\', 'p', 'c', 0x0 }; // "\\pc"
	CHAR __pc2[] = { '\\', '\\', 'p', 'c', '2', 0x0 }; // "\\pc2"
	CHAR __pc3[] = { '\\', '\\', 'p', 'c', '3', 0x0 }; // "\\pc3"

	CHAR __Antiflydmg_warn[] = { 'A', 'n', 't', 'i', 'f', 'l', 'y', ' ', 'D', 'a', 'm', 'a', 'g', 'e', ' ', 'H', 'a', 'c', 'k', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // Antifly Damage Hack detected.
	CHAR __WaitDamage_warn[] = { 'W', 'a', 'i', 't', ' ', 'D', 'a', 'm', 'a', 'g', 'e', ' ', 'H', 'a', 'c', 'k', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // Wait Damage Hack detected.
	CHAR __Antifly_warn[] = { 'A', 'n', 't', 'i', 'f', 'l', 'y', ' ', 'H', 'a', 'c', 'k', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // Antifly Hack detected.
	CHAR __ymirworkpc_warn[] = { 'D', ':', '/', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '/', 'p', 'c', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // D:/ymir work/pc illegal folder detected.
	CHAR __ymirworkpc2_warn[] = { 'D', ':', '/', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '/', 'p', 'c', '2', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // D:/ymir work/pc2 illegal folder detected.
	CHAR __ymirworkpc3_warn[] = { 'D', ':', '/', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '/', 'p', 'c', '3', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // D:/ymir work/pc3 illegal folder detected.
	CHAR __ymirwork_warn[] = { 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', ' ', 'i', 'n', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // 'ymir work' illegal folder detected in game folder
	CHAR __pc_warn[] = { 'p', 'c', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', ' ', 'i', 'n', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // 'pc' illegal folder detected in game folder
	CHAR __pc2_warn[] = { 'p', 'c', '2', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', ' ', 'i', 'n', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // 'pc2' illegal folder detected in game folder
	CHAR __pc3_warn[] = { 'p', 'c', '3', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', ' ', 'i', 'n', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // 'pc3' illegal folder detected in game folder
	CHAR __ymirworkmonster_warn[] = { 'D', ':', '/', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '/', 'm', 'o', 'n', 's', 't', 'e', 'r', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // D:/ymir work/monster illegal folder detected.
	CHAR __ymirworkmonster2_warn[] = { 'D', ':', '/', 'y', 'm', 'i', 'r', ' ', 'w', 'o', 'r', 'k', '/', 'm', 'o', 'n', 's', 't', 'e', 'r', '2', ' ', 'i', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'f', 'o', 'l', 'd', 'e', 'r', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '.', 0x0 }; // D:/ymir work/monster2 illegal folder detected.

	std::string packyediiks = ExePath() + __antiflydmg;
	if (IsFileExist(packyediiks))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_5_FAIL, g_winapiApiTable->GetLastError());
	}

	std::string packyediiksiki = ExePath() + __waitdmg;
	if (IsFileExist(packyediiksiki))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_6_FAIL, g_winapiApiTable->GetLastError());
	}

	std::string packyediiksuc = ExePath() + __antifly;
	if (IsFileExist(packyediiksuc))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_7_FAIL, g_winapiApiTable->GetLastError());
	}
	if (IsDirExist(__ymirpc) || IsDirExist(__ymirpc_2))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_8_FAIL, g_winapiApiTable->GetLastError());
	}
	if (IsDirExist(__ymirpc2) || IsDirExist(__ymirpc2_2))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_9_FAIL, g_winapiApiTable->GetLastError());
	}
	if (IsDirExist(__ymirpc3) || IsDirExist(__ymirpc3_2))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_10_FAIL, g_winapiApiTable->GetLastError());
	}
	if (IsDirExist(__ymirmonster) || IsDirExist(__ymirmonster_2)) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_11_FAIL, g_winapiApiTable->GetLastError());
	}
	if (IsDirExist(__ymirmonster2) || IsDirExist(__ymirmonster2_2)) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_12_FAIL, g_winapiApiTable->GetLastError());
	}
	std::string exedizin = ExePath() + __ymir;
	if (IsDirExist(exedizin.c_str())) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_13_FAIL, g_winapiApiTable->GetLastError());
	}
	std::string pcdizin = ExePath() + __pc;
	if (IsDirExist(pcdizin.c_str())) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_14_FAIL, g_winapiApiTable->GetLastError());
	}
	std::string pc2dizin = ExePath() + __pc2;
	if (IsDirExist(pc2dizin.c_str())) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_15_FAIL, g_winapiApiTable->GetLastError());
	}
	std::string pc3dizin = ExePath() + __pc3;
	if (IsDirExist(pc3dizin.c_str())) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_16_FAIL, g_winapiApiTable->GetLastError());
	}
}

int getdir(const std::string& dir, std::vector<std::string>& files)
{
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL) {
		return errno;
	}

	while ((dirp = readdir(dp)) != NULL) {
		files.push_back(std::string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

bool CDirFunctions::AntiMssExploit()
{
	auto dir = std::string(".");
	std::vector<std::string> files = std::vector<std::string>();

	CHAR __dot[] = { '.', 0x0 }; //"."
	CHAR __py[] = { 'p', 'y', 0x0 }; //.py"
	CHAR __mix[] = { 'm', 'i', 'x', 0x0 }; //"mix"
	CHAR __asi[] = { 'a', 's', 'i', 0x0 }; //"asi"
	CHAR __m3d[] = { 'm', '3', 'd', 0x0 }; //"m3d"
	CHAR __flt[] = { 'f', 'l', 't', 0x0 }; //"flt"

	CHAR __Mixwarn[] = { 'M', 'i', 'x', ' ', 'i', 'n', 'j', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '\n', '\n', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'c', 'l', 'e', 'a', 'r', ' ', 'y', 'o', 'u', 'r', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // Mix injection detected Please clear your game folder
	CHAR __Fltwarn[] = { 'F', 'l', 't', ' ', 'i', 'n', 'j', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '\n', '\n', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'c', 'l', 'e', 'a', 'r', ' ', 'y', 'o', 'u', 'r', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // Flt injection detected Please clear your game folder
	CHAR __Asiwarn[] = { 'A', 's', 'i', ' ', 'i', 'n', 'j', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '\n', '\n', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'c', 'l', 'e', 'a', 'r', ' ', 'y', 'o', 'u', 'r', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // Asi injection detected Please clear your game folder
	CHAR __M3dwarn[] = { 'M', '3', 'd', ' ', 'i', 'n', 'j', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '\n', '\n', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'c', 'l', 'e', 'a', 'r', ' ', 'y', 'o', 'u', 'r', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // M3d injection detected Please clear your game folder
	CHAR __pywarn[] = { '.', 'p', 'y', ' ', 'f', 'i', 'l', 'e', ' ', 'd', 'e', 't', 'e', 'c', 't', 'e', 'd', '\n', '\n', 'P', 'l', 'e', 'a', 's', 'e', ' ', 'c', 'l', 'e', 'a', 'r', ' ', 'y', 'o', 'u', 'r', ' ', 'g', 'a', 'm', 'e', ' ', 'f', 'o', 'l', 'd', 'e', 'r', 0x0 }; // .py file detected Please clear your game folder

	getdir(dir, files);
	for (unsigned int i = 0; i < files.size(); i++) {
		std::string a = files[i];
		if (a.substr(a.find_last_of(__dot) + 1) == __mix) 
		{
			const char* b = a.c_str();

			g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_17_FAIL, g_winapiApiTable->GetLastError());
		}
		else if (a.substr(a.find_last_of(__dot) + 1) == __flt)
		{
			const char* b = a.c_str();

			g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_18_FAIL, g_winapiApiTable->GetLastError());
		}
		else if (a.substr(a.find_last_of(__dot) + 1) == __m3d) 
		{
			const char* b = a.c_str();

			g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_19_FAIL, g_winapiApiTable->GetLastError());
		}
		else if (a.substr(a.find_last_of(__dot) + 1) == __asi)
		{
			const char* b = a.c_str();

			g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_20_FAIL, g_winapiApiTable->GetLastError());
		}
	}

	return false;
}




CHAR __miles[] = { 'm', 'i', 'l', 'e', 's', 0x0 }; //miles
CHAR __mssa3d[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'a', '3', 'd', '.', 'm', '3', 'd', 0x0 }; //"mssa3d.m3d"
CHAR __mssds3d[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'd', 's', '3', 'd', '.', 'm', '3', 'd', 0x0 }; //"mssds3d.m3d"
CHAR __mssdx7[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'd', 'x', '7', '.', 'm', '3', 'd', 0x0 }; //"mssdx7.m3d"
CHAR __msseax[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'e', 'a', 'x', '.', 'm', '3', 'd', 0x0 }; //"msseax.m3d"
CHAR __mssrsx[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'r', 's', 'x', '.', 'm', '3', 'd', 0x0 }; //"mssrsx.m3d"
CHAR __msssoft[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 's', 'o', 'f', 't', '.', 'm', '3', 'd', 0x0 }; //"msssoft.m3d"
CHAR __mssmp3[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'm', 'p', '3', '.', 'a', 's', 'i', 0x0 }; //"mssmp3.asi"
CHAR __mssvoice[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'v', 'o', 'i', 'c', 'e', '.', 'a', 's', 'i', 0x0 }; //"mssvoice.asi"
CHAR __mssdsp[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', 'd', 's', 'p', '.', 'f', 'l', 't', 0x0 }; //"mssdsp.flt"
CHAR __miles_mss32[] = { 'm', 'i', 'l', 'e', 's', '/', '/', 'm', 's', 's', '3', '2', '.', 'd', 'l', 'l', 0x0 }; // miles//mss32.dll
CHAR __mss32[] = { 'm', 's', 's', '3', '2', '.', 'd', 'l', 'l', 0x0 }; //mss32.dll
CHAR __devildll[] = { 'd', 'e', 'v', 'i', 'l', '.', 'd', 'l', 'l', 0x0 }; // devil.dll

if (g_nmApp->DataInstance()->GetAppType() == METIN2_GAME)
{
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__miles_mss32, 0x6c5812e3);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mssa3d, 0x6c0abc4c);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mssds3d, 0xa134de04);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mssdsp, 0xc88f11bb);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mssdx7, 0xe173609);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__msseax, 0xbe7c43f7);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mssmp3, 0x48b4e4d5);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mssrsx, 0x20d6c7b7);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__msssoft, 0xff5f14f8);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mssvoice, 0x53ebe0e8);
	g_nmApp->DirFunctionsInstance()->CheckFileCrc(__mss32, 0x6c5812e3);

	auto dwDevilCrc = g_nmApp->DirFunctionsInstance()->GetFileCrc(__devildll);
	if (dwDevilCrc != 0x90f088b8 && dwDevilCrc != 0x68f0df33)
		g_nmApp->DirFunctionsInstance()->CheckFileCrc(__devildll, 0); // basic wrapper for close & warn

	g_nmApp->DirFunctionsInstance()->MilesCountCheck();

	g_nmApp->DirFunctionsInstance()->CheckDirectory("");

	g_nmApp->DirFunctionsInstance()->CheckDirectory(__miles);

	g_nmApp->DirFunctionsInstance()->MainFolderCheck();

	g_nmApp->DirFunctionsInstance()->PackCheck();

	g_nmApp->DirFunctionsInstance()->AntiMssExploit();
}
#endif