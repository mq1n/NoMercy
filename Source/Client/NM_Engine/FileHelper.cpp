#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "FileHelper.h"
#include "Defines.h"
#include "WinVerHelper.h"

bool CFileFunctions::File2Mem(const std::string & szFileName, std::string * pszOutput)
{
	auto hFile = g_winapiApiTable->CreateFileA(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!IS_VALID_HANDLE(hFile))
	{
		DEBUG_LOG(LL_ERR, "CreateFileA fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto dwFileSize = g_winapiApiTable->GetFileSize(hFile, NULL);
	if (dwFileSize == INVALID_FILE_SIZE || !dwFileSize)
	{
		DEBUG_LOG(LL_ERR, "GetFileSize fail! Error: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	auto pFileBytes = g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(dwFileSize, __FUNCTION__);
	if (!pFileBytes)
	{
		DEBUG_LOG(LL_ERR, "pFileBytes Allocation fail! Error: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hFile);
		return false;
	}

	DWORD dwReadBytes = 0;
	auto bReadFile = g_winapiApiTable->ReadFile(hFile, pFileBytes, dwFileSize, &dwReadBytes, NULL);
	if (!bReadFile || dwReadBytes != dwFileSize) 
	{
		DEBUG_LOG(LL_ERR, "ReadFile fail! Error: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hFile);
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBytes);
		return false;
	}
	g_winapiApiTable->CloseHandle(hFile);

	std::string szBuffer(reinterpret_cast< char const* >(pFileBytes));
	if (pszOutput)
		*pszOutput = szBuffer;

	g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pFileBytes);
	return true;
}

bool CFileFunctions::Mem2File(const std::string & szFileName, BYTE* pBuffer, DWORD dwSize)
{
	auto hFile = g_winapiApiTable->CreateFileA(szFileName.c_str(), FILE_APPEND_DATA, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!IS_VALID_HANDLE(hFile))
	{
		DEBUG_LOG(LL_ERR, "CreateFileA fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	
	auto dwSetPtrRet = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if (dwSetPtrRet == INVALID_SET_FILE_POINTER)
	{
		DEBUG_LOG(LL_ERR, "SetFilePointer fail! Error: %u", g_winapiApiTable->GetLastError());
		CloseHandle(hFile);
		return false;
	}

	auto dwWritedBytes = 0UL;
	auto bWritten = WriteFile(hFile, pBuffer, dwSize, &dwWritedBytes, NULL);
	if (!bWritten || dwWritedBytes != dwSize)
	{
		DEBUG_LOG(LL_ERR, "WriteFile fail! Error: %u", g_winapiApiTable->GetLastError());
		CloseHandle(hFile);
		return false;
	}

	if (!FlushFileBuffers(hFile))
	{
		DEBUG_LOG(LL_ERR, "FlushFileBuffers fail! Error: %u", g_winapiApiTable->GetLastError());
		CloseHandle(hFile);
		return false;
	}

	if (!SetEndOfFile(hFile))
	{
		DEBUG_LOG(LL_ERR, "SetEndOfFile fail! Error: %u", g_winapiApiTable->GetLastError());
		CloseHandle(hFile);
		return false;
	}
	
	CloseHandle(hFile);
	return true;
}
