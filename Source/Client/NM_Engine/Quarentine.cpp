#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Quarentine.h"
#include "Defines.h"
#include "../../Common/SocketPackets.h"

CQuarentine::CQuarentine()
{
	InitializeVeriables();
}
CQuarentine::~CQuarentine()
{
//	InitializeVeriables();
}

void CQuarentine::InitializeVeriables()
{
	m_vBadTestScanList.clear();
	m_vBadBaseMemList.clear();
	m_vBadIconHashList.clear();
	m_vBadFileBasicInformationList.clear();
	m_vBadFileInformationList.clear();
	m_vBadFileCertificateList.clear();
	m_vBadFilePESectionList.clear();
	m_vBadFilePEEATList.clear();
	m_vBadFilePEIATList.clear();
	m_vBadFilePERegionHashList.clear();
}

// ------------------------------------------------------------------------

void CQuarentine::CheckTestScan(const std::string & szWindowName, const std::string & szClassName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadTestScanList.empty())
	{
//		DEBUG_LOG(LL_SYS, "Test scan list is null!");
		return;
	}

//	DEBUG_LOG(LL_SYS, "Test scan process has been started! Size: %u", m_vBadTestScanList.size());

	static auto iViolationIndex = static_cast<int>(SCAN_TEST);
	for (const auto & pCurrScanCtx : m_vBadTestScanList)
	{
		if (IS_VALID_SMART_PTR(pCurrScanCtx))
		{
//			DEBUG_LOG(LL_SYS, "Current test scan list object: '%s'-'%s' List size: %u", pCurrScanCtx->szWindowName, pCurrScanCtx->szClassName, m_vBadTestScanList.size());

			if ((strlen(pCurrScanCtx->szWindowName) && !strcmp(pCurrScanCtx->szWindowName, szWindowName.c_str())) ||
				(strlen(pCurrScanCtx->szClassName) && !strcmp(pCurrScanCtx->szClassName, szClassName.c_str())))
			{
				DEBUG_LOG(LL_SYS, "Blacklist'd object found! Data: '%s'-'%s'", pCurrScanCtx->szWindowName, pCurrScanCtx->szClassName);

				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrScanCtx->iIndex) + 
					xorstr(" Window name: ").crypt_get() + szWindowName + xorstr(" Class name: ").crypt_get() + szClassName;

				// Send violation msg
				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_TEST_SCAN, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadTestScanList.erase(std::remove(m_vBadTestScanList.begin(), m_vBadTestScanList.end(), pCurrScanCtx), m_vBadTestScanList.end());
			}
		}
	}
}
void CQuarentine::AppendTestScanData(int iIndex, const std::string & szWindowName, const std::string & szClassName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pTestScanData = std::make_shared<STestScanData>();
	if (!IS_VALID_SMART_PTR(pTestScanData))
		return;

	pTestScanData->iIndex = iIndex;
	strcpy(pTestScanData->szWindowName, szWindowName.c_str());
	strcpy(pTestScanData->szClassName, szClassName.c_str());

	m_vBadTestScanList.push_back(pTestScanData);
}

// ---------------------------------

void CQuarentine::CheckProcessBaseMem(const std::string & szProcessName, PVOID64 lpBase, LPBYTE lpMemCopy)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadBaseMemList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_PROCESS_BASE);
	for (const auto & pCurrBadMem : m_vBadBaseMemList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadMem))
		{
			if (pCurrBadMem->lpBaseAddress == lpBase && !memcmp(pCurrBadMem->pMem, lpMemCopy, 12))
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadMem->iIndex) + xorstr(" Process name: ").crypt_get() + szProcessName;

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_PROCESS_BASE_SCAN, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadBaseMemList.erase(std::remove(m_vBadBaseMemList.begin(), m_vBadBaseMemList.end(), pCurrBadMem), m_vBadBaseMemList.end());
			}
		}
	}
}

void CQuarentine::AppendProcessBaseMemData(int iIndex, LPVOID lpBase, LPBYTE lpMemCopy)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pMemData = std::make_shared<SProcessBaseScanData>();
	if (!IS_VALID_SMART_PTR(pMemData))
		return;

	pMemData->iIndex = iIndex;
	pMemData->lpBaseAddress = lpBase;
	memcpy(pMemData->pMem, lpMemCopy, 12);

	m_vBadBaseMemList.push_back(pMemData);
}

// ---------------------------------

