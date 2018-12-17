#include "NM_Index.h"
#include "NM_Main.h"
#include "Functions.h"
#include "DynamicWinapi.h"
#include "Defines.h"
#include "Terminator.h"
#include "BasicCrypt.h"
#include "ProcessFunctions.h"
#include "WindowEnumerator.h"

#include <boost\algorithm\string\predicate.hpp>
#include <boost\algorithm\string\replace.hpp>
#include <lazy_importer.hpp>
#include <Wtsapi32.h>


static void __DummyFunc()
{
}

HMODULE CFunctions::GetCurrentModule()
{
	HMODULE hModule = NULL;
	LI_FIND(GetModuleHandleEx)(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)__DummyFunc, &hModule);
	return hModule;
}

const char* CFunctions::GetTime()
{
	time_t rawtime = { 0 };
	time(&rawtime);

	struct tm* timeinfo = { 0 };
	timeinfo = localtime(&rawtime);

	return asctime(timeinfo);
}

DWORD CFunctions::GetCurrentTimestamp()
{
	time_t curTime = { 0 };
	time(&curTime);
	return (DWORD)curTime;
}

DWORD CFunctions::SystemTimeToTimestamp(SYSTEMTIME st)
{
	std::tm tm = { 0 };
	tm.tm_sec = st.wSecond;
	tm.tm_min = st.wMinute;
	tm.tm_hour = st.wHour;
	tm.tm_mday = st.wDay;
	tm.tm_mon = st.wMonth - 1;
	tm.tm_year = st.wYear - 1900;
	tm.tm_isdst = -1;

	std::time_t fileCreatedTime = std::mktime(&tm);
	return (DWORD)fileCreatedTime;
}

std::string CFunctions::GetDate()
{
	SYSTEMTIME sysTime = { 0 };
	LI_FIND(GetSystemTime)(&sysTime);

	char szTimeBuf[1024];
	sprintf(szTimeBuf, xorstr("%02d:%02d:%02d - %02d:%02d:%d").crypt_get(), sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wDay, sysTime.wMonth, sysTime.wYear);
	return szTimeBuf;
}

void CFunctions::MessageBoxAf(HWND hWnd, DWORD dwTimeout, const char* c_szTitle, const char* c_szArgFormat, ...)
{
	char szTmpString[8096] = { 0 };

	va_list vaArgList;
	va_start(vaArgList, c_szArgFormat);
	vsprintf(szTmpString, c_szArgFormat, vaArgList);
	va_end(vaArgList);

	if (dwTimeout)
		g_winapiApiTable->MessageBoxTimeout(hWnd, szTmpString, c_szTitle, NULL, 0, dwTimeout);
	else
		g_winapiApiTable->MessageBoxA(hWnd, szTmpString, c_szTitle, NULL);
}

DWORD CFunctions::ServiceMessageBox2(const std::string & szTitle, const std::string & szMessage, WORD wType)
{
	auto bRet		= false;
	auto dwResponse = 0UL;

	bRet = WTSSendMessageA(
		WTS_CURRENT_SERVER_HANDLE,							// hServer
		WTSGetActiveConsoleSessionId(),						// ID for the console seesion (1)
		const_cast<LPSTR>(szTitle.c_str()),					// MessageBox Caption
		static_cast<DWORD>(strlen(szTitle.c_str())),		// Length of caption
		const_cast<LPSTR>(szMessage.c_str()),				// MessageBox Text
		static_cast<DWORD>(strlen(szMessage.c_str())),		// Length of text
		wType,												// Buttons, etc
		0,													// Timeout period in seconds (0 = infinite)
		&dwResponse,										// What button was clicked (if bWait == TRUE)
		FALSE												// bWait - Blocks until user click
	);

	// TODO: Check return value and implement other methods if current one fail
	// way 2 WinStationSendMessageW(0,currtokensessid,title,titlesize,msg,msgsize,type,-1,responseptr,0)
	// way 3 msi.dll\MsiMessageBox(wnd,text,title,options,0)
	// way 4 shlwapi\ShellMessageBox(0,wnd,text,title,options)
	// way 5 user32-SoftModalMessageBox
	// way 6 Shlwapi\SHMessageBoxCheck(wnd,text,title,options,0,"")
	// way 7 MessageBoxIndirect
	// way 8 NtRaiseHardError
	// way 9 messageboxtimeout

	return dwResponse;
}

