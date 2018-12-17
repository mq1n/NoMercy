#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "WinVerHelper.h"

#include <winevt.h>
#include "boost/algorithm/string/replace.hpp"
#include <boost/foreach.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/tokenizer.hpp>


inline bool IsVBoxInstalled()
{
	HKEY hKey = NULL;
	LRESULT lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("Software\\Oracle\\VirtualBox").crypt_get(), 0, KEY_READ, &hKey);

	bool bRet = (hKey != NULL);

	if (hKey) 
		RegCloseKey(hKey);

	return bRet;
}

ULONGLONG WindowsTickToUnixSeconds(ULONGLONG windowsTicks)
{
	return (ULONGLONG)(windowsTicks / 10000000 - 11644473600);
}

LPWSTR AllocEvtFormatMessage(EVT_HANDLE ehPublisherMetadata, EVT_HANDLE ehLogEntry, DWORD dwFlags)
{
	LPWSTR wszOutput = NULL;
	DWORD dwLength = 32768;
	DWORD dwBufferUsedSize = 0;

	while (1) 
	{
		wszOutput = (LPWSTR)realloc(wszOutput, dwLength * sizeof(WCHAR));
		if (wszOutput == NULL)
			return NULL;

		if (EvtFormatMessage(ehPublisherMetadata, ehLogEntry, 0, 0, NULL, dwFlags, dwLength, wszOutput, &dwBufferUsedSize))
			return wszOutput;

		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
			free(wszOutput);
			return NULL;
		}

		dwLength *= 10;
	}

	return wszOutput;
}

void ProcessEventRecord(std::string szServiceName, std::string szFileName)
{
	// LPLog->AddLog(0, "ServiceName: '%s'", szServiceName.c_str());
	// LPLog->AddLog(0, "FileName: '%s'", szFileName.c_str());

	std::string szLowerServiceName = g_nmApp->FunctionsInstance()->szLower(szServiceName);

	char szWarn[2048];
	if (strstr(szLowerServiceName.c_str(), xorstr("vbox").crypt_get()))
	{
		if (IsVBoxInstalled() == false)
		{
			sprintf(szWarn, xorstr("Unknown device detected! %s - %d").crypt_get(), szFileName.c_str(), 1);
//			g_nmApp->FunctionsInstance()->CloseProcess(szWarn, false, "");
		}

		char szLink[1024];
		sprintf(szLink, xorstr("\\\\.\\%s").crypt_get(), szServiceName.c_str());
		if (INVALID_HANDLE_VALUE == g_winapiApiTable->CreateFileA(szLink, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0))
		{
			sprintf(szWarn, xorstr("Unknown device detected! %s - %d").crypt_get(), szFileName.c_str(), 2);
//			g_nmApp->FunctionsInstance()->CloseProcess(szWarn, false, "");
		}
	}
}

void CheckEventRecord(LPCWSTR c_wszSource, LPCWSTR c_wszMessage)
{
	typedef std::vector <std::string> pVecReadResult;

	std::wstring wszSource = c_wszSource;
	std::string szSource(wszSource.begin(), wszSource.end());
	if (strstr(szSource.c_str(), xorstr("Service Control Manager").crypt_get()))
	{
		std::wstring wszMessage = c_wszMessage;
		std::string szMessage(wszMessage.begin(), wszMessage.end());

		std::string szServiceName = "";
		std::string szFileName = "";

		std::vector <std::string> stringList;
		boost::iter_split(stringList, szMessage, boost::first_finder(xorstr("\n").crypt_get()));

		for (std::size_t i = 0; i < stringList.size(); i++)
		{

			// Service Name:  VBoxDrv
			if (strstr(stringList[i].c_str(), xorstr("Service Name").crypt_get()))
			{
				std::vector<std::string> subStringList;
				boost::iter_split(subStringList, stringList[i], boost::first_finder(xorstr(":  ").crypt_get()));
				szServiceName = subStringList[1];
			}

			// Service File Name:  C:\Windows\system32\drivers\VBoxDrv.sys
			if (strstr(stringList[i].c_str(), xorstr("Service File Name").crypt_get()))
			{
				std::vector<std::string> subStringList;
				boost::iter_split(subStringList, stringList[i], boost::first_finder(xorstr(":  ").crypt_get()));
				szFileName = subStringList[1];
			}

		}

		if (szServiceName.empty() == false && szFileName.empty() == false)
			ProcessEventRecord(szServiceName, szFileName);
	}
}