void CQuarentine::CheckProcessIconHash(const std::string & szProcessName, const std::string & szIconHash)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadIconHashList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_PROCESS_ICON_HASH);
	for (const auto & pCurrBadIconData : m_vBadIconHashList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadIconData))
		{
			if (!strcmp(pCurrBadIconData->szIconHash, szIconHash.c_str()))
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadIconData->iIndex) + xorstr(" Process name: ").crypt_get() + szProcessName;

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_PROCESS_ICON_HASH_SCAN, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadIconHashList.erase(std::remove(m_vBadIconHashList.begin(), m_vBadIconHashList.end(), pCurrBadIconData), m_vBadIconHashList.end());
			}
		}
	}
}

void CQuarentine::AppendProcessIconHashData(int iIndex, const std::string & szIconHash)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pIconHashData = std::make_shared<SProcessIconHashScanData>();
	if (!IS_VALID_SMART_PTR(pIconHashData))
		return;

	pIconHashData->iIndex = iIndex;
	strcpy(pIconHashData->szIconHash, szIconHash.c_str());

	m_vBadIconHashList.push_back(pIconHashData);
}

// ---------------------------------

void CQuarentine::CheckFileBasicInformations(const std::string & szFileName, DWORD dwFileLen, float fEntropy, const std::string & szShaHash, BOOL bIsSignedFile, BOOL bIsSignAnalysed)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadFileBasicInformationList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_BASIC_FILE_INFORMATIONS);
	for (const auto & pCurrBadFileData : m_vBadFileBasicInformationList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadFileData))
		{
			if (!strcmp(pCurrBadFileData->szShaHash, szShaHash.c_str()))
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadFileData->iIndex) + xorstr(" File name: ").crypt_get() + \
					szFileName + xorstr(" File size: ").crypt_get() + std::to_string(dwFileLen) + xorstr(" Entropy: ").crypt_get() + std::to_string(fEntropy) + xorstr(" Hash: ").crypt_get() + szShaHash + \
					xorstr(" Sign result: ").crypt_get() + std::to_string(bIsSignedFile) + xorstr(" Sign analyse result: ").crypt_get() + std::to_string(bIsSignAnalysed);

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_FILE_BASIC_INFORMATION_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadFileBasicInformationList.erase(std::remove(m_vBadFileBasicInformationList.begin(), m_vBadFileBasicInformationList.end(), pCurrBadFileData), m_vBadFileBasicInformationList.end());
			}
		}
	}
}

void CQuarentine::AppendFileBasicInformationsData(int iIndex, const std::string & szShaHash)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pFileInformationData = std::make_shared<SFileBasicInformationScanData>();
	if (!IS_VALID_SMART_PTR(pFileInformationData))
		return;

	pFileInformationData->iIndex = iIndex;
	strcpy(pFileInformationData->szShaHash, szShaHash.c_str());

	m_vBadFileBasicInformationList.push_back(pFileInformationData);
}

// ---------------------------------

void CQuarentine::CheckFileInformations(const std::string & szFileName, DWORD dwFileIndexLow, DWORD dwFileIndexHigh, DWORD dwVolumeSerialNumber)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadFileInformationList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_FILE_INFORMATIONS);
	for (const auto & pCurrBadFileData : m_vBadFileInformationList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadFileData))
		{
			if (pCurrBadFileData->dwFileIndexLow == dwFileIndexLow && pCurrBadFileData->dwFileIndexHigh == dwFileIndexHigh && pCurrBadFileData->dwVolumeSerialNumber == dwVolumeSerialNumber)
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadFileData->iIndex) + xorstr(" File name: ").crypt_get() + \
					szFileName + xorstr(" File low idx: ").crypt_get() + std::to_string(dwFileIndexLow) + xorstr(" File high idx: ").crypt_get() + std::to_string(dwFileIndexHigh) + \
					xorstr(" File volume serial: ").crypt_get() + std::to_string(dwVolumeSerialNumber);

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_FILE_INFORMATION_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadFileInformationList.erase(std::remove(m_vBadFileInformationList.begin(), m_vBadFileInformationList.end(), pCurrBadFileData), m_vBadFileInformationList.end());
			}
		}
	}
}

void CQuarentine::AppendFileInformationsData(int iIndex, DWORD dwFileIndexLow, DWORD dwFileIndexHigh, DWORD dwVolumeSerialNumber)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pFileInformationData = std::make_shared<SFileInformationScanData>();
	if (!IS_VALID_SMART_PTR(pFileInformationData))
		return;

	pFileInformationData->iIndex				= iIndex;
	pFileInformationData->dwFileIndexLow		= dwFileIndexLow;
	pFileInformationData->dwFileIndexHigh		= dwFileIndexHigh;
	pFileInformationData->dwVolumeSerialNumber	= dwVolumeSerialNumber;

	m_vBadFileInformationList.push_back(pFileInformationData);
}

