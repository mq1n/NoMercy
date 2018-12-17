#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "NM_InitilizationManager.h"
#include "PipeManager.h"
#include "ExceptionHandlers.h"
#include "ManualMap.h"
#include "WinVerHelper.h"
#include "AntiDebug.h"
#include "ProcessFunctions.h"

// TODO: Update NoMercy.dll, Sys, Service, Launcher

void HelperAppMainRoutine()
{
	auto bCheckServiceStatus	= true;
	auto pServiceCheckTimer		= CTimer<std::chrono::milliseconds>();

	while (true)
	{
		if (pServiceCheckTimer.diff() > 35000 && bCheckServiceStatus) // Wait 35 sec after than helper initilization, If still have NOT working service app close helper process
		{
			auto szServiceName = g_nmApp->FunctionsInstance()->szLower(g_nmApp->InitMgrInstance()->m_szServiceName);
			if (CProcessFunctions::GetProcessCountFromProcessName(szServiceName.c_str()) == 0)
			{
				g_nmApp->OnCloseRequest(EXIT_ERR_SERVICE_NOT_WORKS, g_winapiApiTable->GetLastError());
				return;
			}
			bCheckServiceStatus = false;
		}

//		DEBUG_LOG(LL_SYS, "alive!!");
		Sleep(1000);
	}
}

bool CNM_App::OnServiceLog(int iLevel, const std::string & szContent)
{
	std::string szFunctionName = xorstr("[SERVICE] ").crypt_get();
	g_nmApp->LogHelperInstance()->sys_log(szFunctionName.c_str(), iLevel, szContent.c_str());

	return true;
}

bool CNM_App::OnServiceCloseRequestNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode)
{
	DEBUG_LOG(LL_SYS, "Service close request received!");
	return true;
}

bool CNM_App::OnHelperAppInit(LPDWORD pDwErrorCode) // TODO: Features(Manage updater, service log helper, faq page launcher, detect ui, tray icon)
{
	DEBUG_LOG(LL_SYS, "Helper app routine started!");

	// Common Veriables
	auto bRet					= false;
	auto iElevationRet			= 0;
	auto iSysMetrics			= 0;
	auto dwRemoteCheckErr		= 0UL;
	auto dwParentPID			= 0UL;
	auto szParentName			= std::string("");
	auto szLowerLauncherName	= m_nmFunctions->szLower(m_initMgr->m_szLauncherName);

	// Common Init routine
	if (g_nmApp->InitMgrInstance()->IsProcessProtected() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PROTECTION_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Protection check step completed!");

	iElevationRet = g_nmApp->InitMgrInstance()->CheckElevation();
	if (iElevationRet == 1)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_ELEVATION_FAIL_FIRST;
		goto _Complete;
	}
	else if (iElevationRet == 2)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_ELEVATION_FAIL_SECOND;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Elevation check step completed!");

	if (g_nmApp->InitMgrInstance()->RequestPrivilege(SE_DEBUG_PRIVILEGE) == false)
	{
#ifndef _DEBUG
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DEBUG_PRIV_FAIL;
		goto _Complete;
#endif
	}
	DEBUG_LOG(LL_SYS, "Request debug priv step completed!");

	if (CExceptionHandlers::InitExceptionHandlers() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_EXCEPTION_HANDLER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Exception handler setup step completed!");

	// Anti analysis
	CAntiDebug::AntiCuckoo();
	DEBUG_LOG(LL_SYS, "Anti cuckoo step completed!");

	// Remap windows modules
	if (m_nmManualMapHelper->RefreshModules() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_MMAP_RELOAD_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Manualmap refresh routine step completed!");

	// Check parent process
	dwParentPID = CProcessFunctions::GetProcessParentProcessId(g_winapiApiTable->GetCurrentProcessId());
	if (!dwParentPID)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_PID_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Parent process check step completed! Parent: %u", dwParentPID);

	szParentName = CProcessFunctions::GetProcessNameFromProcessId(dwParentPID);
	if (szParentName.empty())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_NAME_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Parent process check step completed! Parent: %s", szParentName.c_str());

#ifndef _DEBUG
	if (szLowerLauncherName != szParentName)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_CHECK_FAIL;
		goto _Complete;
	}
#endif
	DEBUG_LOG(LL_SYS, "Parent compare check step completed!");

	// OS Check
	if (IsWindowsXPOrGreater() == false || IsWindowsServer())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_UNSUPPORTED_OS;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 1 completed!");

	if (IsWindowsVistaOrGreater() == false && g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll, xorstr("NtCreateThreadEx").crypt_get()) /* Any not XP supported windows api*/)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_FAKE_OS;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 2 completed!");

	if (IsFakeConditionalVersion())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_FAKE_OS_VERSION;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 3 completed!");

	// Safe mode check
	if (m_nmFunctions->IsSafeModeEnabled())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SAFE_MODE;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Safe mode check step completed!");

	// Remote web connections
	if (g_nmApp->InitMgrInstance()->SafeRemoteCheck(&dwRemoteCheckErr) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_REMOTE_ERROR_BASE + dwRemoteCheckErr;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Remote check step completed!");

	// Create server pipe
	m_pHelperPipeManager = std::make_shared<CPipeManager>(g_nmApp->InitMgrInstance()->m_szHelperAppPipe, 100UL /* ms */);
	if (!IS_VALID_SMART_PTR(m_pHelperPipeManager))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_PIPE_CLASS_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe helper alloc step completed!");

	// Initialize pipe watcher thread
	if (m_pHelperPipeManager->CreateServerWatcherThread() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_PIPE_RECV_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe helper create routine step completed!");

	HelperAppMainRoutine();

	bRet = true;

_Complete:
	DEBUG_LOG(LL_SYS, "OnHelperAppInit routine completed!");
	return bRet;
}

