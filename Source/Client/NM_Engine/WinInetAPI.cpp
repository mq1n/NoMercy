#include "NM_Index.h"
#include "NM_Main.h"
#include "WinInetAPI.h"
#include "DynamicWinapi.h"
#include "Functions.h"
#include "DirFunctions.h"
#include "Defines.h"

#include <boost/algorithm/string/predicate.hpp>


CInternetAPI::CInternetAPI()
{
	m_szWebAgentName = xorstr("NM_Web_Agent").crypt_get();
}

bool CInternetAPI::ExistOnHostsFile(const std::string & szEntry)
{
	auto szHosts = g_nmApp->DirFunctionsInstance()->WinPath() + xorstr("\\System32\\drivers\\etc\\hosts").crypt_get();
	if (g_nmApp->DirFunctionsInstance()->IsFileExist(szHosts))
	{
		if (g_nmApp->DirFunctionsInstance()->ReadFileContent(szHosts).find(szEntry) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

bool CInternetAPI::CheckInternetConnection(const std::string & szTargetUrl)
{
	auto bConnect = g_winapiApiTable->InternetCheckConnectionA(szTargetUrl.c_str(), FLAG_ICC_FORCE_CONNECTION, 0);
	if (!bConnect) 
	{
		DEBUG_LOG(LL_ERR, "InternetCheckConnectionA fail! Last error: %u", g_winapiApiTable->GetLastError());
	}
	return bConnect;
}

bool CInternetAPI::WebStatusCheck(const std::string & szAddress, LPDWORD pdwWebStat)
{
	DWORD uiStatCode = 0;
	DWORD statCodeLen = sizeof(DWORD);

	auto hInternet = g_winapiApiTable->InternetOpenA(m_szWebAgentName.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (!hInternet)
	{
		DEBUG_LOG(LL_ERR, "InternetOpenA fail. Error: %u", g_winapiApiTable->GetLastError());

		return false;
	}

	auto hRequestHandle = g_winapiApiTable->InternetOpenUrlA(hInternet, szAddress.c_str(), NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, NULL);
	if (!hRequestHandle)
	{
		DEBUG_LOG(LL_ERR, "InternetOpenUrlA fail. Error: %u", g_winapiApiTable->GetLastError());

		g_winapiApiTable->InternetCloseHandle(hInternet);
		return false;
	}

	auto hQuery = g_winapiApiTable->HttpQueryInfoA(hRequestHandle, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &uiStatCode, &statCodeLen, NULL);
	if (!hQuery)
	{
		DEBUG_LOG(LL_ERR, "HttpQueryInfoA fail. Error: %u", g_winapiApiTable->GetLastError());

		g_winapiApiTable->InternetCloseHandle(hInternet);
		g_winapiApiTable->InternetCloseHandle(hRequestHandle);
		return false;
	}

	g_winapiApiTable->InternetCloseHandle(hInternet);
	g_winapiApiTable->InternetCloseHandle(hRequestHandle);

	if (pdwWebStat)
		*pdwWebStat = uiStatCode;
	return true;
}

bool CInternetAPI::IsManipulatedConnection()
{
	return false; // rip timeapi

	std::string szReadData = "";
	std::size_t uiDataSize = 0;
	auto bReadRet = ReadUrl(xorstr("http://www.timeapi.org/utc/now?\\Y\\m\\d").crypt_get(), &szReadData, &uiDataSize);;
	if (!bReadRet || !uiDataSize || uiDataSize != szReadData.size())
	{
		DEBUG_LOG(LL_ERR, "ReadUrl fail! Read ret: %d Read size: %u Data size: %u", bReadRet, szReadData.size(), uiDataSize);
		return false;
	}

	return (uiDataSize != 8);
}

bool CInternetAPI::IsCorrectIPAddressOfWebsite(const std::string & szWebsite, const std::string & szIPAddress)
{
	WSADATA2 wsaData;
	int iResult = g_winapiApiTable->WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		DEBUG_LOG(LL_ERR, "WSAStartup fail! Ret: %d Last error: %d", iResult, WSAGetLastError());
		return false;
	}

	auto remoteHost = g_winapiApiTable->gethostbyname(szWebsite.c_str());
	if (!remoteHost)
	{
		DEBUG_LOG(LL_ERR, "gethostbyname fail! Last error: %d", WSAGetLastError());
		g_winapiApiTable->WSACleanup();
		return false;
	}
	g_winapiApiTable->WSACleanup();


	if (remoteHost && remoteHost->h_addrtype == AF_INET)
	{
		int i = 0;
		while (remoteHost->h_addr_list[i] != 0)
		{
			in_addr addr;
			addr.s_addr = *(DWORD*)remoteHost->h_addr_list[i++];

			DEBUG_LOG(LL_SYS, "IsCorrectIPAddressOfWebsite event new addr detected. Correct Addr: %s Addr: %s", g_winapiApiTable->inet_ntoa(addr), szIPAddress.c_str());
			if (boost::iequals(szIPAddress.c_str(), g_winapiApiTable->inet_ntoa(addr)))
				return true;
		}
	}

	DEBUG_LOG(LL_ERR, "IsCorrectIPAddressOfWebsite failed! Remotehost: %p Addrtype: %d", remoteHost, remoteHost->h_addrtype);
	return false;
}

bool CInternetAPI::ReadUrl(const std::string & szAddress, std::string * pszResult, std::size_t * puiSize)
{
	auto vTempdata			= std::vector<uint8_t>();
	char szTempBuffer[4096] = { 0 };
	auto dwBytesRead		= 0UL;

	auto hInternet = g_winapiApiTable->InternetOpenA(m_szWebAgentName.c_str(), NULL, NULL, NULL, NULL);
	if (!hInternet) 
	{
		DEBUG_LOG(LL_ERR, "InternetOpenA fail. Error: %u", GetLastError());
		return false;
	}

	auto hFile = g_winapiApiTable->InternetOpenUrlA(hInternet, szAddress.c_str(), NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, NULL);
	if (!hFile)
	{
		DEBUG_LOG(LL_ERR, "InternetOpenUrlA fail. Error: %u", GetLastError());

		g_winapiApiTable->InternetCloseHandle(hInternet);
		return false;
	}

#if 0
	auto dwAvailableBytes = 0UL;
	if (!InternetQueryDataAvailable(hFile, &dwAvailableBytes, 0, 0))
	{
		DEBUG_LOG(LL_ERR, "InternetOpenUrlA fail. Error: %u", GetLastError());

		g_winapiApiTable->InternetCloseHandle(hInternet);
		g_winapiApiTable->InternetCloseHandle(hFile);
		return false;
	}
	DEBUG_LOG(LL_SYS, "Available byte count: %p", dwAvailableBytes);
#endif

	do {
		if (g_winapiApiTable->InternetReadFile(hFile, szTempBuffer, sizeof(szTempBuffer), &dwBytesRead))
			std::copy(&szTempBuffer[0], &szTempBuffer[dwBytesRead], std::back_inserter(vTempdata));
	} while (dwBytesRead);

	if (vTempdata.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Any data can not read. Last Error: %u", g_winapiApiTable->GetLastError());

		g_winapiApiTable->InternetCloseHandle(hInternet);
		g_winapiApiTable->InternetCloseHandle(hFile);
		return false;
	}

	g_winapiApiTable->InternetCloseHandle(hInternet);
	g_winapiApiTable->InternetCloseHandle(hFile);

	if (puiSize)
		*puiSize = vTempdata.size();

	std::string szData(vTempdata.begin(), vTempdata.end());
	if (pszResult)
		*pszResult = szData;

	if (!vTempdata.empty())
		vTempdata.clear();

	return true;
}

bool CInternetAPI::PostData(const std::string & szHost, const std::string & szUrl, const char* c_szFileName, LPBYTE pData, std::size_t ulDataSize)
{	
	CHAR __formname[] = { 'u', 'p', 'l', 'o', 'a', 'd', 'e', 'd', 'f', 'i', 'l', 'e', 0x0 }; // uploadedfile
	CHAR __formfilename[] = { 'L', 'o', 'g', '.', 't', 'x', 't', 0x0 }; // Log.txt
	CHAR __boundary[] = { 'M', 'D', '5', '_', '0', 'b', 'e', '6', '3', 'c', 'd', 'a', '3', 'b', 'f', '4', '2', '1', '9', '3', 'e', '4', '3', '0', '3', 'd', 'b', '2', 'c', '5', 'a', 'c', '3', '1', '3', '8', 0x0 }; // MD5_0be63cda3bf42193e4303db2c5ac3138
	std::string boundary(__boundary);

	static std::string hdrs = xorstr("Content-Type: multipart/form-data; boundary=").crypt_get() + boundary;
	std::ostringstream head;
	head << xorstr("--").crypt_get() << boundary << xorstr("\r\n").crypt_get();
	head << xorstr("Content-Disposition: form-data; name=\"").crypt_get() << __formname << xorstr("\"; filename=\"").crypt_get() << __formfilename << xorstr("\"\r\n").crypt_get();
	head << xorstr("Content-Type: application/octet-stream\r\n").crypt_get();
	head << xorstr("Content-Transfer-Encoding: binary\r\n").crypt_get();
	head << xorstr("\r\n").crypt_get();
	static std::string tail = xorstr("\r\n--").crypt_get() + boundary + xorstr("--\r\n").crypt_get();

	CHAR __POST[] = { 'P', 'O', 'S', 'T', 0x0 }; // POST
	CHAR __HTTPtype[] = { 'H', 'T', 'T', 'P', '/', '1', '.', '1', 0x0 }; // HTTP/1.1
	CHAR __httpUseragent[] = { 'B', 'e', 't', 'a', 'S', 'h', 'i', 'e', 'l', 'd', 'V', '2', ' ', 'A', 'n', 't', 'i', 'c', 'h', 'e', 'a', 't', ' ', 'W', 'e', 'b', ' ', 'A', 'g', 'e', 'n', 't', 0x0 }; // BetaShieldV2 Anticheat Web Agent

	try {

		auto hInternet = g_winapiApiTable->InternetOpenA(__httpUseragent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hInternet == NULL) {
			DEBUG_LOG(LL_ERR, "InternetOpenA fail! Error code: %u", g_winapiApiTable->GetLastError());
			return false;
		}

		auto hConnect = g_winapiApiTable->InternetConnectA(hInternet, szHost.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
		if (hConnect == NULL) {
			DEBUG_LOG(LL_ERR, "InternetConnectA fail! Error code: %u", g_winapiApiTable->GetLastError());

			g_winapiApiTable->InternetCloseHandle(hInternet);
			return false;
		}

		auto hRequest = g_winapiApiTable->HttpOpenRequestA(hConnect, __POST, szUrl.c_str(), __HTTPtype, NULL, NULL,
			INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
			INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
			INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
			INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
			INTERNET_FLAG_NO_AUTH |
			INTERNET_FLAG_NO_CACHE_WRITE |
			INTERNET_FLAG_NO_UI |
			INTERNET_FLAG_PRAGMA_NOCACHE |
			INTERNET_FLAG_RELOAD, NULL);

		if (hRequest == NULL) {
			DEBUG_LOG(LL_ERR, "HttpOpenRequestA fail! Error code: %u", g_winapiApiTable->GetLastError());

			g_winapiApiTable->InternetCloseHandle(hConnect);
			g_winapiApiTable->InternetCloseHandle(hInternet);
			return false;
		}

		auto addReqHeaders = g_winapiApiTable->HttpAddRequestHeadersA(hRequest, hdrs.c_str(), -1, HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);
		if (addReqHeaders == FALSE) {
			DEBUG_LOG(LL_ERR, "HttpAddRequestHeadersA fail! Error code: %u", g_winapiApiTable->GetLastError());

			g_winapiApiTable->InternetCloseHandle(hRequest);
			g_winapiApiTable->InternetCloseHandle(hConnect);
			g_winapiApiTable->InternetCloseHandle(hInternet);
			return false;
		}

		INTERNET_BUFFERSA bufferIn = { 0 };
		bufferIn.dwStructSize = sizeof(bufferIn);
		bufferIn.dwBufferTotal = static_cast<DWORD>(head.str().size()) + static_cast<DWORD>(ulDataSize) + static_cast<DWORD>(tail.size());

		auto bSendReq = g_winapiApiTable->HttpSendRequestExA(hRequest, &bufferIn, NULL, HSR_INITIATE, 0);
		if (!bSendReq) {
			DEBUG_LOG(LL_ERR, "HttpSendRequestExA fail! Error code: %u", g_winapiApiTable->GetLastError());

			g_winapiApiTable->InternetCloseHandle(hRequest);
			g_winapiApiTable->InternetCloseHandle(hConnect);
			g_winapiApiTable->InternetCloseHandle(hInternet);
			return false;
		}

		DWORD bytesWritten = 0;
		if (
			FALSE == g_winapiApiTable->InternetWriteFile(hRequest, (const void*)head.str().c_str(), static_cast<DWORD>(head.str().size()), &bytesWritten) ||
			FALSE == g_winapiApiTable->InternetWriteFile(hRequest, (const void*)pData, static_cast<DWORD>(ulDataSize), &bytesWritten) ||
			FALSE == g_winapiApiTable->InternetWriteFile(hRequest, (const void*)tail.c_str(), static_cast<DWORD>(tail.size()), &bytesWritten)
		) {
			DEBUG_LOG(LL_ERR, "InternetWriteFile fail! Error code: %u", g_winapiApiTable->GetLastError());

			g_winapiApiTable->InternetCloseHandle(hRequest);
			g_winapiApiTable->InternetCloseHandle(hConnect);
			g_winapiApiTable->InternetCloseHandle(hInternet);
			return false;
		}

		if (g_winapiApiTable->HttpEndRequestA(hRequest, NULL, HSR_INITIATE, 0) == FALSE) {
			DEBUG_LOG(LL_ERR, "HttpEndRequestA fail! Error code: %u", g_winapiApiTable->GetLastError());

			g_winapiApiTable->InternetCloseHandle(hRequest);
			g_winapiApiTable->InternetCloseHandle(hConnect);
			g_winapiApiTable->InternetCloseHandle(hInternet);
			return false;
		}


		g_winapiApiTable->InternetCloseHandle(hRequest);
		g_winapiApiTable->InternetCloseHandle(hConnect);
		g_winapiApiTable->InternetCloseHandle(hInternet);
		DEBUG_LOG(LL_SYS, "File succesfully sended to server!");

		return true;
	}
	catch (std::exception& e) {
#ifndef _DEBUG
		UNREFERENCED_PARAMETER(e);
#endif
		DEBUG_LOG(LL_ERR, "Exception(std::exception) triggered on PostData. Info: %s", e.what());
		return false;
	}
	catch (DWORD dwError) {
#ifndef _DEBUG
		UNREFERENCED_PARAMETER(dwError);
#endif
		DEBUG_LOG(LL_ERR, "Exception triggered on PostData. Error: %u", dwError);
		return false;
	}
	catch (...) {
		DEBUG_LOG(LL_ERR, "Unhandled exception triggered on PostData");
		return false;
	}

	return false;
}


