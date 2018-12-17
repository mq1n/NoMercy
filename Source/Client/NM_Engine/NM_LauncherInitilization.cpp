#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "NM_InitilizationManager.h"
#include "ExceptionHandlers.h"
#include "ManualMap.h"
#include "ProcessFunctions.h"
#include "PipeManager.h"
#include "ServiceHelper.h"
#include "ServiceCommunication.h"
#include "SafeProcessHandle.h"
#include "AntiDebug.h"
#include "WindowUI.h"

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

inline bool ParseLauncherConfig()
{
	try
	{
		auto szDefaultValue = std::string("unknown");

		boost::property_tree::ptree root;
		boost::property_tree::read_json(g_nmApp->InitMgrInstance()->m_szLauncherConfigName, root);

		auto szCurrentStage = root.get<std::string>("stage", "dev");
		if (szCurrentStage.empty() || szCurrentStage == szDefaultValue)
			return false;

		auto stageTree = root.get_child(szCurrentStage);
		if (stageTree.empty())
			return false;

		g_nmApp->InitMgrInstance()->m_szSplashHideOption = stageTree.get<std::string>("hide_splash_screen", szDefaultValue);
		if (g_nmApp->InitMgrInstance()->m_szSplashHideOption.empty() || g_nmApp->InitMgrInstance()->m_szSplashHideOption == szDefaultValue)
			return false;

		g_nmApp->InitMgrInstance()->m_szOpenFaqPageOption = stageTree.get<std::string>("open_faq_page", szDefaultValue);
		if (g_nmApp->InitMgrInstance()->m_szOpenFaqPageOption.empty() || g_nmApp->InitMgrInstance()->m_szOpenFaqPageOption == szDefaultValue)
			return false;

		g_nmApp->InitMgrInstance()->m_szProtectedAppOption = stageTree.get<std::string>("protected_app", szDefaultValue);
		if (g_nmApp->InitMgrInstance()->m_szProtectedAppOption.empty() || g_nmApp->InitMgrInstance()->m_szProtectedAppOption == szDefaultValue)
			return false;

		// TODO: use command line parameters

		// TODO: extra command line parameters
	}
	catch (const std::exception & e)
	{
#ifdef _DEBUG
		DEBUG_LOG(LL_ERR, "Exception handled on parse json, Error: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif
		return false;
	}
	return true;
}

// -------------------------

typedef struct _UI_THREAD_PARAM
{
	gui::window * pWndPtr;
	bool * pbCompleted;
} SUIThreadParam, *PUIThreadParam;

bool CNM_App::OnLauncherInit(bool bProtected, LPDWORD pDwErrorCode)
{
	DEBUG_LOG(LL_SYS, "Launcher routine started!");

	// CreateProcess veriables
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA		si = { 0 };
	si.cb				= sizeof(si);

	// Common Veriables
	auto bRet						= false;
	auto bIsServiceRunning			= false;
	auto bIsHelperRunning			= false;
	auto bReCheckServiceLoad		= false;
	auto bReCheckServiceStart		= false;
	auto bServicePipeInitCompleted	= false;
	auto bHelperPipeInitCompleted	= false;
	auto bCompletedUIRoutine		= false;
	auto pServicePipeManager		= std::shared_ptr<CPipeManager>();
	auto pHelperPipeManager			= std::shared_ptr<CPipeManager>();
	auto pServiceHelper				= std::unique_ptr<CServiceHelper>();
	auto pUIThreadTimer				= CTimer<std::chrono::milliseconds>();
	auto szLowerServiceName			= std::string("");
	auto szLowerHelperAppName		= std::string("");
	auto szServicePath				= std::string("");
	auto szCurrentPath				= std::string("");
	auto szServiceMutex				= std::string("");
	auto iFindAttemptCount			= 0;
	auto iPipeOpenAttemptCount		= 0;
	auto iElevationRet				= 0;
	auto iSysMetrics				= 0;
	auto dwLogoSum					= 0UL;
	auto dwRemoteCheckErr			= 0UL;
	auto dwDriverHelperErr			= 0UL;
	auto dwServiceStatus			= 0UL;

	// Shared veriables
	static auto s_szParams			= m_nmFunctions->GetProcessCommandLine(true);

	// Init UI
	auto LauncherUIRoutine = [](LPVOID lpParam) -> DWORD 
	{
		auto param = reinterpret_cast<SUIThreadParam*>(lpParam);
		if (!param)
			return 0;

		auto pLauncherWnd = gui::window::singleton();
		if (!pLauncherWnd)
		{
			DEBUG_LOG(LL_CRI, "UI Thread class initilization fail!");

			g_nmApp->OnCloseRequest(EXIT_ERR_UI_CLASS_ALLOC_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}
		pLauncherWnd->set_instance(g_nmApp->DataInstance()->GetMainWnd());
		param->pWndPtr = pLauncherWnd;

		if (!pLauncherWnd->assemble(xorstr("nm_updater_class").crypt_get(), xorstr("nm_updater").crypt_get(), gui::rectangle(0, 0, 290, 118), MAKEINTRESOURCEA(NM_ICON_IMAGE)))
		{
			DEBUG_LOG(LL_CRI, "UI Thread assemble initilization fail!");
			
			g_nmApp->OnCloseRequest(EXIT_ERR_UI_CLASS_INIT_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}
		if (param->pbCompleted) *param->pbCompleted = true;

		gui::execute();
		return 0;
	};

	pUIThreadTimer.reset();

	auto pUIThreadParam = std::make_shared<SUIThreadParam>();
	pUIThreadParam->pbCompleted = &bCompletedUIRoutine;
	auto hThread = g_winapiApiTable->CreateThread(nullptr, 0, LauncherUIRoutine, pUIThreadParam.get(), 0, nullptr);
	if (!IS_VALID_HANDLE(hThread))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_UI_THREAD_CREATE_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Launcher UI thread create step completed!");

	while (bCompletedUIRoutine == false || !pUIThreadParam->pWndPtr)
	{
		if (pUIThreadTimer.diff() > 5000)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_UI_THREAD_INIT_FAIL;
			goto _Complete;
		}

		g_winapiApiTable->Sleep(100);
	}
	DEBUG_LOG(LL_SYS, "Launcher UI thread init step completed!");

	// Common Init routine
	if (g_nmApp->InitMgrInstance()->IsProcessProtected() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PROTECTION_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Protection check step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(2);
	// 

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

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(4);
	// 

	if (g_nmApp->InitMgrInstance()->RequestPrivilege(SE_DEBUG_PRIVILEGE) == false)
	{
#ifndef _DEBUG
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DEBUG_PRIV_FAIL;
		goto _Complete;
#endif
	}
	DEBUG_LOG(LL_SYS, "Request debug priv step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(6);
	// 

	if (CExceptionHandlers::InitExceptionHandlers() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_EXCEPTION_HANDLER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Exception handler setup step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(10);
	// 

	// Check dependencies
	if (m_nmDirFunctions->IsFileExist(g_nmApp->InitMgrInstance()->m_szModuleName) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_MODULE_NOT_FOUND;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Module exist check step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(12);
	// 

	if (m_nmDirFunctions->IsFileExist(g_nmApp->InitMgrInstance()->m_szLauncherConfigName) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_LAUNCHER_CONFIG_NOT_FOUND;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Launcher config exist check step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(14);
	// 

	if (m_nmDirFunctions->IsFileExist(g_nmApp->InitMgrInstance()->m_szServiceName) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_NOT_FOUND;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Service exist check step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(16);
	// 

	if (m_nmDirFunctions->IsFileExist(g_nmApp->InitMgrInstance()->m_szHelperAppName) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPERAPP_NOT_FOUND;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper app exist check step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(18);
	// 

	// Anti analysis
	CAntiDebug::AntiCuckoo();
	DEBUG_LOG(LL_SYS, "Anti cuckoo step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(20);
	// 

	// Remap windows modules
	if (m_nmManualMapHelper->RefreshModules() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_MMAP_RELOAD_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Manualmap refresh routine step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(24);
	// 

	// OS Check
	if (IsWindowsXPOrGreater() == false || IsWindowsServer())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_UNSUPPORTED_OS;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 1 completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(26);
	// 

	if (IsWindowsVistaOrGreater() == false && g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll, xorstr("NtCreateThreadEx").crypt_get()) /* Any not XP supported windows api*/)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_FAKE_OS;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 2 completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(28);
	// 

	if (IsFakeConditionalVersion())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_FAKE_OS_VERSION;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 3 completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(30);
	// 

	// Safe mode check
	if (m_nmFunctions->IsSafeModeEnabled())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SAFE_MODE;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Safe mode check step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(32);
	// 

	// Remote web connections
	if (g_nmApp->InitMgrInstance()->SafeRemoteCheck(&dwRemoteCheckErr) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_REMOTE_ERROR_BASE + dwRemoteCheckErr;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Remote check step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(36);
	// 

	// Read & Parse config options
	if (ParseLauncherConfig() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARSE_LAUNCHER_CONFIG;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Launcher config parse step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(40);
	// 

	// Check helper app
	szLowerHelperAppName	= g_nmApp->FunctionsInstance()->szLower(g_nmApp->InitMgrInstance()->m_szHelperAppName);
	bIsHelperRunning		= bool(CProcessFunctions::GetProcessCountFromProcessName(szLowerHelperAppName.c_str()) != 0);
	if (!bIsHelperRunning)
	{
		auto szHelperWithPath = g_nmApp->DirFunctionsInstance()->ExePath() + xorstr("\\").crypt_get() + g_nmApp->InitMgrInstance()->m_szHelperAppName;
		DEBUG_LOG(LL_SYS, "Helper process not running! Process: %s the process will begin!", szHelperWithPath.c_str());

		if (!g_winapiApiTable->CreateProcessA(szHelperWithPath.c_str(), (char*)"", NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
		{
			DEBUG_LOG(LL_ERR, "Helper process can not started! Error: %u", g_winapiApiTable->GetLastError());

			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_START_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Helper process created! PID: %u Handle: %p", pi.dwProcessId, pi.hProcess);

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(44);
		// 

		auto ntStat = g_nmApp->DynamicWinapiInstance()->NTHelper()->ResumeThread(pi.hThread, false);
		if (!NT_SUCCESS(ntStat))
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_RESUME_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Helper process resume succesfully resumed!");

		g_winapiApiTable->Sleep(1000); // wait for initilization
	}

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(48);
	// 
	
	// Create client pipe to launcher and connect helper's server pipe
	pHelperPipeManager = std::make_shared<CPipeManager>(g_nmApp->InitMgrInstance()->m_szHelperAppPipe, 100UL /* ms */);
	if (!IS_VALID_SMART_PTR(pHelperPipeManager))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_PIPE_CLASS_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper pipe alloc step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(50);
	// 

	do
	{
		if (pHelperPipeManager->InitPipeManagerClientside())
		{
			bHelperPipeInitCompleted = true;
			break;
		}

		DEBUG_LOG(LL_ERR, "Helper clientside pipe initilization fail! Attempt: %d Error: %u", iPipeOpenAttemptCount, g_winapiApiTable->GetLastError());

		iPipeOpenAttemptCount += 1;
		g_winapiApiTable->Sleep(1000);

	} while (iPipeOpenAttemptCount <= 10);

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(54);
	// 

	if (bHelperPipeInitCompleted == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_PIPE_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper pipe init step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(56);
	// 

	iPipeOpenAttemptCount = 0;
	DEBUG_LOG(LL_SYS, "Pipe attempt counter reset!");

	if (pHelperPipeManager->SendPing() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_PIPE_PING_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper pipe ping step completed!");

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(60);
	// 

	// Check helper app current status
	bIsHelperRunning = bool(CProcessFunctions::GetProcessCountFromProcessName(szLowerHelperAppName.c_str()) != 0);
	if (!bIsHelperRunning)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HELPER_EXIST_LAST_FAIL;
		goto _Complete;
	}

	// Set progressbar position
	pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(64);
	// 

	// Check service status
	szLowerServiceName	= g_nmApp->FunctionsInstance()->szLower(g_nmApp->InitMgrInstance()->m_szServiceName);
	bIsServiceRunning	= bool(CProcessFunctions::GetProcessCountFromProcessName(szLowerServiceName.c_str()) != 0);

	if (bIsServiceRunning) // Service looks like already exist, Simple jump to communication part
	{
		goto _ServiceCommStart;
	}
	else // here have not any nm_service.exe, so create new one
	{
		szCurrentPath = g_nmApp->DirFunctionsInstance()->ExePath();
		if (szCurrentPath.empty())
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_CURR_PATH_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Path detect step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(66);
		// 

		szServicePath = szCurrentPath + xorstr("\\").crypt_get() + g_nmApp->InitMgrInstance()->m_szServiceName;
		if (g_nmApp->DirFunctionsInstance()->IsFileExist(szServicePath) == false)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_NOT_FOUND_WITH_PATH;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Service exist check step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(68);
		// 

		// Service management
		pServiceHelper = std::make_unique<CServiceHelper>(g_nmApp->InitMgrInstance()->m_szServiceID, g_nmApp->InitMgrInstance()->m_szServiceDisplayName, szServicePath);
		if (!IS_VALID_SMART_PTR(pServiceHelper))
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_HELPER_ALLOC_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Service helper alloc step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(70);
		// 

_SvcLoadRoutinePre:
		if (pServiceHelper->LoadService(SERVICE_WIN32_OWN_PROCESS, &dwDriverHelperErr) == false)
		{
			if (dwDriverHelperErr == ERROR_SERVICE_EXISTS) // Service object is already created
			{
				if (bReCheckServiceLoad == true) // If we tried destroy old object and re-load but still looks already exist
				{
					if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_ALREADY_EXIST;
					goto _Complete;
				}

				dwServiceStatus = pServiceHelper->GetServiceStatus(); // check current status before than unload, if service is running at the first stop it
				DEBUG_LOG(LL_SYS, "Service load fail, Target service already exist! Service status: %u", dwServiceStatus);

				if (dwServiceStatus == SERVICE_START_PENDING || dwServiceStatus == SERVICE_RUNNING || dwServiceStatus == SERVICE_CONTINUE_PENDING)
				{
					if (pServiceHelper->StopDriver(&dwDriverHelperErr) == false)
					{
						if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_PRE_STOP_FAIL;
						goto _Complete;
					}
					DEBUG_LOG(LL_SYS, "Service pre stop step completed!");
				}

				g_winapiApiTable->Sleep(2000); // Wait 2sec for complete stop operation

				if (pServiceHelper->UnloadDriver(&dwDriverHelperErr) == false)
				{
					if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_UNLOAD_FAIL;
					goto _Complete;
				}
				bReCheckServiceLoad = true;

				DEBUG_LOG(LL_SYS, "Service unload step completed!");
				g_winapiApiTable->Sleep(2000); // Wait 2sec for complete unload operation

				goto _SvcLoadRoutinePre;
			}
			else
			{
				if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_LOAD_ERR_BASE + dwDriverHelperErr;
				goto _Complete;
			}
		}
		DEBUG_LOG(LL_SYS, "Driver load step completed!");

_SvcStartRoutinePre:
		if (pServiceHelper->StartDriver(&dwDriverHelperErr) == false)
		{
			if (dwDriverHelperErr == ERROR_INVALID_IMAGE_HASH)
			{
				if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_SIGN_FAIL;
				goto _Complete;
			}
			else if (dwDriverHelperErr == ERROR_SERVICE_ALREADY_RUNNING) // Service is already running
			{
				if (bReCheckServiceStart == true) // If we tried stop and re-start but still looks already running
				{
					if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_ALREADY_RUNNING;
					goto _Complete;
				}

				if (pServiceHelper->StopDriver(&dwDriverHelperErr) == false)
				{
					if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_STOP_FAIL;
					goto _Complete;
				}

				g_winapiApiTable->Sleep(2000); // Wait 2sec for complete stop operation

				bReCheckServiceStart = true;
				goto _SvcStartRoutinePre;
			}
			else
			{
				if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_START_ERR_BASE + dwDriverHelperErr;
				goto _Complete;
			}
		}
		DEBUG_LOG(LL_SYS, "Service start step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(76);
		// 

		// Enum process and check service process is it running
		while (iFindAttemptCount <= 3)
		{
			if (CProcessFunctions::GetProcessCountFromProcessName(szLowerServiceName.c_str()) != 0) { // service succesfully found
				break;
			}
			else
			{
				DEBUG_LOG(LL_ERR, "Service process not running yet! Attempt: %d", iFindAttemptCount);

				iFindAttemptCount += 1;
				g_winapiApiTable->Sleep(1000);
			}
		}
		DEBUG_LOG(LL_SYS, "Service running check step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(80);
		// 

		// Check service is it not running
		if (iFindAttemptCount > 0 && CProcessFunctions::GetProcessCountFromProcessName(szLowerServiceName.c_str()) == 0)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_FIND_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Service count check step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(84);
		// 

_ServiceCommStart:
		// Create client pipe to launcher and connect service's server pipe
		pServicePipeManager = std::make_shared<CPipeManager>(g_nmApp->InitMgrInstance()->m_szServicePipe, 100UL /* ms */);
		if (!IS_VALID_SMART_PTR(pServicePipeManager))
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PIPE_CLASS_ALLOC_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Pipe helper alloc step completed!");

		do
		{
			if (pServicePipeManager->InitPipeManagerClientside())
			{
				bServicePipeInitCompleted = true;
				break;
			}

			DEBUG_LOG(LL_ERR, "Clientside pipe initilization fail! Attempt: %d Error: %u", iPipeOpenAttemptCount, g_winapiApiTable->GetLastError());

			iPipeOpenAttemptCount += 1;
			g_winapiApiTable->Sleep(1000);

		} while (iPipeOpenAttemptCount <= 30);

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(88);
		// 

		if (bServicePipeInitCompleted == false)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SVC_PIPE_INIT_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Pipe helper init step completed!");

		if (pServicePipeManager->SendPing() == false)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SVC_PIPE_PING_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Pipe ping step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(92);
		// 

		// Check current status
		bIsServiceRunning = bool(CProcessFunctions::GetProcessCountFromProcessName(szLowerServiceName.c_str()) != 0);
		if (!bIsServiceRunning)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_EXIST_LAST_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Service count check(2) step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(100);
		// 

		// Send game start request
		if (pServicePipeManager->SendGameLaunchNotification(m_initMgr->m_szProtectedAppOption, s_szParams, true) == false)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_PIPE_GAME_REQ_SEND_FAIL;
			goto _Complete;
		}
		DEBUG_LOG(LL_SYS, "Send game launch notification step completed!");

		// Set progressbar position
		pUIThreadParam->pWndPtr->GetProgressBarInstance()->set_position(100);
		// 
	}

	bRet = true;

_Complete:
	auto bServicePipeFinalizeRet	= true;
	auto bHelperPipeFinalizeRet		= true;

	if (IS_VALID_SMART_PTR(pServicePipeManager))
		bServicePipeFinalizeRet = pServicePipeManager->DestroyPipeManagerClientside();
	DEBUG_LOG(LL_SYS, "Service pipe destroy step completed!");

	if (IS_VALID_SMART_PTR(pHelperPipeManager))
		bHelperPipeFinalizeRet = pHelperPipeManager->DestroyPipeManagerClientside();
	DEBUG_LOG(LL_SYS, "Helper pipe destroy step completed!");

	if (pUIThreadParam->pWndPtr && pUIThreadParam->pWndPtr->get_handle())
		g_winapiApiTable->SendMessageA(pUIThreadParam->pWndPtr->get_handle(), WM_CLOSE, NULL, NULL);
	DEBUG_LOG(LL_SYS, "Launcher UI destroy step completed!");

	if (pDwErrorCode && *pDwErrorCode == 0)
	{
		if (bServicePipeFinalizeRet == false)
			*pDwErrorCode = INIT_ERR_SVC_PIPE_FINALIZE_FAIL;

		else if (bHelperPipeFinalizeRet == false)
			*pDwErrorCode = INIT_ERR_HELPER_PIPE_FINALIZE_FAIL;

		else
			bRet = true;
	}

	DEBUG_LOG(LL_SYS, "OnLauncherInit routine completed!");
	return bRet;
}