// ---------------------------------

void CQuarentine::CheckSignCertInformations(const std::string & szFileName, const std::string & pCertHash, LPSTR szSerialNumber, LPSTR szIssuer, LPSTR szSubject, LPSTR szFriendlyName, LPSTR szAuthority, LPSTR szPublisher)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadFileCertificateList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_FILE_CERTIFICATE);
	for (const auto & pCurrBadCertData : m_vBadFileCertificateList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadCertData))
		{
			if (!strcmp(pCurrBadCertData->szHash, pCertHash.c_str()) || !strcmp(pCurrBadCertData->szPublisher, szPublisher))
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadCertData->iIndex) + xorstr(" File name: ").crypt_get() + \
					szFileName + xorstr(" Cert hash: ").crypt_get() + pCertHash + xorstr(" Serial: ").crypt_get() + std::string(szSerialNumber) + xorstr(" Issuer: ").crypt_get() + std::string(szIssuer) + \
					xorstr(" Subject: ").crypt_get() + std::string(szSubject) + xorstr(" Friendly: ").crypt_get() + std::string(szFriendlyName) + xorstr(" Authority: ").crypt_get() + std::string(szAuthority) + \
					xorstr(" Publisher: ").crypt_get() + std::string(szPublisher);

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_FILE_CERTIFICATE_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadFileCertificateList.erase(std::remove(m_vBadFileCertificateList.begin(), m_vBadFileCertificateList.end(), pCurrBadCertData), m_vBadFileCertificateList.end());
			}
		}
	}
}

void CQuarentine::AppendFileCertificateData(int iIndex, const std::string & szHash, const std::string & szPublisher)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pFileInformationData = std::make_shared<SFileCertificateScanData>();
	if (!IS_VALID_SMART_PTR(pFileInformationData))
		return;

	pFileInformationData->iIndex = iIndex;
	strcpy(pFileInformationData->szHash, szHash.c_str());
	strcpy(pFileInformationData->szPublisher, szPublisher.c_str());

	m_vBadFileCertificateList.push_back(pFileInformationData);
}

// ---------------------------------

void CQuarentine::CheckMappedFilePEInformations(const std::string & szFileName, const std::string & szSectionName, DWORD dwSize, DWORD dwChecksum, DWORD dwCharacteristics, float fEntropy)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadFilePESectionList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_PE);
	for (const auto & pCurrBadSectionData : m_vBadFilePESectionList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadSectionData))
		{
			if ( (pCurrBadSectionData->dwChecksum == dwChecksum && pCurrBadSectionData->dwSize == dwSize) || fEntropy > 7.5f )
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadSectionData->iIndex) + xorstr(" File name: ").crypt_get() + \
					szFileName + xorstr(" Section name: ").crypt_get() + szSectionName + xorstr(" Characteristics: ").crypt_get() + std::to_string(dwCharacteristics) + xorstr(" Entropy: ").crypt_get() + std::to_string(fEntropy);

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_PE_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadFilePESectionList.erase(std::remove(m_vBadFilePESectionList.begin(), m_vBadFilePESectionList.end(), pCurrBadSectionData), m_vBadFilePESectionList.end());
			}
		}
	}
}

void CQuarentine::AppendMappedFilePEData(int iIndex, DWORD dwSize, DWORD dwChecksum)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pPEInformationData = std::make_shared<SMappedFilePeSectionData>();
	if (!IS_VALID_SMART_PTR(pPEInformationData))
		return;

	pPEInformationData->iIndex = iIndex;
	pPEInformationData->dwSize = dwSize;
	pPEInformationData->dwChecksum = dwChecksum;

	m_vBadFilePESectionList.push_back(pPEInformationData);
}

// ---------------------------------

void CQuarentine::CheckMappedFileEAT(const std::string & szFileName, DWORD dwExportDirectory, DWORD dwImageExportDataSize, DWORD dwEATChecksum)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadFilePESectionList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_EAT);
	for (const auto & pCurrBadEatData : m_vBadFilePEEATList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadEatData))
		{
			if (pCurrBadEatData->dwEATChecksum == dwExportDirectory && pCurrBadEatData->dwEATChecksum == dwEATChecksum && pCurrBadEatData->dwImageExportDataSize == dwImageExportDataSize)
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + 
					std::to_string(pCurrBadEatData->iIndex) + xorstr(" File name: ").crypt_get() + szFileName;

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_EAT_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadFilePEEATList.erase(std::remove(m_vBadFilePEEATList.begin(), m_vBadFilePEEATList.end(), pCurrBadEatData), m_vBadFilePEEATList.end());
			}
		}
	}
}

