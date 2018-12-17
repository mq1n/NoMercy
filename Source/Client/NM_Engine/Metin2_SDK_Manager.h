#pragma once
#include <xorstr.hpp>

#if 0
typedef struct _M2_FILE_INFORMATIONS
{
	char szFileName[MAX_PATH];
	DWORD dwCRC32;
} SM2FileInformations, *PM2FileInformations;

typedef struct _M2_FILE_DATA
{
	int iServerIndex;
	int iFileCount;
	std::vector < std::shared_ptr <SM2FileInformations> > vFileInformations;
} SM2Fileata, *PM2FileData;


auto pFileData = std::make_shared<SM2FileData>();

pFileData->iServerIndex = X_METIN2;
pFileData->iFileCount = 0;

auto pFile = std::make_shared<SM2FileInformations>();
pFile->szFileName = x.msa
pFile->dwCRC32 = 1234;
pFileData->iFileCount += 1;
pFileData->FileInformations.push_back(pFile);

auto pFile2 = std::make_shared<SM2FileInformations>();
pFile2->szFileName = y.msa
pFile2->dwCRC32 = 4321;
pFileData->iFileCount += 1;
pFileData->FileInformations.push_back(pFile2);

// Get serialize file-hash combined information & parse clientside, push to struct


static struct stCheckFileList {
	const char*			c_szFileName;
	DWORD				dwCRC32;
} st_FileHashList[] = {
	//	 filename										     crc32

	{ "d:\\ymir work\\pc\\warrior\\horse\\wait.msa",		0xfaa65a11 },
	{ "d:\\ymir work\\pc\\warrior\\horse\\damage.msa",		0x8c4b695 },
	{ "d:\\ymir work\\pc\\warrior\\horse\\wait_1.msa",		0xa6daf416 },
	{ "d:\\ymir work\\pc\\warrior\\horse\\wait_2.msa",		0x514d520e },

	{ "metin2_map_a1\\000000\\attr.atr",							0x60699649 },
	{ "metin2_map_a1\\001001\\attr.atr",							0x60699649 },
	{ "metin2_map_a3\\000000\\attr.atr",							0x60699649 },
	{ "metin2_map_a3\\001001\\attr.atr",							0x60699649 },
	{ "metin2_map_b1\\000000\\attr.atr",							0x60699649 },

	{ "XXX", 0 },
};
#endif

typedef struct _M2_SDK_FUNC_DATA
{
	LPCVOID c_pFuncAddr;
	int iCode;
} SM2SDKFuncData, *PM2SDKFuncData;

class CMetin2SDKMgr
{
	typedef bool		(__cdecl* lpSendWrapper)		(const char* c_pData, int iLength);
	typedef bool		(__cdecl* lpSendSequenceFunc)	();
	typedef DWORD		(__cdecl* lpGetVID)				();
	typedef std::string	(__cdecl* lpGetPhase)			();
	typedef std::string	(__cdecl* lpGetPlayerName)		();
	typedef bool		(__cdecl* lpIsMappedFileExist)	(const char* c_szFileName);
	typedef DWORD		(__cdecl* lpGetMappedFileHash)	(const char* c_szFileName);

	public:
		CMetin2SDKMgr();
		~CMetin2SDKMgr() = default;

		void OnGameTick();
		void OnClientMessage(int Code, const void* lpMessage);

		// TODO
		void CheckMappedFiles();
		void SetupHeartbeat();

		bool VerifyFunctionModules();

	public:
		auto Send(const char* c_pData, int iLength)			{ return m_pSend					? m_pSend(c_pData, iLength)				: false;							};
		auto SendSequence()									{ return m_pSendSequence			? m_pSendSequence()						: false;							};
		auto GetVID()										{ return m_pGetVID					? m_pGetVID()							: (DWORD)-1;						};
		auto GetPhase()										{ return m_pGetPhase				? m_pGetPhase()							: xorstr("Unknown").crypt_get();	};
		auto GetPlayerName()								{ return m_pGetPlayerName			? m_pGetPlayerName()					: xorstr("Unknown").crypt_get();	};
		auto IsMappedFileExist(const char* c_szFileName)	{ return m_pGetMappedFileIsExist	? m_pGetMappedFileIsExist(c_szFileName)	: false;							};
		auto GetMappedFileHash(const char* c_szFileName)	{ return m_pGetMappedFileHash		? m_pGetMappedFileHash(c_szFileName)	: (DWORD)-1;						};

	protected:
		void DumpMappedFileHashes();
		bool FunctionIsInGameArea(DWORD dwAddress);

	private:
		bool					m_bInitialized;
		bool					m_bEterPackCheckEnabled;

		lpSendWrapper			m_pSend;
		lpSendSequenceFunc		m_pSendSequence;
		lpGetVID				m_pGetVID;
		lpGetPhase				m_pGetPhase;
		lpGetPlayerName			m_pGetPlayerName;
		lpIsMappedFileExist		m_pGetMappedFileIsExist;
		lpGetMappedFileHash		m_pGetMappedFileHash;

		std::vector <std::shared_ptr <SM2SDKFuncData> >	m_vFunctions;
};

