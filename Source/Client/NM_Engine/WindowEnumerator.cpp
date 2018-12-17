#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "WindowEnumerator.h"
#include "DynamicWinAPI.h"
#include "VersionHelpers.h"
#include "Defines.h"

HWND CWindowEnumerator::FindWindowWithClassName(const std::string & szClassName, bool bSubstrCheck)
{
	HWND hTargetWnd = nullptr;
	if (szClassName.empty())
		return hTargetWnd;

	auto vWindowList = EnumerateWindows();
	for (auto & hWnd : vWindowList)
	{
		char szClass[MAX_PATH] = { 0x0 };
		g_winapiApiTable->GetClassNameA(hWnd, szClass, MAX_PATH);

		if  ( (!bSubstrCheck && !strcmp(szClass, szClassName.c_str())) ||
			  (bSubstrCheck  && strstr(szClass, szClassName.c_str()))
			)
		{
			hTargetWnd = hWnd;
		}
	}

	return hTargetWnd;
}

HWND CWindowEnumerator::FindWindowWithTitleName(const std::string & szTitleName, bool bSubstrCheck)
{
	HWND hTargetWnd = nullptr;
	if (szTitleName.empty())
		return hTargetWnd;

	auto vWindowList = EnumerateWindows();
	for (auto & hWnd : vWindowList)
	{
		char szTitle[MAX_PATH] = { 0x0 };
		g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH);

		if  ( (!bSubstrCheck && !strcmp(szTitle, szTitleName.c_str())) ||
			  (bSubstrCheck  && strstr(szTitle, szTitleName.c_str()))
			)
		{
			hTargetWnd = hWnd;
		}
	}

	return hTargetWnd;
}

std::vector <HWND> CWindowEnumerator::EnumerateWindows(DWORD dwOwnerPID)
{
	auto vWindowList = std::vector<HWND>();

	HWND hWnd = nullptr;
	do
	{
		hWnd = g_winapiApiTable->FindWindowExA(NULL, hWnd, NULL, NULL);

		DWORD dwPID = 0;
		g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwPID);

		if (dwPID == dwOwnerPID)
			vWindowList.push_back(hWnd);

	} while (hWnd);

	return vWindowList;
}

std::vector <HWND> CWindowEnumerator::EnumerateWindows()
{
	auto vWindowList = std::vector<HWND>();

	HWND hWnd = nullptr;
	do
	{
		hWnd = g_winapiApiTable->FindWindowExA(NULL, hWnd, NULL, NULL);
		vWindowList.push_back(hWnd);
	} while (hWnd);

	return vWindowList;
}