#ifdef _DEBUG
int CFunctions::OpenConsoleWindowEx()
{
	if (!AllocConsole())
		return 0;

	freopen("CONOUT$", "a", stdout);
	freopen("CONIN$", "r", stdin);

#if 0
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!h)
		return -2;

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	if (!GetConsoleScreenBufferInfo(h, &csbiInfo))
		return -3;
	if (!SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY))
		return -4;

	Sleep(120);
#endif

	auto szTitle = CreateString("NoMercy debug console | PID: %u", GetCurrentProcessId());
	if (!SetConsoleTitleA(szTitle.c_str()))
		return -5;

	return 0;
}

void CFunctions::OpenConsoleWindow()
{
	int iRet = OpenConsoleWindowEx();
	if (iRet < 0)
	{
		DEBUG_LOG(LL_CRI, "Console can not opened! Error code: %d", iRet);
		abort();
	}
}
#endif


std::wstring CFunctions::UTF8ToWstring(const std::string & input)
{
	wchar_t buf[8192] = { 0 };
	g_winapiApiTable->MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), buf, ARRAYSIZE(buf));
	return buf;
}

std::string CFunctions::WstringToUTF8(const std::wstring & input)
{
	char buf[8192] = { 0 };
	g_winapiApiTable->WideCharToMultiByte(CP_UTF8, 0, input.c_str(), (int)input.length(), buf, ARRAYSIZE(buf), nullptr, nullptr);
	return buf;
}

std::string CFunctions::szLower(std::string szSource)
{
	std::string szLowerCopy(szSource.begin(), szSource.end());
	std::transform(szLowerCopy.begin(), szLowerCopy.end(), szLowerCopy.begin(), tolower);
	return szLowerCopy;
}

std::wstring CFunctions::wszLower(std::wstring wszSource)
{
	std::wstring wszLowerCopy(wszSource.begin(), wszSource.end());
	std::transform(wszLowerCopy.begin(), wszLowerCopy.end(), wszLowerCopy.begin(), tolower);
	return wszLowerCopy;
}

std::string CFunctions::CreateString(const char* c_szFormat, ...)
{
	char pTmpString[8096] = { 0 };

	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf(pTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	return pTmpString;
}

std::wstring CFunctions::CreateString(const wchar_t* c_wszFormat, ...)
{
	wchar_t pTmpString[8096] = { L'\0' };

	va_list vaArgList;
	va_start(vaArgList, c_wszFormat);
	wvsprintf(pTmpString, c_wszFormat, vaArgList);
	va_end(vaArgList);

	return pTmpString;
}

std::string CFunctions::DisplayError(DWORD dwErrorCode)
{
	CHAR szErrorMessage[4096] = { 0 };

	auto dwFlags = DWORD(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK);
	g_winapiApiTable->FormatMessageA(dwFlags, NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szErrorMessage, sizeof(szErrorMessage) / sizeof(TCHAR), NULL);
	
	return szErrorMessage;
}

std::string CFunctions::DisplaySystemError(DWORD dwErrorCode)
{
	CHAR szErrorMessage[4096] = { 0 };

	auto dwFlags = DWORD(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK);
	auto dwDosErr = g_winapiApiTable->RtlNtStatusToDosError(dwErrorCode);
	g_winapiApiTable->FormatMessageA(dwFlags, g_winapiModuleTable->hNtdll, dwDosErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szErrorMessage, sizeof(szErrorMessage) / sizeof(TCHAR), NULL);

	return szErrorMessage;
}

bool CFunctions::IsMainWindow(HWND hWnd)
{
	if (g_winapiApiTable->IsWindowVisible(hWnd))
		return (g_winapiApiTable->GetWindow(hWnd, GW_OWNER) == (HWND)0);
	return false;
}

HWND CFunctions::GetMainWindow(DWORD dwProcessID)
{
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator)) 
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return nullptr;
	}

	auto vWindows = windowEnumerator->EnumerateWindows(dwProcessID);
	if (vWindows.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Can not found any window for: %u", dwProcessID);
		return nullptr;
	}

	for (const auto & hWnd : vWindows)
	{
		if (g_winapiApiTable->IsWindowVisible(hWnd))
		{
#ifdef _DEBUG
			char szTitle[MAX_PATH] = { 0x0 };
			g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH);
			if (strstr(szTitle, "debug "))
				continue;
#endif

			if (IsMainWindow(hWnd))
				return hWnd;
		}
	}
	return nullptr;
}

