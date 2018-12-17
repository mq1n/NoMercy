#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Hooks.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"

CSelfApiHooks::CSelfApiHooks() :
	m_bHooksIsInitialized(false), m_pNktHook(nullptr)
{
}


bool CSelfApiHooks::BlockAPI(LPCSTR lpModule, LPCSTR lpFuncName, int iType)
{
	HOOK_LOG(LL_SYS, "%s!%s api block initilization has been start!", lpModule, lpFuncName);

#ifdef _M_X64
	auto dwAddr = 0ULL;
#else
	auto dwAddr = 0UL;
#endif
	if (!strcmp(xorstr("python").crypt_get(), lpModule))
		dwAddr = (DWORD_PTR)g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hPython, lpFuncName);
	else
		dwAddr = (DWORD_PTR)g_winapiApiTable->GetProcAddress_o(g_winapiApiTable->GetModuleHandleA_o(lpModule), lpFuncName);

	if (!dwAddr)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_PATCH_API_NOT_FOUND, g_winapiApiTable->GetLastError(), (void*)lpFuncName);
		return false;
	}

	if (iType == RET_HOOK)
	{
		BYTE ret[6] = { 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3 };

		auto dwOldProtection = 0UL;
		if (!g_winapiApiTable->VirtualProtect(reinterpret_cast<LPVOID>(dwAddr), sizeof(ret), PAGE_EXECUTE_READWRITE, &dwOldProtection))
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_PATCH_API_1_VP_1_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		memcpy((void*)(dwAddr), &ret, sizeof(ret));

		if (!g_winapiApiTable->VirtualProtect(reinterpret_cast<LPVOID>(dwAddr), sizeof(ret), dwOldProtection, &dwOldProtection))
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_PATCH_API_1_VP_2_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}
	}
	else if (iType == NOP_HOOK)
	{
		BYTE ret[1] = { 0x90 };

		auto dwOldProtection = 0UL;
		if (!g_winapiApiTable->VirtualProtect(reinterpret_cast<LPVOID>(dwAddr), sizeof(ret), PAGE_EXECUTE_READWRITE, &dwOldProtection))
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_PATCH_API_2_VP_1_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		memcpy((void*)(dwAddr), &ret, sizeof(ret));

		if (!g_winapiApiTable->VirtualProtect(reinterpret_cast<LPVOID>(dwAddr), sizeof(ret), dwOldProtection, &dwOldProtection))
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_PATCH_API_2_VP_2_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}
	}

	HOOK_LOG(LL_SYS, "%s!%s succesfuly banned! Type: %d", lpModule, lpFuncName, iType);
	return true;
}


bool CSelfApiHooks::HooksIsInitialized()
{
	return m_bHooksIsInitialized;
}

bool CSelfApiHooks::InitializeSelfAPIHooks()
{
	auto szPID					= std::to_string(g_winapiApiTable->GetCurrentProcessId());
	std::string szLoggerName	= xorstr("NoMercy_Hook").crypt_get();
	auto szLoggerFileName		= xorstr("NoMercy/").crypt_get() + szLoggerName + xorstr("_").crypt_get() + szPID + xorstr(".log").crypt_get();

#ifdef _DEBUG
	m_logHelper = std::make_shared<CLogger>(szLoggerName, szLoggerFileName.c_str());
	if (!IS_VALID_SMART_PTR(m_logHelper)) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_LOGGER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	if (InitializePatchs() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (InitializeDetours() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	m_bHooksIsInitialized = true;
	return true;
}

