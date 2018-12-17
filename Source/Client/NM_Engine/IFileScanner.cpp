#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "PEHelper.h"
#include "MappedFile.h"
#include "ProcessFunctions.h"
#include "../../Common/SocketPackets.h"

#include <ImageHlp.h>
#include <filesystem>

inline bool IsConconatsChar(char ch)
{
	switch (toupper(ch))
	{
		case 'A':
		case 'E':
		case 'I':
		case 'O':
		case 'U':
			return false;
		default:
			return true;
	}
	return true;
} 

inline int GetConsonatsCount(const std::string & szInput)
{
	std::string a;
	std::string b;

	auto iCounter = 0;
	for (auto i = szInput.length() - 1; i >= 0; --i)
	{
		if (IsConconatsChar(szInput[i]))
		{
			iCounter++;
		}
	}
	return iCounter;
}


bool CheckFileRegionHashes(const std::string & szFileName)
{
	std::ifstream in(szFileName, std::ifstream::in | std::ifstream::binary);
	if (!in) 
		return false;

	in.seekg(0, std::ifstream::end);
	auto length = (size_t)in.tellg();
	in.seekg(0, std::ifstream::beg);

	std::vector<uint8_t> data(length);
	in.read(reinterpret_cast<char*>(&data[0]), length);
	in.close();

	auto pDos = reinterpret_cast<IMAGE_DOS_HEADER*>(&data[0]);
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE) 
		return false;

	auto pNt = reinterpret_cast<IMAGE_NT_HEADERS*>(&data[pDos->e_lfanew]);
	if (pNt->Signature != IMAGE_NT_SIGNATURE) 
		return false;

	std::vector<IMAGE_SECTION_HEADER*> sections;
	sections.reserve(pNt->FileHeader.NumberOfSections);

	auto pSection = reinterpret_cast<IMAGE_SECTION_HEADER*>(&data[pDos->e_lfanew + sizeof(IMAGE_FILE_HEADER) + sizeof(uint32_t) + pNt->FileHeader.SizeOfOptionalHeader]);
	for (uint16_t i = 0; i < pNt->FileHeader.NumberOfSections; ++i, ++pSection)
	{
		if ((pSection->Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_DISCARDABLE)) == 0 && pSection->NumberOfRelocations == 0)
		{
			sections.push_back(pSection);
		}
	}

	uint32_t BytesPerBlock = 1024 * 512;
	for (auto iter : sections)
	{
		uint32_t offset = 0;
		while (offset < iter->SizeOfRawData)
		{
			uint32_t len = BytesPerBlock;
			if (offset + len > iter->SizeOfRawData) 
				len = iter->SizeOfRawData - offset;

			char szMappedFileName[2048] = { 0 };
			g_winapiApiTable->GetMappedFileNameA(NtCurrentProcess, &data[iter->PointerToRawData + offset], szMappedFileName, 2048);

			auto szCurrSectionHash = g_nmApp->DirFunctionsInstance()->GetSHA256(&data[iter->PointerToRawData + offset], len);

			DEBUG_LOG(LL_SYS, "Current region base: %p Size: %u Checksum: %s Characteristics: %p",
				iter->PointerToRawData + offset, len, szCurrSectionHash.c_str(), iter->Characteristics);

			g_nmApp->QuarentineInstance()->CheckMappedFileRegionHash(szFileName, szMappedFileName, len, szCurrSectionHash, iter->Characteristics);

			offset += len;
		}
	}

	data.clear();
	return true;
}

