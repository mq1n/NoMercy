#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Defines.h"

bool CNM_App::GetScreenProtectionStatus(HWND hWnd)
{
	if (m_mScreenProtectionStatus.find(hWnd) != m_mScreenProtectionStatus.end())
		return m_mScreenProtectionStatus[hWnd];
	return false;
}

bool CNM_App::ActivateScreenProtection(HWND hWnd, bool bEnabled)
{
	auto bRet = g_winapiApiTable->SetWindowDisplayAffinity(hWnd, bEnabled ? WDA_MONITOR : WDA_NONE);
	DEBUG_LOG(LL_SYS, "hWnd: %p New status: %d Protection ret: %d Last error: %u", hWnd, bEnabled, bRet, g_winapiApiTable->GetLastError());

	m_mScreenProtectionStatus[hWnd] = bEnabled;
	return bRet;
}

void CNM_App::CheckScreenProtection(HWND hWnd)
{
	if (!GetScreenProtectionStatus(hWnd))
		return;

	auto dwAffinity = 0UL;
	auto bRet = g_winapiApiTable->GetWindowDisplayAffinity(hWnd, &dwAffinity);
	DEBUG_LOG(LL_SYS, "hWnd: %p Current protection status ret: %d affinity: %u", bRet, dwAffinity);

	if (!bRet)
		return;

//	while (1)
	{
		if (dwAffinity != WDA_MONITOR)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_SCREEN_PROTECTION_STATUS_CORRUPTED, g_winapiApiTable->GetLastError());
			return;
		}
	}
}

void CNM_App::InitScreenProtection(HWND hWnd)
{
	if (!hWnd || !g_winapiApiTable->IsWindow(hWnd))
	{
		DEBUG_LOG(LL_ERR, "Target window: %p is NOT valid!", hWnd);
		return;
	}

	char szClass[MAX_PATH] = { 0x0 };
	g_winapiApiTable->GetClassNameA(hWnd, szClass, MAX_PATH);

	char szTitle[MAX_PATH] = { 0x0 };
	g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH);

	DEBUG_LOG(LL_SYS, "Target window found! %p | %s-%s", hWnd, szTitle, szClass);

	ActivateScreenProtection(hWnd, true);
}