void CQuarentine::AppendMappedFileEATData(int iIndex, DWORD dwExportDirectory, DWORD dwImageExportDataSize, DWORD dwEATChecksum)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pPEInformationData = std::make_shared<SMappedFilePeEatData>();
	if (!IS_VALID_SMART_PTR(pPEInformationData))
		return;

	pPEInformationData->iIndex					= iIndex;
	pPEInformationData->dwExportDirectory		= dwExportDirectory;
	pPEInformationData->dwImageExportDataSize	= dwImageExportDataSize;
	pPEInformationData->dwEATChecksum			= dwEATChecksum;

	m_vBadFilePEEATList.push_back(pPEInformationData);
}

// ---------------------------------

void CQuarentine::CheckMappedFileIAT(const std::string & szFileName, DWORD dwImportDirectory, DWORD dwImageImportDataSize, DWORD dwIATChecksum)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadFilePESectionList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_IAT);
	for (const auto & pCurrBadIatData : m_vBadFilePEIATList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadIatData))
		{
			if (pCurrBadIatData->dwIATChecksum == dwIATChecksum && pCurrBadIatData->dwImageImportDataSize == dwImageImportDataSize && pCurrBadIatData->dwImportDirectory == dwImportDirectory)
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadIatData->iIndex) + xorstr(" File name: ").crypt_get() + szFileName;

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_IAT_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadFilePEIATList.erase(std::remove(m_vBadFilePEIATList.begin(), m_vBadFilePEIATList.end(), pCurrBadIatData), m_vBadFilePEIATList.end());
			}
		}
	}
}

void CQuarentine::AppendMappedFileIATData(int iIndex, DWORD dwImportDirectory, DWORD dwImageImportDataSize, DWORD dwIATChecksum)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pPEInformationData = std::make_shared<SMappedFilePeIatData>();
	if (!IS_VALID_SMART_PTR(pPEInformationData))
		return;

	pPEInformationData->iIndex = iIndex;
	pPEInformationData->dwImportDirectory = dwImportDirectory;
	pPEInformationData->dwImageImportDataSize = dwImageImportDataSize;
	pPEInformationData->dwIATChecksum = dwIATChecksum;

	m_vBadFilePEIATList.push_back(pPEInformationData);
}

// ---------------------------------

void CQuarentine::CheckMappedFileRegionHash(const std::string & szFileName, const std::string & szMappedFileName, DWORD dwSize, const std::string & szSectionHash, DWORD dwCharacteristics)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	if (m_vBadFilePERegionHashList.empty())
		return;

	static auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_REGION_HASH);
	for (const auto & pCurrBadHashData : m_vBadFilePERegionHashList)
	{
		if (IS_VALID_SMART_PTR(pCurrBadHashData))
		{
			if (!strcmp(pCurrBadHashData->szHash, szSectionHash.c_str()))
			{
				std::string szMessage =
					xorstr("Scan ID: ").crypt_get() + std::to_string(iViolationIndex) + xorstr(" Violation index: ").crypt_get() + std::to_string(pCurrBadHashData->iIndex) + xorstr(" File name: ").crypt_get() + \
					szFileName + xorstr(" Mapped name: ").crypt_get() + szMappedFileName + xorstr(" Size: ").crypt_get() + std::to_string(dwSize) + xorstr(" Characteristics: ").crypt_get() + std::to_string(dwCharacteristics);

				g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_REGION_HASH_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

				// Delete already processed object from quarentine db
				m_vBadFilePERegionHashList.erase(std::remove(m_vBadFilePERegionHashList.begin(), m_vBadFilePERegionHashList.end(), pCurrBadHashData), m_vBadFilePERegionHashList.end());
			}
		}
	}
}

void CQuarentine::AppendMappedFileRegionHashData(int iIndex, const std::string & szSectionHash)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pPEInformationData = std::make_shared<SMappedFilePeRegionHashData>();
	if (!IS_VALID_SMART_PTR(pPEInformationData))
		return;

	pPEInformationData->iIndex	= iIndex;
	strcpy(pPEInformationData->szHash, szSectionHash.c_str());

	m_vBadFilePERegionHashList.push_back(pPEInformationData);
}