bool CheckFilePEInformations(const std::string & szFileName)
{
	DEBUG_LOG(LL_SYS, "PE information scanner has been started! Target: %s", szFileName.c_str());

	auto bRet = false;

	auto pMappedFile = std::make_unique<CMappedFile>();
	if (!IS_VALID_SMART_PTR(pMappedFile))
	{
		return false;
	}

	LPVOID pMappedFileData;
	if (!pMappedFile->Create(szFileName.c_str(), &pMappedFileData, 0, 0))
	{
		DEBUG_LOG(LL_ERR, "File can NOT mapped! Error: %u", g_winapiApiTable->GetLastError());
		
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}


	auto pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(pMappedFileData);
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "Target file's DOS header is NOT valid!");

		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	auto pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<LONG_PTR>(pMappedFileData) + pDosHeader->e_lfanew);
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		DEBUG_LOG(LL_ERR, "Target file's NT header is NOT valid!");
		
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	// Parse sections for scan
	auto pImageSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
	if (!pImageSectionHeader)
	{
		DEBUG_LOG(LL_ERR, "Target file's first section is NOT found!");

		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	DEBUG_LOG(LL_SYS, "Target file: %s succesfully parsed! Base: %p Size: %p", szFileName.c_str(), pMappedFileData, pMappedFile->Size());

	auto iVMPCounter = 0;
	auto iEnigmaCounter = 0;
	auto iMpressCounter = 0; 
	auto iUpxCounter = 0;
	auto iThemidaCounter = 0;
	auto iMoleboxCounter = 0;
	auto iYodaCounter = 0;
	auto iShieldenCounter = 0;
	for (WORD i = 0; i < pNtHeaders->FileHeader.NumberOfSections; ++i)
	{
		auto dwBase = (DWORD)pMappedFileData + pImageSectionHeader[i].VirtualAddress;
		auto dwSize = pImageSectionHeader[i].SizeOfRawData;
		auto dwOldProtect = 0UL;
		if (!g_winapiApiTable->VirtualProtect((LPVOID)dwBase, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			continue;

		auto szName = (char*)pImageSectionHeader[i].Name;
		auto dwChecksum = CPEFunctions::CalculateChecksum(dwBase, dwSize);
		auto dwCharacteristics = pImageSectionHeader[i].Characteristics;
		auto fEntropy = g_nmApp->FunctionsInstance()->GetEntropy((BYTE*)dwBase, dwSize);

		DEBUG_LOG(LL_SYS, "Section informations; Name: %s Base: %p Size: %p Checksum: %p", szName, dwBase, dwSize, dwChecksum);

		auto __szSectionName = std::string(szName);
		std::reverse(__szSectionName.begin(), __szSectionName.end());

		if (__szSectionName.at(0) == '0' || __szSectionName.at(0) == '1')
			iVMPCounter++;

		if (!strlen(szName))
			iEnigmaCounter++;

		if (strstr(szName, "MPRESS"))
			iMpressCounter++;

		if (strstr(szName, "UPX"))
			iUpxCounter++;

		if (!strcmp(szName, ".yP"))
			iYodaCounter++;

		if (!strcmp(szName, ".sedata"))
			iShieldenCounter++;

		if (strlen(szName) == 1 && std::to_string(i) == __szSectionName)
			iMoleboxCounter++;

		if (strlen(szName) == 8)
		{
			auto iConsonatsCount = GetConsonatsCount(szName);
			if (iConsonatsCount >= 5)
				iThemidaCounter++;
		}

		g_nmApp->QuarentineInstance()->CheckMappedFilePEInformations(szFileName, szName, dwSize, dwChecksum, dwCharacteristics, fEntropy);

		g_winapiApiTable->VirtualProtect((LPVOID)dwBase, dwSize, dwOldProtect, &dwOldProtect);
	}

	if (iVMPCounter == 2)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: VMProtect";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	if (iEnigmaCounter >= 3)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: Enigma";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	if (iMpressCounter)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: Mpress";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	if (iUpxCounter)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: UPX";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	if (iYodaCounter)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: Yoda";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	if (iShieldenCounter)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: Shielden";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	if (iMoleboxCounter)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: Molebox";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	if (iThemidaCounter)
	{
		auto iViolationIndex = static_cast<int>(SCAN_MAPPED_FILE_SECTION_NAME);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName + " Type: Themida";
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);
	}

	// Export Directory parsing
	auto dwExportDirectory = CPEFunctions::Rva2Offset((DWORD)pMappedFileData, pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if (!dwExportDirectory)
	{
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	auto dwImageExportDataSize = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	if (!dwImageExportDataSize)
	{
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	auto pImageExportData = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)pMappedFileData + dwExportDirectory);
	if (!pImageExportData)
	{
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	// TODO: EAT Module-API based blacklist scan

	auto dwEATChecksum = CPEFunctions::CalculateChecksum((DWORD)pImageExportData, dwImageExportDataSize);
	DEBUG_LOG(LL_SYS, "EAT Directory: %p EAT Base: %p EAT Size: %p Checksum: %p", dwExportDirectory, pImageExportData, dwImageExportDataSize, dwEATChecksum);
	g_nmApp->QuarentineInstance()->CheckMappedFileEAT(szFileName, dwExportDirectory, dwImageExportDataSize, dwEATChecksum);

	// Import Directory parsing
	auto dwImportDirectory = CPEFunctions::Rva2Offset((DWORD)pMappedFileData, pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if (!dwImportDirectory)
	{
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	auto dwImageImportDataSize = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
	if (!dwImageImportDataSize)
	{
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	auto pImageImportData = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)pMappedFileData + dwImportDirectory);
	if (!pImageImportData)
	{
		if (!pMappedFile->IsNull())
			pMappedFile->Destroy();

		return bRet;
	}

	// todo: check iat modules for proxy dll injection
	// TODO: IAT Module-API based blacklist scan

	auto dwIATChecksum = CPEFunctions::CalculateChecksum((DWORD)pImageImportData, dwImageImportDataSize);
	DEBUG_LOG(LL_SYS, "IAT Directory: %p IAT Base: %p IAT Size: %p Checksum: %p", dwImportDirectory, pImageImportData, dwImageImportDataSize, dwIATChecksum);
	g_nmApp->QuarentineInstance()->CheckMappedFileIAT(szFileName, dwImportDirectory, dwImageImportDataSize, dwIATChecksum);

	// TODO
	auto dwFileChecksum = CPEFunctions::CalculateChecksum((DWORD)pMappedFileData, pMappedFile->Size());
	DEBUG_LOG(LL_SYS, "File: %s checksum generated: %p", szFileName.c_str(), dwFileChecksum);
	//	fileInfo->dwFileChecksum = dwFileChecksum;


	bRet = true;

	if (!pMappedFile->IsNull())
		pMappedFile->Destroy();

	return bRet;
}



inline BOOL CertGetString(PCCERT_CONTEXT pCertContext, DWORD dwType, DWORD dwFlags, LPSTR *lpszNameString)
{
	if (pCertContext == NULL)
		return FALSE;

	DWORD dwData = CertGetNameStringA(pCertContext, dwType, 0, NULL, NULL, 0);

	if (dwData == 0)
		return FALSE;

	*lpszNameString = (LPSTR)HeapAlloc(GetProcessHeap(), 0, (dwData + 1) * sizeof(char));

	if (*lpszNameString == NULL)
		return FALSE;

	dwData = CertGetNameStringA(pCertContext, dwType, dwFlags, NULL, *lpszNameString, dwData);

	if (dwData == 0)
	{
		HeapFree(GetProcessHeap(), 0, *lpszNameString);
		return FALSE;
	}

	return TRUE;
}

VOID CheckCertificateInformations(const std::string & szFileName, PCCERT_CONTEXT pCertContext)
{
	DEBUG_LOG(LL_SYS, "Certificate information check started for: %s Context at: %p", szFileName.c_str(), pCertContext);

	if (!pCertContext)
	{
		DEBUG_LOG(LL_ERR, "Certificate context is NULL!");
		return;
	}

	auto pCertInfo = pCertContext->pCertInfo;
	if (!pCertInfo)
	{
		DEBUG_LOG(LL_ERR, "Certificate info is NULL!");
		return;
	}

	if (!pCertContext->pbCertEncoded)
	{
		DEBUG_LOG(LL_ERR, "Certificate encoded size is NULL!");
		return;
	}

	DEBUG_LOG(LL_SYS, "Cert base: %p size: %u", pCertContext->pbCertEncoded, pCertContext->cbCertEncoded);
	auto pCertHash = g_nmApp->DirFunctionsInstance()->GetSHA256(pCertContext->pbCertEncoded, pCertContext->cbCertEncoded);
	DEBUG_LOG(LL_SYS, "Cert hash: %s", pCertHash.c_str());

	LPSTR szSerialNumber = (LPSTR)"";
	if (pCertContext->pCertInfo->SerialNumber.cbData != 0)
	{
		auto lpszSerial = (LPSTR)HeapAlloc(GetProcessHeap(), 0, ((pCertContext->pCertInfo->SerialNumber.cbData * 2) + 1) * sizeof(char));

		if (lpszSerial != NULL)
		{
			szSerialNumber = lpszSerial;

			for (DWORD dwCount = pCertContext->pCertInfo->SerialNumber.cbData; dwCount != 0; dwCount--)
				szSerialNumber += sprintf(szSerialNumber, "%02X", pCertContext->pCertInfo->SerialNumber.pbData[dwCount - 1]);

			HeapFree(GetProcessHeap(), 0, lpszSerial);
		}
	}
	DEBUG_LOG(LL_SYS, "Cert serial number: %s", szSerialNumber);

	LPSTR szIssuer = (LPSTR)"";
	if (pCertContext->pCertInfo->Issuer.cbData != 0)
	{
		auto lpszIssuer = (LPSTR)HeapAlloc(GetProcessHeap(), 0, ((pCertContext->pCertInfo->Issuer.cbData * 2) + 1) * sizeof(char));

		if (lpszIssuer != NULL)
		{
			szIssuer = lpszIssuer;

			for (DWORD dwCount = pCertContext->pCertInfo->Issuer.cbData; dwCount != 0; dwCount--)
				szIssuer += sprintf(szIssuer, "%02X", pCertContext->pCertInfo->Issuer.pbData[dwCount - 1]);

			HeapFree(GetProcessHeap(), 0, lpszIssuer);
		}
	}
	DEBUG_LOG(LL_SYS, "Cert issuer: %s", szIssuer);

	LPSTR szSubject = (LPSTR)"";
	if (pCertContext->pCertInfo->Subject.cbData != 0)
	{
		auto lpszSubject = (LPSTR)HeapAlloc(GetProcessHeap(), 0, ((pCertContext->pCertInfo->Subject.cbData * 2) + 1) * sizeof(char));

		if (lpszSubject != NULL)
		{
			szSubject = lpszSubject;

			for (DWORD dwCount = pCertContext->pCertInfo->Subject.cbData; dwCount != 0; dwCount--)
				szSubject += sprintf(szSubject, "%02X", pCertContext->pCertInfo->Subject.pbData[dwCount - 1]);

			HeapFree(GetProcessHeap(), 0, lpszSubject);
		}
	}
	DEBUG_LOG(LL_SYS, "Cert subject: %s", szSubject);

	LPSTR szFriendlyName;
	if (!CertGetString(pCertContext, CERT_NAME_FRIENDLY_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, &szFriendlyName))
		szFriendlyName = (LPSTR)"";

	LPSTR szAuthority;
	if (!CertGetString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, &szAuthority))
		szAuthority = (LPSTR)"";

	LPSTR szPublisher;
	if (!CertGetString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, &szPublisher))
		szPublisher = (LPSTR)"";

	DEBUG_LOG(LL_SYS, "Cert friendly: %s Authority: %s Publisher: %s", szFriendlyName, szAuthority, szPublisher);

	g_nmApp->QuarentineInstance()->CheckSignCertInformations(szFileName, pCertHash, szSerialNumber, szIssuer, szSubject, szFriendlyName, szAuthority, szPublisher);

	HeapFree(GetProcessHeap(), 0, szFriendlyName);
	HeapFree(GetProcessHeap(), 0, szAuthority);
	HeapFree(GetProcessHeap(), 0, szPublisher);
}

BOOL CheckFileCertificates(const std::string & szFileName, HANDLE hFile)
{
	auto dwCerts = 0UL;
	if (!ImageEnumerateCertificates(hFile, CERT_SECTION_TYPE_ANY, &dwCerts, NULL, 0))
		return FALSE;

	for (auto dwCount = 0UL; dwCount < dwCerts; dwCount++)
	{
		WIN_CERTIFICATE wcHdr	= { 0 };
		wcHdr.dwLength			= 0;
		wcHdr.wRevision			= WIN_CERT_REVISION_1_0;

		if (!ImageGetCertificateHeader(hFile, dwCount, &wcHdr))
			return FALSE;


		DWORD dwLen = sizeof(WIN_CERTIFICATE) + wcHdr.dwLength;
		WIN_CERTIFICATE *pWinCert = (WIN_CERTIFICATE *)HeapAlloc(GetProcessHeap(), 0, dwLen);

		if (pWinCert == NULL)
			return FALSE;

		if (!ImageGetCertificateData(hFile, dwCount, pWinCert, &dwLen))
		{
			HeapFree(GetProcessHeap(), 0, pWinCert);
			return FALSE;
		}


		CRYPT_VERIFY_MESSAGE_PARA cvmp	= { 0 };
		cvmp.cbSize						= sizeof(CRYPT_VERIFY_MESSAGE_PARA);
		cvmp.dwMsgAndCertEncodingType	= (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING);

		PCCERT_CONTEXT pCertContext = NULL;
		if (!CryptVerifyMessageSignature(&cvmp, dwCount, pWinCert->bCertificate, pWinCert->dwLength, NULL, NULL, &pCertContext))
		{
			HeapFree(GetProcessHeap(), 0, pWinCert);
			return FALSE;
		}

		CheckCertificateInformations(szFileName, pCertContext);


		if (!CertFreeCertificateContext(pCertContext))
		{
			HeapFree(GetProcessHeap(), 0, pWinCert);
			return FALSE;
		}

		HeapFree(GetProcessHeap(), 0, pWinCert);
	}

	return TRUE;
}


bool CheckFileBasicInformations(const std::string & szFileName)
{
	DEBUG_LOG(LL_SYS, "File basic information scan started! File: %s", szFileName.c_str());

	auto hFile = g_winapiApiTable->CreateFileA(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (!IS_VALID_HANDLE(hFile))
	{
		DEBUG_LOG(LL_ERR, "CreateFileA fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto dwFileLen = g_winapiApiTable->GetFileSize(hFile, NULL);
	if (!dwFileLen || dwFileLen == INVALID_FILE_SIZE)
	{
		DEBUG_LOG(LL_ERR, "GetFileSize fail! Error code: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	BYTE* byImage = new BYTE[dwFileLen];
	if (!byImage)
	{
		DEBUG_LOG(LL_ERR, "Image allocation fail!");
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	DWORD dwReadedBytes;
	BOOL readRet = g_winapiApiTable->ReadFile(hFile, byImage, dwFileLen, &dwReadedBytes, NULL);
	if (!readRet || dwReadedBytes != dwFileLen)
	{
		DEBUG_LOG(LL_ERR, "ReadFile fail! Error code: %u", g_winapiApiTable->GetLastError());
		delete[] byImage;
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto wszFileName = std::wstring(szFileName.begin(), szFileName.end());

	auto iSignRet = g_nmApp->ScannerInstance()->IsFileDigitalSigned(wszFileName.c_str());
	auto iSignCheckRet = BOOL(FALSE);
	if (iSignRet)
		iSignCheckRet = CheckFileCertificates(szFileName, hFile);
	auto fEntropy = g_nmApp->FunctionsInstance()->GetEntropy(byImage, dwFileLen);
	auto szShaHash = g_nmApp->DirFunctionsInstance()->GetSHA256(byImage, dwFileLen);

	g_nmApp->QuarentineInstance()->CheckFileBasicInformations(szFileName, dwFileLen, fEntropy, szShaHash, iSignRet, iSignCheckRet);

	g_winapiApiTable->CloseHandle(hFile);
	delete[] byImage;
	return true;
}

bool CheckFileInformations(const std::string & szFileName)
{
	DEBUG_LOG(LL_SYS, "File information scan started! File: %s", szFileName.c_str());

	auto hFile = g_winapiApiTable->CreateFileA(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (!IS_VALID_HANDLE(hFile))
	{
		DEBUG_LOG(LL_ERR, "CreateFileA fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	BY_HANDLE_FILE_INFORMATION hfi;
	if (!GetFileInformationByHandle(hFile, &hfi))
	{
		DEBUG_LOG(LL_ERR, "GetFileInformationByHandle fail! Error code: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	g_nmApp->QuarentineInstance()->CheckFileInformations(szFileName, hfi.nFileIndexLow, hfi.nFileIndexHigh, hfi.dwVolumeSerialNumber);

	g_winapiApiTable->CloseHandle(hFile);
	return true;
}

bool CheckFileExist(const std::string & szFileName)
{
	PVOID OldValue = NULL;
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &OldValue))
		return true;

	auto bExist = std::experimental::filesystem::exists(szFileName);

	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, OldValue, nullptr))
		return true;

	if (bExist == false)
	{
		SCANNER_LOG(LL_ERR, "Target file: '%s' is NOT visible or deleted!", szFileName.c_str());

		auto iViolationIndex = static_cast<int>(SCAN_FILE_EXIST_CHECK);
		std::string szMessage = "Scan ID: " + std::to_string(iViolationIndex) + " File: " + szFileName;
		g_nmApp->ScannerInstance()->SendViolationMessageToMasterServer(ERR_VIOLATION_FILE_EXIST_CHECK, g_winapiApiTable->GetLastError(), szMessage, false);

		return false;
	}

	return true;
}


bool IScanner::IsScannedFile(const std::string & szFileName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	return HasInVector(m_vScannedFileNames, szFileName);
}

bool IScanner::ScanFile(const std::string & szFileName, EFileScanTypes fileType)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	SCANNER_LOG(LL_SYS, "File scanner has been started! Target file: %s", szFileName.c_str());

	if (IsScannedFile(szFileName))
	{
		SCANNER_LOG(LL_SYS, "File already scanned!");
		return false;
	}

	// Add to checked list
	m_vScannedFileNames.push_back(szFileName);

	// Scan routine

	/// 1
	// Exist check
	auto bRet = CheckFileExist(szFileName);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "File exist scan completed! Result: %d", bRet);

	/// 2
	// File name + file size + Entropy + Hash
	bRet = CheckFileBasicInformations(szFileName);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "File basic information scan completed! Result: %d", bRet);

	/// 3
	// nFileIndexLow, nFileIndexHigh, dwVolumeSerialNumber
	bRet = CheckFileInformations(szFileName);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "File information scan completed! Result: %d", bRet);

	/// 4
	// PE Informations
	bRet = CheckFilePEInformations(szFileName);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "File PE information scan completed! Result: %d", bRet);

	/// 4
	// Region hash Informations
	bRet = CheckFileRegionHashes(szFileName);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "File PE information scan completed! Result: %d", bRet);

	return true;
}

bool IScanner::ScanFile(HANDLE hProcess, EFileScanTypes fileType)
{
	SCANNER_LOG(LL_SYS, "Process file scanner has been started! Target process: %u(%p)", g_winapiApiTable->GetProcessId(hProcess), hProcess);

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

	// TODO: tolower

	ScanFile(szProcessName, fileType);
	return true;
}

