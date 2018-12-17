#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "NM_InitilizationManager.h"
#include "WinInetAPI.h"
#include "WinVerHelper.h"
#include "ExceptionHandlers.h"

CNM_InitMgr::CNM_InitMgr() :
	m_szSplashHideOption(""), m_szOpenFaqPageOption(""), m_szProtectedAppOption("")
{
	m_szDriverID					= xorstr("NoMercy").crypt_get();
	m_szDriverDisplayName			= xorstr("NoMercySys").crypt_get();
	m_szDriverSymLink				= xorstr("\\\\.\\NoMercy").crypt_get();
	m_szServiceID					= xorstr("NoMercySvc").crypt_get();
	m_szServiceDisplayName			= xorstr("NoMercy Service").crypt_get();
	m_szServicePipe					= xorstr("NMServicePipe").crypt_get();
	m_szServiceMutex				= xorstr("Global\\{G4ATBCE5-7348-450a-GDSV-CVL5XBF12X3D}").crypt_get();
	m_szServiceName					= xorstr("NM_Service.exe").crypt_get();
	m_szLauncherName				= xorstr("NM_Launcher.exe").crypt_get();
	m_szHelperAppName				= xorstr("NM_HelperApp.exe").crypt_get();
	m_szHelperAppPipe				= xorstr("NMHelperAppPipe").crypt_get();
	m_szModuleName					= xorstr("NoMercy.dll").crypt_get();
	m_szDriverName_x86				= xorstr("NM_Sys_x86.sys").crypt_get();
	m_szDriverName_x64				= xorstr("NM_Sys_x64.sys").crypt_get();
	m_szLauncherConfigName			= xorstr("NM_Launcher.json").crypt_get();
	m_szRemoteWebURL				= xorstr("http://nomercy.org").crypt_get();
	m_szRemoteWebMainCheckURL		= xorstr("http://nomercy.org/canrun.txt").crypt_get();
	m_szRemoteWebLicenseCheckURL	= xorstr("http://nomercy.org/license.php").crypt_get();
	m_szRemoteWebChecksumCheckURL	= xorstr("http://nomercy.org/sum_list.json").crypt_get();

	m_vLaunchedProcesses.clear();
	m_vLauncherThreads.clear();
}

CNM_InitMgr::~CNM_InitMgr()
{
}


inline bool LicenseIDCheckRoutine(const std::string & szContent)
{
	// 2) Current license code is it allowed
	return true;
}

inline bool ChecksumCheckRoutine(const std::string & szContent)
{
	// 3) NM_Module's checksum
	// 4) NM_Service's checksum
	// 5) NM_Sys's checksum

	return true;
}

bool CNM_InitMgr::RemoteCheckRoutine(LPDWORD pdwErrorCode)
{
	if (!pdwErrorCode)
		return false;

	auto pInternetMgr = std::make_unique<CInternetAPI>();
	if (!IS_VALID_SMART_PTR(pInternetMgr))
	{
		*pdwErrorCode = REMOTE_ERR_INT_CLASS_ALLOC_FAIL;
		return false;
	}

	if (!pInternetMgr->CheckInternetConnection(m_szRemoteWebURL))
	{
		*pdwErrorCode = REMOTE_ERR_CHECK_CONNECTION_FAIL;
		return false;
	}

	auto dwWebStatus = 0UL;
	if (!pInternetMgr->WebStatusCheck(m_szRemoteWebURL, &dwWebStatus))
	{
		*pdwErrorCode = REMOTE_ERR_CHECK_STATUS_FAIL;
		return false;
	}

	if (dwWebStatus != 200)
	{
		*pdwErrorCode = REMOTE_ERR_UNKNOWN_WEB_STATUS;
		return false;
	}

	// Main

	auto szContent	= std::string("");
	auto ulSize		= std::size_t(0);

	if (!pInternetMgr->ReadUrl(m_szRemoteWebMainCheckURL, &szContent, &ulSize) || !ulSize || ulSize != szContent.size())
	{
		*pdwErrorCode = REMOTE_ERR_READ_MAIN_FAIL;
		return false;
	}

	if (!strcmp(szContent.c_str(), xorstr("FAIL").crypt_get()))
	{
		*pdwErrorCode = REMOTE_ERR_CHECK_MAIN_FAIL;
		return false;
	}

	szContent.clear();
	ulSize = 0;

	// License
	auto szLicenseURL = std::string(m_szRemoteWebLicenseCheckURL) + xorstr("?id=").crypt_get() + g_nmApp->DataInstance()->GetLicenseCode();
	if (!pInternetMgr->ReadUrl(szLicenseURL, &szContent, &ulSize) || !ulSize || ulSize != szContent.size())
	{
		*pdwErrorCode = REMOTE_ERR_READ_LICENSE_FAIL;
		return false;
	}

	if (LicenseIDCheckRoutine(szContent) == false)
	{
		*pdwErrorCode = REMOTE_ERR_CHECK_LICENSE_FAIL;
		return false;
	}

	szContent.clear();
	ulSize = 0;

	// Checksum
	if (!pInternetMgr->ReadUrl(m_szRemoteWebChecksumCheckURL, &szContent, &ulSize) || !ulSize || ulSize != szContent.size())
	{
		*pdwErrorCode = REMOTE_ERR_READ_CHECKSUM_FAIL;
		return false;
	}

	if (ChecksumCheckRoutine(szContent) == false)
	{
		*pdwErrorCode = REMOTE_ERR_CHECK_CHECKSUM_FAIL;
		return false;
	}

	return true;
}

