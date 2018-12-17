#include "NM_Index.h"
#include "NM_Main.h"
#include "DirFunctions.h"
#include "DynamicWinapi.h"
#include "Functions.h"
#include "Defines.h"
#include "dirent.h"
#include "MappedFile.h"
#include "md5.h"
#include "Timer.h"
#include "WinVerHelper.h"

#include <cryptopp/sha.h>
#include <cryptopp/md5.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>


bool CDirFunctions::IsFileExist(const std::string & szFileName)
{
	auto dwAttrib = g_winapiApiTable->GetFileAttributesA(szFileName.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool CDirFunctions::IsDirExist(const std::string & szDirName)
{
	auto dwAttrib = g_winapiApiTable->GetFileAttributesA(szDirName.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


std::string CDirFunctions::ReadFileContent(const std::string & szFileName)
{
	std::ifstream in(szFileName.c_str(), std::ios_base::binary);
	in.exceptions(std::ios_base::badbit | std::ios_base::failbit | std::ios_base::eofbit);
	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void CDirFunctions::WriteFileContent(const std::string & szFileName, const std::string & szText)
{
	std::ofstream f(szFileName.c_str(), std::ofstream::out | std::ofstream::app);
	f << szText.c_str() << std::endl;
	f.close();
}

bool CDirFunctions::ManageFsRedirection(bool bDisable, PVOID pCookie, PVOID * ppCookie)
{
	if (IsWindowsVistaOrGreater() && g_nmApp->FunctionsInstance()->IsWow64Process(NtCurrentProcess))
	{
		PVOID OldValue = NULL;
		if (bDisable && !g_winapiApiTable->Wow64DisableWow64FsRedirection(&OldValue))
		{
			if (ppCookie) *ppCookie = OldValue;
			return false;
		}

		if (!bDisable && !g_winapiApiTable->Wow64RevertWow64FsRedirection(pCookie))
			return false;
	}
	return true;
}

int CDirFunctions::DeleteDirectory(const std::string & refcstrRootDirectory, bool bDeleteSubdirectories)
{
	bool             bSubdirectory = false;
	std::string      strFilePath = "";
	WIN32_FIND_DATAA FileInformation = { 0 };

	auto strPattern = refcstrRootDirectory + "\\*.*";

	auto hFile = g_winapiApiTable->FindFirstFileA(strPattern.c_str(), &FileInformation);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
		return 0;

	do
	{
		if (FileInformation.cFileName[0] != '.')
		{
			strFilePath.erase();
			strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

			if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (bDeleteSubdirectories)
				{
					// Delete subdirectory
					auto iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
					if (iRC)
						return iRC;
				}
				else
					bSubdirectory = true;
			}
			else
			{
				// Set file attributes
				if (g_winapiApiTable->SetFileAttributesA(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL))
					g_winapiApiTable->DeleteFileA(strFilePath.c_str());
			}
		}
	} while (g_winapiApiTable->FindNextFileA(hFile, &FileInformation));

	// Close handle
	g_winapiApiTable->FindClose(hFile);

	DWORD dwError = g_winapiApiTable->GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
		return dwError;
	
	if (!bSubdirectory)
	{
		// Set directory attributes
		if (!g_winapiApiTable->SetFileAttributesA(refcstrRootDirectory.c_str(), FILE_ATTRIBUTE_NORMAL))
			return g_winapiApiTable->GetLastError();

		// Delete directory
		if (!g_winapiApiTable->RemoveDirectoryA(refcstrRootDirectory.c_str()))
			return g_winapiApiTable->GetLastError();
	}
	
	return 0;
}

std::string CDirFunctions::WinPath()
{
	char buffer[MAX_PATH] = { 0 };
	g_winapiApiTable->GetWindowsDirectoryA(buffer, MAX_PATH);
	return buffer;
}

std::string CDirFunctions::SystemPath()
{
	char buffer[MAX_PATH] = { 0 };
	g_winapiApiTable->GetSystemDirectoryA(buffer, MAX_PATH);
	return buffer;
}

std::string CDirFunctions::ExeNameWithPath()
{
	char buffer[MAX_PATH] = { 0 };
	g_winapiApiTable->GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return buffer;
}

std::string CDirFunctions::ExeName()
{
	auto szExeNameWithPath = ExeNameWithPath();
	auto szExeNameWithoutPath = GetNameFromPath(szExeNameWithPath);
	return szExeNameWithoutPath;
}

std::string CDirFunctions::ExePath()
{
	auto szBuffer = ExeNameWithPath();
	return GetPathFromProcessName(szBuffer);
}

bool CDirFunctions::IsFromWindowsPath(const std::string & szPath)
{
	auto szLowerWinPath = WinPath();
	std::transform(szLowerWinPath.begin(), szLowerWinPath.end(), szLowerWinPath.begin(), tolower);

	return (szPath.find(szLowerWinPath) != std::string::npos);
}

bool CDirFunctions::IsFromCurrentPath(const std::string & szPath)
{
	auto szLowerExePath = ExePath();
	std::transform(szLowerExePath.begin(), szLowerExePath.end(), szLowerExePath.begin(), tolower);

	return (szPath.find(szLowerExePath) != std::string::npos);
}

std::string CDirFunctions::GetNameFromPath(std::string __szFileName)
{
	auto szFileName = __szFileName;
	auto iLastSlash = szFileName.find_last_of("\\/");
	szFileName = szFileName.substr(iLastSlash + 1, szFileName.length() - iLastSlash);
	return szFileName;
}

std::wstring CDirFunctions::GetNameFromPath(std::wstring __wszFileName)
{
	auto wszFileName = __wszFileName;
	auto iLastSlash = wszFileName.find_last_of(L"\\/");
	wszFileName = wszFileName.substr(iLastSlash + 1, wszFileName.length() - iLastSlash);
	return wszFileName;
}

std::string CDirFunctions::GetPathFromProcessName(std::string szBuffer)
{
	auto szCopyBuffer = szBuffer;
	auto pos = szCopyBuffer.find_last_of("\\/");
	return szCopyBuffer.substr(0, pos);
}


DWORD CDirFunctions::GetFileSize(const std::string & szName)
{
	auto hFile = g_winapiApiTable->CreateFileA(szName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile && hFile != INVALID_HANDLE_VALUE)
		return g_winapiApiTable->GetFileSize(hFile, NULL);
	return 0;
}
std::string CDirFunctions::GetFileMd5(const std::string & szName)
{
	std::string szOutput;

	CMappedFile pMappedFile;
	LPVOID pMappedFileData;
	if (!pMappedFile.Create(szName.c_str(), &pMappedFileData, 0, 0))
		return szOutput;

	CryptoPP::Weak::MD5 hash;
	uint8_t csum[CryptoPP::Weak::MD5::DIGESTSIZE];

	hash.CalculateDigest(csum, (const byte*)pMappedFileData, pMappedFile.Size());
	CryptoPP::StringSource(csum, sizeof(csum), true, new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(szOutput))));

	pMappedFile.Destroy();
	return szOutput;
}

void CDirFunctions::CheckFileSize(const std::string & szName, DWORD ulCorrectSize)
{
	auto ulCurrentSize	= GetFileSize(szName.c_str());
	auto bEqRet			= ulCorrectSize == ulCurrentSize;

	DEBUG_LOG(LL_SYS, "File size check event. File: %s Correct size: %u Current Size: %u Eq: %d", szName.c_str(), ulCorrectSize, ulCurrentSize, bEqRet);

	if (bEqRet == false) 
	{
		auto szWarnStr = g_nmApp->FunctionsInstance()->CreateString(xorstr("File modification detected! File: %s Error code: 0x1").crypt_get(), szName.c_str());
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_21_FAIL, g_winapiApiTable->GetLastError());
	}
}

void CDirFunctions::CheckFileMd5(const std::string & szName, const std::string & szCorrectHash)
{
	auto szCurrentHash	= GetFileMd5(szName.c_str());

	auto szLowerCurrentHash = g_nmApp->FunctionsInstance()->szLower(szCurrentHash);
	auto szLowerCorrectHash = g_nmApp->FunctionsInstance()->szLower(szCorrectHash);

	auto bEqRet = bool(szLowerCurrentHash == szLowerCorrectHash);

	DEBUG_LOG(LL_SYS, "File md5 check event. File: %s Correct md5: %s Current md5: %s Eq: %d", szName.c_str(), szCorrectHash.c_str(), szCurrentHash.c_str(), bEqRet);

	if (bEqRet == false)
	{
		auto szWarnStr = g_nmApp->FunctionsInstance()->CreateString(xorstr("File modification detected! File: %s Error code: 0x3").crypt_get(), szName.c_str());
		g_nmApp->OnCloseRequest(EXIT_ERR_DIR_FUNC_23_FAIL, g_winapiApiTable->GetLastError());
	}
}

bool CDirFunctions::IsPackedExecutable(const std::string & szName)
{
	bool bIsPacked = false;

	auto hFile = g_winapiApiTable->CreateFileA(szName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (!IS_VALID_HANDLE(hFile))
	{
		DEBUG_LOG(LL_ERR, "CreateFileA fail! Error code: %u", g_winapiApiTable->GetLastError());
		return bIsPacked;
	}

	auto dwFileLen = g_winapiApiTable->GetFileSize(hFile, NULL);
	if (!dwFileLen || dwFileLen == INVALID_FILE_SIZE)
	{
		DEBUG_LOG(LL_ERR, "GetFileSize fail! Error code: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hFile);
		return bIsPacked;
	}

	auto pImage = reinterpret_cast<BYTE*>(g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(dwFileLen, __FUNCTION__));
	if (!pImage)
	{
		DEBUG_LOG(LL_ERR, "Image allocation fail!");
		g_winapiApiTable->CloseHandle(hFile);
		return bIsPacked;
	}

	DWORD dwReadedBytes;
	BOOL readRet = g_winapiApiTable->ReadFile(hFile, pImage, dwFileLen, &dwReadedBytes, NULL);
	if (!readRet || dwReadedBytes != dwFileLen)
	{
		DEBUG_LOG(LL_ERR, "ReadFile fail! Error code: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hFile);
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pImage);
		return bIsPacked;
	}
	g_winapiApiTable->CloseHandle(hFile);

	float fEntropy = g_nmApp->FunctionsInstance()->GetEntropy(pImage, dwFileLen);

	g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pImage);
	return (fEntropy > 7.5f);
}

std::string CDirFunctions::CreateTempFileName(const std::string & szPrefix)
{
	char szTempPath[MAX_PATH + 1] = { 0 };
	LI_FIND(GetTempPathA)(MAX_PATH, szTempPath);

	char szTempName[MAX_PATH + 1] = { 0 };
	LI_FIND(GetTempFileNameA)(szTempPath, !szPrefix.empty() ? szPrefix.c_str() : xorstr("nm1").crypt_get(), !szPrefix.empty() ? true : false, szTempName);

	return szTempName;
}

std::string CDirFunctions::GetSHA256(LPCVOID c_lpData, std::size_t cbSize)
{
	auto szOutput = std::string("");

	CryptoPP::SHA256 sha;
	uint8_t csum[CryptoPP::SHA256::DIGESTSIZE];

	sha.CalculateDigest(csum, (const byte*)c_lpData, cbSize);
	CryptoPP::StringSource ss(csum, sizeof(csum), true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(szOutput)));

	return szOutput;
}

std::string CDirFunctions::GetFileSHA256(const std::string & szName)
{
	auto szOutput = std::string("");

	CMappedFile pMappedFile;
	LPVOID pMappedFileData;
	if (!pMappedFile.Create(szName.c_str(), &pMappedFileData, 0, 0))
		return szOutput;

	szOutput = GetSHA256(pMappedFileData, pMappedFile.Size());

	if (!pMappedFile.IsNull())
		pMappedFile.Destroy();
	return szOutput;
}

#undef CRYPTOPP_ENABLE_NAMESPACE_WEAK