std::string CFunctions::GetMainWindowTitle(DWORD dwProcessID)
{
	std::string szOutput = "";

	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator))
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return szOutput;
	}

	auto vWindows = windowEnumerator->EnumerateWindows(dwProcessID);
	if (vWindows.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Can not found any window for: %u", dwProcessID);
		return szOutput;
	}

	char szTitle[MAX_PATH] = { 0x0 };
	for (const auto & hWnd : vWindows)
	{
		if (g_winapiApiTable->IsWindowVisible(hWnd))
		{
			g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH);

			if (IsMainWindow(hWnd) && szTitle)
			{
				szOutput = szTitle;
				break;
			}
		}
	}
	return szOutput;
}

std::string CFunctions::GetProcessCommandLine(bool bPassFirstArg)
{
	std::string szOutput = "";

	int iArgCount = 0;
	auto wcArgs = g_winapiApiTable->CommandLineToArgvW(g_winapiApiTable->GetCommandLineW(), &iArgCount);
	if (!iArgCount)
		return szOutput;

	for (int i = bPassFirstArg ? 1 : 0; i < iArgCount; ++i)
	{
		std::wstring wszArgLaunch = wcArgs[i];
		std::string szArgLaunch(wszArgLaunch.begin(), wszArgLaunch.end());

		szOutput += szArgLaunch;
		szOutput += " ";
	}

	return szOutput;
}

std::string CFunctions::GetFirstArgument(bool bLower, bool bFirst)
{
	int iArgCount = 0;
	auto wcArgs = g_winapiApiTable->CommandLineToArgvW(g_winapiApiTable->GetCommandLineW(), &iArgCount);
	if (!iArgCount)
		return std::string("");

	std::wstring wszArgLaunch = wcArgs[bFirst ? 0 : 1];
	auto szArgLaunch = std::string(wszArgLaunch.begin(), wszArgLaunch.end());

	if (szArgLaunch.empty())
		return std::string("");

	if (bLower)
		return szLower(szArgLaunch);
	return szArgLaunch;
}

bool CFunctions::IsSysWow64()
{
#ifdef _WIN64
	return false;
#else
	return ((DWORD)__readfsdword(0xC0) != 0);
#endif
}

bool CFunctions::IsX64System()
{
	SYSTEM_INFO SysInfo = { 0 };;
	g_winapiApiTable->GetNativeSystemInfo(&SysInfo);

	return (SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64);
}

bool CFunctions::IsWow64Process(HANDLE hProcess)
{
	BOOL bRet = FALSE;
	if (!g_winapiApiTable->IsWow64Process(hProcess, &bRet) || !bRet)
		return false;

	return true;
}

float CFunctions::GetEntropy(BYTE* byBuffer, DWORD dwLength)
{
	DWORD dwSize = 0;
	long lBuff[0xFF + 1] = { 0 };
	float fTemp, fEntropy = 0;

	for (DWORD i = 0; i < dwLength; i++)
	{
		lBuff[byBuffer[i]]++;
		dwSize++;
	}

	for (DWORD i = 0; i < 256; i++)
	{
		if (lBuff[i])
		{
			fTemp = (float)lBuff[i] / (float)dwSize;
			fEntropy += (-fTemp * log2(fTemp));
		}
	}

	return fEntropy;
}

bool CFunctions::IsInModuleRange(HMODULE hModule, DWORD_PTR dwAddress)
{
	bool bRet = false;

	MODULEINFO mi = { 0 };
	if (g_winapiApiTable->GetModuleInformation(NtCurrentProcess, hModule, &mi, sizeof(mi)))
	{
		auto dwBase	= reinterpret_cast<DWORD_PTR>(mi.lpBaseOfDll);
		auto dwHi	= reinterpret_cast<DWORD_PTR>(mi.lpBaseOfDll) + mi.SizeOfImage;

		bRet		= (dwAddress >= dwBase && dwAddress <= dwHi);
	}
	return bRet;
}

bool CFunctions::IsInModuleRange(const char* c_szModuleName, DWORD_PTR dwAddress)
{
	bool bRet = false;

	MODULEINFO mi = { 0 };
	if (g_winapiApiTable->GetModuleInformation(NtCurrentProcess, g_winapiApiTable->GetModuleHandleA(c_szModuleName), &mi, sizeof(mi)))
	{
		auto dwBase = reinterpret_cast<DWORD_PTR>(mi.lpBaseOfDll);
		auto dwHi	= reinterpret_cast<DWORD_PTR>(mi.lpBaseOfDll) + mi.SizeOfImage;

		bRet		= (dwAddress >= dwBase && dwAddress <= dwHi);
	}
	return bRet;
}