bool CNM_InitMgr::SafeRemoteCheck(LPDWORD pdwErrorCode)
{
	auto bRet = false;
	__try
	{
		bRet = RemoteCheckRoutine(pdwErrorCode);
	}
	__except (CExceptionHandlers::OnExceptionThrowed(GetExceptionInformation()))
	{
//		Logf(CUSTOM_LOG_FILENAME, xorstr("Remote check fail! Error: %p\n").crypt_get(), dwExceptionCode);
		if (pdwErrorCode) *pdwErrorCode = INIT_ERR_REMOTE_EXCEPTION_BASE + GetExceptionCode();
	}
	return bRet;
}

// ------

bool CNM_InitMgr::IsProcessProtected()
{
#if !defined(_DEBUG)
#if USE_ENIGMA_SDK == 1
	if (EP_CheckupIsProtected() && EP_CheckupIsEnigmaOk() == FALSE)
#elif USE_VMPROTECT_SDK == 1
	if (VMProtectIsValidImageCRC() == FALSE)
#elif USE_SHIELDEN_SDK == 1
	if (SECheckProtection() == FALSE)
#else
	if (false)
#endif
	{
		DEBUG_LOG(LL_CRI, "Protection is changed/cracked...");
		return false;
	}
#endif

	return true;
}

int CNM_InitMgr::CheckElevation()
{
#ifndef _DEBUG
	try {
		if (g_nmApp->AccessHelperInstance()->IsProcessElevated() == FALSE && g_nmApp->DataInstance()->GetAppType() != NM_TESTAPP)
		{
//			m_nmFunctions->CloseProcess(xorstr("Process can not elevated!").crypt_get(), false, "");
			return 1;
		}
	}
	catch (DWORD dwError)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("IsProcessElevated failed w/err: %p\n").crypt_get(), dwError);
	}
#endif

#ifndef _DEBUG
	try {
		if (g_nmApp->AccessHelperInstance()->IsRunAsAdmin() == FALSE && g_nmApp->DataInstance()->GetAppType() != NM_TESTAPP)
		{
//			m_nmFunctions->CloseProcess(xorstr("Please run this process as administrator").crypt_get(), false, "");
			return 2;
		}
	}
	catch (DWORD dwError)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("IsRunAsAdmin failed w/err: %p\n").crypt_get(), dwError);
	}
#endif

	return 0;
}

bool CNM_InitMgr::RequestPrivilege(ULONG ulPriv)
{
	if (IsWindowsVistaOrGreater() && g_nmApp->DataInstance()->GetAppType() != NM_TESTAPP)
	{
		BOOLEAN bPrevStatus;
		auto ntStat = g_winapiApiTable->RtlAdjustPrivilege(ulPriv, TRUE, FALSE, &bPrevStatus);

		DEBUG_LOG(LL_SYS, "RtlAdjustPrivilege ret: %p", ntStat);
		if (!NT_SUCCESS(ntStat))
		{
			Logf(CUSTOM_LOG_FILENAME, xorstr("Process elevation fail! Status: %p Prev: %d\n").crypt_get(), ntStat, bPrevStatus);
			return false;
		}
	}
	return true;
}

bool CNM_InitMgr::RestartAsAdmin()
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	DWORD dwPathSize = MAX_PATH;
	if (g_winapiApiTable->QueryFullProcessImageNameW(GetCurrentProcess(), 0, wszPath, &dwPathSize) == FALSE)
		return false;

	auto wszParam = xorstr(L"runas").crypt_get();
	auto iExecuteRet = (INT_PTR)g_winapiApiTable->ShellExecuteA(NULL, wszParam, wszPath, NULL, NULL, SW_SHOW);
	if (iExecuteRet > 32) // If the function succeeds, it returns a value greater than 32.
		return true;
	
	return false;
}

