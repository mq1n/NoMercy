#pragma once

class CWindowEnumerator
{
	public:
		CWindowEnumerator() = default;
		~CWindowEnumerator() = default;

		HWND FindWindowWithClassName(const std::string & szClassName, bool bSubstrCheck);
		HWND FindWindowWithTitleName(const std::string & szTitleName, bool bSubstrCheck);

		std::vector <HWND> 	EnumerateWindows(DWORD dwOwnerPID);
		std::vector <HWND> 	EnumerateWindows();
};