std::string CFunctions::FixBuildDate()
{
	char pFixDate[512] = { 0 };
	sprintf(pFixDate, xorstr("%s-%s").crypt_get(), xorstr(__DATE__).crypt_get(), xorstr(__TIME__).crypt_get());

	std::string szFixDate(pFixDate);
	boost::replace_all(szFixDate, xorstr(":").crypt_get(), "");
	boost::replace_all(szFixDate, xorstr(" ").crypt_get(), "");
	return szFixDate;
}

VOID CFunctions::AppendRandomData(PBYTE pBuffer, DWORD uBuffLen)
{
	static BOOL uFirstTime = TRUE;

	if (uFirstTime) 
	{
		srand(g_winapiApiTable->GetTickCount());
		uFirstTime = FALSE;
	}

	for (DWORD i = 0; i < uBuffLen; i++)
		pBuffer[i] = rand();
}
LPBYTE CFunctions::GetRandomData(DWORD dwBuffLen)
{
	LPBYTE lpBuffer = (LPBYTE)malloc(dwBuffLen);
	AppendRandomData(lpBuffer, dwBuffLen);
	return lpBuffer;
}
ULONG CFunctions::GetRandomInt(ULONG uMin, ULONG uMax)
{
	if (uMax < (ULONG)0xFFFFFFFF)
		uMax++;

	return (rand() % (uMax - uMin)) + uMin;
}
std::string CFunctions::GetRandomString(int iLength)
{
	CHAR __alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0x0 }; // abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890
	static std::string charset = __alphabet;
	std::string result;
	result.resize(iLength);

	for (int i = 0; i < iLength; i++)
		result[i] = charset[rand() % charset.length()];

	return result;
}

std::string CFunctions::EncryptString(std::string szIn, const size_t size, BYTE byKey)
{
	BYTE pData[1024] = { 0 };
	strcpy((char*)pData, szIn.c_str());

	EncryptBuffer(pData, size, byKey);

	std::string szOut = std::string(reinterpret_cast<const char*>(pData));
	return szOut;
}
std::string CFunctions::DecryptString(std::string szIn, const size_t size, BYTE byKey)
{
	BYTE pData[1024] = { 0 };
	strcpy((char*)pData, szIn.c_str());

	DecryptBuffer(pData, size, byKey);

	std::string szOut = std::string(reinterpret_cast<const char*>(pData));
	return szOut;
}

DWORD CFunctions::GetStringHash(LPVOID lpBuffer, BOOL bUnicode, std::size_t uLen)
{
	DWORD dwHash = 0;
	LPSTR strBuffer = (LPSTR)lpBuffer;

	while (uLen--)
	{
		dwHash = (dwHash >> 13) | (dwHash << 19);
		dwHash += (DWORD)*strBuffer++;

		if (bUnicode)
			strBuffer++;
	}
	return dwHash;
}

bool CFunctions::InvokeBSOD()
{
	BOOLEAN bBuf;
	auto ntStatus = g_winapiApiTable->RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, true, false, &bBuf);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "RtlAdjustPrivilege fail! Status: %p", ntStatus);
		return false;
	}

	HARDERROR_RESPONSE response;
	ntStatus = g_winapiApiTable->NtRaiseHardError(STATUS_ASSERTION_FAILURE, NULL, NULL, nullptr, OptionAbortRetryIgnore, &response);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "NtRaiseHardError fail! Status: %p", ntStatus);
		return false;
	}
	return true;
}

std::string CFunctions::GetModuleOwnerName(LPVOID pModuleBase)
{
	char cFileName[2048] = { 0 };
	if (!g_winapiApiTable->GetMappedFileNameA(NtCurrentProcess, pModuleBase, cFileName, 2048))
		return std::string("");

	std::string szFileName = cFileName;
	auto szRealName = CProcessFunctions::DosDevicePath2LogicalPath(cFileName);
	if (szRealName.empty())
		return std::string("");

	auto szLowerName = g_nmApp->FunctionsInstance()->szLower(szRealName);
	return szLowerName;
}

bool CFunctions::IsSafeModeEnabled()
{
	int iSysMetrics = g_winapiApiTable->GetSystemMetrics(SM_CLEANBOOT);
	return (iSysMetrics > 0);
}