bool CheckLog(EVT_HANDLE ehRenderContext, EVT_HANDLE ehLogEntry)
{
	PEVT_VARIANT peVariant = nullptr;
	DWORD dwLength = 32768;
	DWORD dwBufferUsedSize = 0;
	DWORD dwPropertyCount = 0;

	while (1) {
		peVariant = (PEVT_VARIANT)realloc(peVariant, dwLength);
		if (peVariant == NULL) {
			DEBUG_LOG(LL_ERR, "CheckLog alloc fail! Last Error: %u", g_winapiApiTable->GetLastError());
			return false;
		}

		if (g_winapiApiTable->EvtRender(ehRenderContext, ehLogEntry, EvtRenderEventValues, dwLength, peVariant, &dwBufferUsedSize, &dwPropertyCount))
			break;

		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			DEBUG_LOG(LL_ERR, "CheckLog unknown error! Last Error: %u", g_winapiApiTable->GetLastError());
			free(peVariant);
			return false;
		}

		dwLength = dwBufferUsedSize;
	}

	LPWSTR wszMetaMessage = NULL;
	EVT_HANDLE ehMetaData = g_winapiApiTable->EvtOpenPublisherMetadata(NULL, peVariant[2].StringVal, NULL, 0, 0);
	if (ehMetaData)
		wszMetaMessage = AllocEvtFormatMessage(ehMetaData, ehLogEntry, EvtFormatMessageEvent);


	CheckEventRecord(peVariant[2].StringVal, wszMetaMessage);


	if (wszMetaMessage)
		free(wszMetaMessage);
	if (ehMetaData)
		g_winapiApiTable->EvtClose(ehMetaData);

	free(peVariant);
	return true;
}

bool EventLogCheckEx()
{
	if (IsWindowsVistaOrGreater() == false)
	{
		DEBUG_LOG(LL_SYS, "EventLogCheckEx not supported os");
		return true;
	}

	std::string szPath1 = xorstr("/Event/System/EventRecordID").crypt_get();
	std::wstring wszPath1(szPath1.begin(), szPath1.end());
	std::string szPath2 = xorstr("/Event/System/TimeCreated/@SystemTime").crypt_get();
	std::wstring wszPath2(szPath2.begin(), szPath2.end());
	std::string szPath3 = xorstr("/Event/System/Provider/@Name").crypt_get();
	std::wstring wszPath3(szPath3.begin(), szPath3.end());
	std::string szPath4 = xorstr("/Event/System/Computer").crypt_get();
	std::wstring wszPath4(szPath4.begin(), szPath4.end());

	LPCWSTR c_wszPaths[] = {
		wszPath1.c_str(), wszPath2.c_str(), wszPath3.c_str(), wszPath4.c_str()
	};

	bool bRet = false;
	int iCount = 0;

	EVT_HANDLE ehRenderContext = g_winapiApiTable->EvtCreateRenderContext(_countof(c_wszPaths), c_wszPaths, EvtRenderContextValues);
	if (ehRenderContext == NULL) {
		DEBUG_LOG(LL_ERR, "EvtCreateRenderContext fail! Error: %u", g_winapiApiTable->GetLastError());
		return bRet;
	}

	std::string szSystem = xorstr("System").crypt_get();
	std::wstring wszSystem(szSystem.begin(), szSystem.end());

	std::string szAsterisk = xorstr("*").crypt_get();
	std::wstring wszAsterisk(szAsterisk.begin(), szAsterisk.end());

	EVT_HANDLE ehQuery = g_winapiApiTable->EvtQuery(NULL, wszSystem.c_str(), wszAsterisk.c_str(), EvtQueryChannelPath | EvtQueryReverseDirection);
	if (ehQuery == NULL) {
		DEBUG_LOG(LL_ERR, "EvtQuery fail! Error: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->EvtClose(ehRenderContext);
		return bRet;
	}

	DWORD i = 0;
	DWORD dwEventCount = 0;
	EVT_HANDLE ehEvents[1024];
	while (g_winapiApiTable->EvtNext(ehQuery, _countof(ehEvents), ehEvents, INFINITE, 0, &dwEventCount))
	{
		for (i = 0; i < dwEventCount; i++)
		{
			if (!CheckLog(ehRenderContext, ehEvents[i]))
				goto leave_loop;
			
			if (!g_winapiApiTable->EvtClose(ehEvents[i]))
				goto leave_loop;		

			if (iCount > 1000)
				goto leave_loop; // First 1000 log
			
			iCount++;
		}
	}

	if (GetLastError() == ERROR_NO_MORE_ITEMS)
		bRet = true;

	if (iCount <= 10) {
		// cleaned log, kill process
	}

leave_loop:
	while (i < dwEventCount) {
		g_winapiApiTable->EvtClose(ehEvents[i]);
		i++;
	}

	g_winapiApiTable->EvtClose(ehQuery);
	g_winapiApiTable->EvtClose(ehRenderContext);

	return bRet;
}


void IScanner::InitializeEventLogCheck()
{
	EventLogCheckEx();
}

