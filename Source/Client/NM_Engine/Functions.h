#pragma once

class CFunctions : public std::enable_shared_from_this <CFunctions>
{
	public:
		HMODULE GetCurrentModule();

		const char* GetTime();
		DWORD GetCurrentTimestamp();
		DWORD SystemTimeToTimestamp(SYSTEMTIME st);
		std::string GetDate();
		std::string FixBuildDate();

		void MessageBoxAf(HWND hWnd, DWORD dwTimeout, const char* c_szTitle, const char* c_szArgFormat, ...);
		DWORD ServiceMessageBox2(const std::string & szTitle, const std::string & szMessage, WORD wType = NULL);
		void CsrssMessageBox(const wchar_t* c_wszTitle, const wchar_t* c_wszMessage);
		void TrayBaloon(const wchar_t* c_wszTitle, const wchar_t* c_wszMessage);

		void OpenConsoleWindow();

		std::wstring UTF8ToWstring(const std::string & input);
		std::string WstringToUTF8(const std::wstring & input);

		std::string szLower(std::string szSource);
		std::wstring wszLower(std::wstring wszSource);

		std::string CreateString(const char* c_szFormat, ...);
		std::wstring CreateString(const wchar_t* c_wszFormat, ...);

		std::string DisplayError(DWORD dwErrorCode);
		std::string DisplaySystemError(DWORD dwErrorCode);

		bool IsMainWindow(HWND hWnd);
		HWND GetMainWindow(DWORD dwProcessID);
		std::string GetMainWindowTitle(DWORD dwProcessID);

		std::string GetFirstArgument(bool bLower, bool bFirst = true);
		std::string GetProcessCommandLine(bool bPassFirstArg = false);

		bool RunShadow(DWORD dwProcessId);
		void InitShadow(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
		bool KillShadow();
		bool CreateShadow(DWORD dwProtectedPID);
		DWORD FindOwnShadowProcessId();

		bool IsSysWow64();
		bool IsX64System();
		bool IsWow64Process(HANDLE hProcess);

		float GetEntropy(BYTE* byBuffer, DWORD dwLength);

		bool IsInModuleRange(HMODULE hModule, DWORD_PTR dwAddress);
		bool IsInModuleRange(const char* c_szModuleName, DWORD_PTR dwAddress);
		std::string GetModuleOwnerName(LPVOID pModuleBase);

		VOID AppendRandomData(PBYTE pBuffer, DWORD uBuffLen);
		LPBYTE GetRandomData(DWORD dwBuffLen);
		ULONG GetRandomInt(ULONG uMin, ULONG uMax);
		std::string GetRandomString(int iLength);

		std::string EncryptString(std::string szIn, const size_t size, BYTE byKey);
		std::string DecryptString(std::string szIn, const size_t size, BYTE byKey);

		DWORD GetStringHash(LPVOID lpBuffer, BOOL bUnicode, std::size_t uLen);

		bool InvokeBSOD();
		bool IsSafeModeEnabled();

	protected:
		int OpenConsoleWindowEx();
};

