#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "NM_InitilizationManager.h"
#include "WinVerHelper.h"
#include "ExceptionHandlers.h"
#include "ManualMap.h"
#include "AntiDebug.h"
#include "ProcessFunctions.h"
#include "PipeManager.h"
#include "WinInetAPI.h"
#include "ServiceHelper.h"
#include "ServiceCommunication.h"
#include "SafeProcessHandle.h"
#include "Terminator.h"
#include "Timer.h"
#include "AntiDebug.h"
#include "AntiEmulation.h"
#include "AntiBreakpoint.h"
#include "FilterCommunication.h"
#include "FilterBridge.h"

#include "../../Common/SharedTypes/WdkTypes.h"
#include "../../Common/SharedTypes/CtlTypes.h"

// static const auto gsc_iDriverLoadType = SERVICE_KERNEL_DRIVER;
static const auto gsc_iDriverLoadType = SERVICE_FILE_SYSTEM_DRIVER;

static auto gs_pObCallbackFilter		= std::shared_ptr <CCommPortListener <KB_FLT_OB_CALLBACK_INFO,		KbObCallbacks> >();
static auto gs_pProcessCallbackFilter	= std::shared_ptr <CCommPortListener <KB_FLT_PS_PROCESS_INFO,		KbPsProcess> >();
static auto gs_pThreadCallbackFilter	= std::shared_ptr <CCommPortListener <KB_FLT_PS_THREAD_INFO,		KbPsThread> >();
static auto gs_pImageCallbackFilter		= std::shared_ptr <CCommPortListener <KB_FLT_PS_IMAGE_INFO,			KbPsImage> >();
static auto gs_pDevHandleCallbackFilter	= std::shared_ptr <CCommPortListener <KB_FLT_CREATE_INFO,			KbFltPreCreate> >();
static auto gs_pDevIOCallbackFilter		= std::shared_ptr <CCommPortListener <KB_FLT_DEVICE_CONTROL_INFO,	KbFltPreDeviceControl> >();

static auto gs_hTimerQueue		= HANDLE(INVALID_HANDLE_VALUE);
static auto gs_hNetworkTimer	= HANDLE(INVALID_HANDLE_VALUE);

// Network
inline void NetworkCheckRoutineEx()
{
#if (CHECK_BACKEND_CONNECTION == TRUE)
	if (IS_VALID_SMART_PTR(g_nmApp) && IS_VALID_SMART_PTR(g_nmApp->FunctionsInstance()))
	{
		if (g_nmApp->IsConnectedToBackend() == false)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_BACKEND_CONNECTION_CHECK_FAIL, g_winapiApiTable->GetLastError());
		}
		else
		{
			DEBUG_LOG(LL_SYS, "Backend connection check completed!");

			if (IS_VALID_HANDLE(gs_hNetworkTimer) && IS_VALID_HANDLE(gs_hTimerQueue))
				g_winapiApiTable->RtlDeleteTimer(gs_hTimerQueue, gs_hNetworkTimer, INVALID_HANDLE_VALUE);
		}
	}
#endif
}

VOID CALLBACK NetworkCheckRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	NetworkCheckRoutineEx();
}

// ---------------------------------------------------

DWORD WINAPI ProtectedProcessWatcherRoutine(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Protected process watcher routine started!");

#ifdef _DEBUG
	Sleep(INFINITE); // Disable protected process watcher routine on debug build
#endif

	g_winapiApiTable->Sleep(30 * 1000); // Wait 30 sec for init before than first check

	auto pTimer = CTimer<std::chrono::milliseconds>();
	while (true)
	{
		if (pTimer.diff() > 2000)
		{
			auto vProtectedProcesses = g_nmApp->InitMgrInstance()->m_vLaunchedProcesses;
			if (vProtectedProcesses.empty())
			{
				DEBUG_LOG(LL_CRI, "All protected process has been closed, Service must be exit!");

				// Service finalize routine
				g_nmApp->OnServiceFinalize();

				// Terminate helper app
				auto hHelperProc = g_nmApp->DynamicWinapiInstance()->NTHelper()->OpenProcess(PROCESS_ALL_ACCESS, g_nmApp->GetHelperProcessId());
				if (IS_VALID_HANDLE(hHelperProc))
				{
					DEBUG_LOG(LL_SYS, "Handle succesfully created for helper process!");

					if (CTerminator::TerminateProcess(hHelperProc))
					{
						DEBUG_LOG(LL_SYS, "Helper process succesfully terminate!");
					}
				}

				auto pDriverHelper = std::make_unique<CServiceHelper>(g_nmApp->InitMgrInstance()->m_szDriverID);
				if (IS_VALID_SMART_PTR(pDriverHelper))
				{
					DEBUG_LOG(LL_SYS, "Driver helper class succesfully created!");
					
					if (pDriverHelper->StopDriver(nullptr))
					{
						DEBUG_LOG(LL_SYS, "Driver succesfully stopped!");
						
						if (pDriverHelper->UnloadDriver(nullptr))
						{
							DEBUG_LOG(LL_SYS, "Driver succesfully unloaded!");
						}
					}
				}

				auto pServiceHelper = std::make_unique<CServiceHelper>(g_nmApp->InitMgrInstance()->m_szServiceID);
				if (IS_VALID_SMART_PTR(pServiceHelper))
				{
					DEBUG_LOG(LL_SYS, "Service helper class succesfully created!");
					
					if (pServiceHelper->StopDriver(nullptr))
					{
						DEBUG_LOG(LL_SYS, "Service succesfully stopped!");
						
						if (pServiceHelper->UnloadDriver(nullptr))
						{
							DEBUG_LOG(LL_SYS, "Service succesfully unloaded!");
						}
					}
				}

				CTerminator::TerminateProcess(NtCurrentProcess);
			}
			else /* there are exist a alive protected process */
			{
				/*
				for (const auto & pCurrProcData : vProtectedProcesses)
				{
					// TODO: Simple running, suspend checks
					// TODO: Simple heartbeat for each client
				}
				*/
			}

			pTimer.reset();
		}
		g_winapiApiTable->Sleep(500);
	}

	g_nmApp->OnCloseRequest(EXIT_ERR_PROCESS_WATCHER_THREAD_EXIT, g_winapiApiTable->GetLastError());
	return 0;
}


typedef struct _GAME_LAUNCH_REQ_PARAMS
{
	char	szProcessName[MAX_PATH];
	char	szCommandLine[4096];
	bool	bForceClose;
} SGameLaunchReqParams, *PGameLaunchReqParams;

DWORD WINAPI OnGameLaunchReqReceived(LPVOID lpParam)
{
	DEBUG_LOG(LL_SYS, "Game launch request routine started!");

	auto lpLaunchParam = reinterpret_cast<SGameLaunchReqParams *>(lpParam);
	if (!lpLaunchParam)
	{
		DEBUG_LOG(LL_ERR, "Params not found!");

		g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_NULL_PARAM, g_winapiApiTable->GetLastError());
		return 0;
	}
	DEBUG_LOG(LL_SYS, "Params found!");

	std::string szProcessName = lpLaunchParam->szProcessName;
	if (szProcessName.empty())
	{
		DEBUG_LOG(LL_ERR, "Target process name param not found!");
	
		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_NULL_PROC_NAME, g_winapiApiTable->GetLastError());

		delete lpLaunchParam;
		lpLaunchParam = nullptr;

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Target process name: %s", szProcessName.c_str());

	std::string szCommandLine = lpLaunchParam->szCommandLine;
#if 0
	if (szCommandLine.empty())
	{
		DEBUG_LOG(LL_ERR, "Target process command line param not found!");

		delete lpLaunchParam;
		lpLaunchParam = nullptr;

		return 0;
	}
#endif
	DEBUG_LOG(LL_SYS, "Target process command line: %s", szCommandLine.c_str());

	delete lpLaunchParam;
	lpLaunchParam = nullptr;

	auto dwHelperProcessId = g_nmApp->GetHelperProcessId();
	if (!dwHelperProcessId)
	{
		DEBUG_LOG(LL_ERR, "Helper process id not found!");

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_NULL_HELPER_PID, g_winapiApiTable->GetLastError());
		return 0;
	}
	DEBUG_LOG(LL_SYS, "Helper process id: %u", dwHelperProcessId);

	// ---

	std::string szTargetProcessWithPath = g_nmApp->DirFunctionsInstance()->ExePath() + xorstr("\\").crypt_get() + szProcessName; // Bundle target process name with path
	if (g_nmApp->DirFunctionsInstance()->IsFileExist(szTargetProcessWithPath.c_str()) == false)
	{
		DEBUG_LOG(LL_ERR, "Target app: %s is NOT exist! Error: %u", szTargetProcessWithPath.c_str(), g_winapiApiTable->GetLastError());
	
		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_FILE_EXIST_CHECK_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Bundle path created! Target: %s", szTargetProcessWithPath.c_str());

	auto hCurrProc = g_nmApp->DynamicWinapiInstance()->NTHelper()->OpenProcess(PROCESS_ALL_ACCESS, g_winapiApiTable->GetCurrentProcessId());
	if (!IS_VALID_HANDLE(hCurrProc))
	{
		DEBUG_LOG(LL_ERR, "Current process open fail! Error: %u", g_winapiApiTable->GetLastError());
	
		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_OPEN_CURR_PROC_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}

	auto hHelperProc = g_nmApp->DynamicWinapiInstance()->NTHelper()->OpenProcess(PROCESS_ALL_ACCESS, dwHelperProcessId);
	if (!IS_VALID_HANDLE(hHelperProc))
	{
		DEBUG_LOG(LL_ERR, "Explorer.exe can not open! Error: %u", g_winapiApiTable->GetLastError());
	
		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_OPEN_HELPER_PROC_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}

	PROCESS_INFORMATION pi = { 0 };
	auto szBundlePath = szTargetProcessWithPath + " " + szCommandLine;
	auto hProtectedProcess = g_nmApp->ServiceRunProgram(hHelperProc, 0, szBundlePath.c_str(), g_nmApp->DirFunctionsInstance()->ExePath().c_str(), &pi, FALSE, hCurrProc);
	if (!IS_VALID_HANDLE(hProtectedProcess))
	{
		DEBUG_LOG(LL_ERR, "Target process can not launched! Error: %u", g_winapiApiTable->GetLastError());

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_LAUNCH_GAME_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Target process succesfully launched! PID: %u", pi.dwProcessId);

	auto ntStat = g_winapiApiTable->NtResumeProcess(hProtectedProcess);;
	if (!NT_SUCCESS(ntStat))
	{
		DEBUG_LOG(LL_ERR, "Target process can not resume! Ntstatus: %p", ntStat);

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_RESUME_GAME_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Target process succesfully resumed!");

	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(pi.hProcess);
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(pi.hThread);
	DEBUG_LOG(LL_SYS, "Handles closed!");

	if (CProcessFunctions::ProcessIsItAlive(pi.dwProcessId) == false)
	{
		DEBUG_LOG(LL_ERR, "Target process: %u is NOT alive!", pi.dwProcessId);

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_GAME_ALIVE_CHECK_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Alive check completed!");

	// TODO: Start shadow process
#if 0
	if (m_nmFunctions->CreateShadow(pi.dwProcessId) == false)
	{
	}
#endif

	auto hProcess = g_winapiApiTable->OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, pi.dwProcessId);
	if (!IS_VALID_HANDLE(hProcess))
	{
		DEBUG_LOG(LL_ERR, "Target process access can NOT created! Error: %u", g_winapiApiTable->GetLastError());

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_GAME_HANDLE_OPEN_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Handle created for protected process!");

	auto dwWaitRet = g_winapiApiTable->WaitForSingleObject(hProcess, 5000);
	if (dwWaitRet != WAIT_TIMEOUT)
	{
		DEBUG_LOG(LL_ERR, "Target process access lost! Ret: %u Error: %u", dwWaitRet, g_winapiApiTable->GetLastError());

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_GAME_WAIT_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Wait completed!");

	auto pLaunchedProcessParams = std::make_shared<SGameLaunchParams>();
	if (!IS_VALID_SMART_PTR(pLaunchedProcessParams))
	{
		DEBUG_LOG(LL_ERR, "Launcher parameters can NOT created! Error: %u", g_winapiApiTable->GetLastError());

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_LAUNCH_PARAM_CONTAINER_ALLOC_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Launch param container allocated!");

	pLaunchedProcessParams->dwProcessId = pi.dwProcessId;
	pLaunchedProcessParams->hProcess	= hProcess;
	strcpy(pLaunchedProcessParams->szPath, szTargetProcessWithPath.c_str());

	g_nmApp->InitMgrInstance()->m_vLaunchedProcesses.push_back(pLaunchedProcessParams);
	DEBUG_LOG(LL_SYS, "Launch param container stored!");

	auto pLauncherThreadParams = std::make_shared<SGameLaunchThreadParams>();
	if (!IS_VALID_SMART_PTR(pLauncherThreadParams))
	{
		DEBUG_LOG(LL_ERR, "Launcher thread parameters can NOT created! Error: %u", g_winapiApiTable->GetLastError());

		if (lpLaunchParam->bForceClose)
			g_nmApp->OnCloseRequest(EXIT_ERR_GAME_LAUNCH_ROUTINE_LAUNCH_THREAD_PARAM_CONTAINER_ALLOC_FAIL, g_winapiApiTable->GetLastError());

		return 0;
	}
	DEBUG_LOG(LL_SYS, "Launcher thread param container allocated!");

	pLauncherThreadParams->dwProcessId			= pi.dwProcessId;
	pLauncherThreadParams->dwLauncherThreadId	= g_winapiApiTable->GetCurrentThreadId();
	pLauncherThreadParams->bCompleted			= true;

	DEBUG_LOG(LL_SYS, "Game launch request routine completed!");
	return 0;
}

bool CNM_App::OnLaunchGameNotification(const std::string & szProcessName, const std::string & szCommandLine, bool bForceCloseWhenFail)
{
	DEBUG_LOG(LL_SYS, "Game launch request received! App: %s Cmd: %s Force: %d", szProcessName.c_str(), szCommandLine.c_str(), bForceCloseWhenFail);

	auto pParam = new SGameLaunchReqParams();
	if (!pParam)
	{
		DEBUG_LOG(LL_ERR, "Thread param can not initialized! Error: %u\n", g_winapiApiTable->GetLastError());
		return false;
	}
	strcpy(pParam->szProcessName, szProcessName.c_str());
	strcpy(pParam->szCommandLine, szCommandLine.c_str());
	pParam->bForceClose = bForceCloseWhenFail;

	auto dwThreadId = 0UL;
	auto hThread	= g_winapiApiTable->CreateThread(nullptr, 0, OnGameLaunchReqReceived, pParam, 0, &dwThreadId);
	if (!IS_VALID_HANDLE(hThread))
	{
		DEBUG_LOG(LL_ERR, "Thread can not launched! Error: %u\n", g_winapiApiTable->GetLastError());
		return false;
	}

	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hThread);
	DEBUG_LOG(LL_SYS, "Thread launched! TID: %u\n", dwThreadId);
	return true;
}

bool CNM_App::OnGameInformationNotification(DWORD dwProcessId, DWORD dwParentProcessId, DWORD dwAppCode, DWORD dwGameCode, const std::string & szPath,
	const std::string & szCommandline, BOOL bCloseOnExit, BOOL bIsX64, DWORD dwNoMercyVersion)
{
	DEBUG_LOG(LL_SYS, "Game information notification received!");

	// TODO: Process all params

	// e.g.
//	if (dwParentProcessId != g_winapiApiTable->GetCurrentProcessId()) // notification sender game's parent is not equal to our pid
//	{
//	}

	return true;
}

bool CNM_App::OnClientInitilizationFailNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode)
{
	DEBUG_LOG(LL_SYS, "Cliet init fail notification received! Error codes: %u-%u", dwErrorCode, dwSystemErrorCode);
	return true;
}

bool CNM_App::OnClientCloseRequestNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode)
{
	DEBUG_LOG(LL_SYS, "Cliet close request notification received! Error codes: %u-%u", dwErrorCode, dwSystemErrorCode);

	// TODO: Wait 10sec if still exist process terminate from service, Remove from process list, Send terminate notification to driver, Open faq page via helper app
	return true;
}

// ---------------------------------------------------

void CNM_App::OnServiceMessageHandled(int iMessageID)
{
	std::string szLog = "NoMercy: Service message handled! ID: " + std::to_string(iMessageID);
	OutputDebugStringA(szLog.c_str());
}

void CNM_App::OnServiceFinalize()
{
	if (IS_VALID_HANDLE(gs_hNetworkTimer) && IS_VALID_HANDLE(gs_hTimerQueue))
		g_winapiApiTable->RtlDeleteTimer(gs_hTimerQueue, gs_hNetworkTimer, INVALID_HANDLE_VALUE);

	if (IS_VALID_HANDLE(gs_hTimerQueue))
		g_winapiApiTable->RtlDeleteTimerQueue(gs_hTimerQueue);
}

bool CNM_App::OnServiceInit(bool bProtected, LPDWORD pDwErrorCode)
{
	DEBUG_LOG(LL_SYS, "Service routine started!");

	// Common Veriables
	auto bRet					= false;
	auto bManualMapCompleted	= false;
	auto bReCheckDriverLoad		= false;
	auto bReCheckDriverStart	= false;
	auto bReCheckServiceLoad	= false;
	auto bReCheckServiceStart	= false;
	auto bPipeInitCompleted		= false;
	auto pServiceHelper			= std::unique_ptr<CServiceHelper>();
	auto pSvcCommHelper			= std::unique_ptr<CServiceComm>();
	auto szCurrentPath			= std::string("");
	auto szDriverPath			= std::string("");
	auto szServicePath			= std::string("");
	auto szLowerServiceName		= std::string("");
	auto szLowerLauncherName	= std::string("");
	auto szParentName			= std::string("");
	auto szModuleWithPath		= std::string("");
	auto szServicesName			= std::string("");
	auto szMutexName			= std::string("");
	auto szBadModuleName		= std::string("");
	auto szHelperProcName		= std::string("");
	auto iElevationRet			= 0;
	auto iSysMetrics			= 0;
	auto iPipeOpenAttemptCount	= 0;
	auto dwLogoSum				= 0UL;
	auto dwRemoteCheckErr		= 0UL;
	auto dwDriverHelperErr		= 0UL;
	auto dwProcessWatcherTid	= 0UL;
	auto dwParentProcessId		= 0UL;
	auto dwDebugRet				= 0UL;
	auto dwEmulationRet			= 0UL;
	auto dwVirtualizeRet		= 0UL;
	auto dwModuleCheckRet		= 0UL;
	auto dwTestSignRet			= 0UL;
	auto dwHelperProcessId		= 0UL;
	auto ntStatus				= 0UL;
	auto hWatcherThread			= HANDLE(INVALID_HANDLE_VALUE);

	// Create timer queue
	ntStatus = g_winapiApiTable->RtlCreateTimerQueue(&gs_hTimerQueue);
	if (!NT_SUCCESS(ntStatus) || !IS_VALID_HANDLE(gs_hTimerQueue))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_TIMER_QUEUE_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Timer queue init step completed!");

	// Connect to helper app's pipe
	m_pSTOHPipeManager = std::make_shared<CPipeManager>(g_nmApp->InitMgrInstance()->m_szHelperAppPipe, 100UL /* ms */);
	if (!IS_VALID_SMART_PTR(m_pSTOHPipeManager))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_HELPER_PIPE_CLASS_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper pipe helper alloc step completed!");

	do
	{
		if (m_pSTOHPipeManager->InitPipeManagerClientside())
		{
			bPipeInitCompleted = true;
			break;
		}

		DEBUG_LOG(LL_ERR, "Clientside pipe initilization fail! Attempt: %d Error: %u", iPipeOpenAttemptCount, g_winapiApiTable->GetLastError());

		iPipeOpenAttemptCount += 1;
		g_winapiApiTable->Sleep(1000);

	} while (iPipeOpenAttemptCount <= 10);

	// Reset pipe attempt counter
	iPipeOpenAttemptCount = 0;
	DEBUG_LOG(LL_SYS, "Pipe attempt counter reset!");

	if (bPipeInitCompleted == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_HELPER_PIPE_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper Pipe init step completed!");

	if (!IS_VALID_SMART_PTR(m_pSTOHPipeManager->GetClientPipe()) || !IS_VALID_HANDLE(m_pSTOHPipeManager->GetClientPipe()->Get()))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_HELPER_PIPE_NULL_PTR;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe ptr check step completed!");

	if (m_pSTOHPipeManager->SendPing() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_HELPER_PIPE_PING_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe ping step completed!");

	if (!g_winapiApiTable->GetNamedPipeServerProcessId(m_pSTOHPipeManager->GetClientPipe()->Get(), &dwHelperProcessId))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_HELPER_PIPE_PID_FAIL;
		goto _Complete;
	}
	m_dwHelperProcessId = dwHelperProcessId;
	DEBUG_LOG(LL_SYS, "Pipe pid check step completed!");

	if (CProcessFunctions::ProcessIsItAlive(dwHelperProcessId) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_HELPER_ENUM_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper alive check step completed!");

	szHelperProcName = CProcessFunctions::GetProcessNameFromProcessId(dwHelperProcessId);
	if (szHelperProcName.empty() || szHelperProcName != m_nmFunctions->szLower(m_initMgr->m_szHelperAppName))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_SVC_ERR_HELPER_NAME_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Helper name check step completed!");

_PreMMap:
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

	if (g_nmApp->InitMgrInstance()->RequestPrivilege(SE_DEBUG_PRIVILEGE) == false && bManualMapCompleted == false)
	{
#ifndef _DEBUG
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DEBUG_PRIV_FAIL;
		goto _Complete;
#endif
	}
	DEBUG_LOG(LL_SYS, "Request debug priv step completed!");

	if (g_nmApp->InitMgrInstance()->RequestPrivilege(SE_LOAD_DRIVER_PRIVILEGE) == false && bManualMapCompleted == false)
	{
#ifndef _DEBUG
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_LOAD_PRIV_FAIL;
		goto _Complete;
#endif
	}
	DEBUG_LOG(LL_SYS, "Request driver load priv step completed!");

	if (g_nmApp->InitMgrInstance()->RequestPrivilege(SE_TCB_PRIVILEGE) == false && bManualMapCompleted == false)
	{
#ifndef _DEBUG
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_TCB_PRIV_FAIL;
		goto _Complete;
#endif
	}
	DEBUG_LOG(LL_SYS, "Request tcb priv step completed!");

	if (CExceptionHandlers::InitExceptionHandlers() == false && bManualMapCompleted == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_EXCEPTION_HANDLER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Exception handler setup step completed!");

	szCurrentPath = g_nmApp->DirFunctionsInstance()->ExePath();
	if (szCurrentPath.empty())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_CURR_PATH_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Path detect step completed!");

#ifndef _DEBUG
	dwParentProcessId = CProcessFunctions::GetProcessParentProcessId(g_winapiApiTable->GetCurrentProcessId());
	if (!dwParentProcessId)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_PID_FAIL;
		goto _Complete;
	}

	szParentName = CProcessFunctions::GetProcessNameFromProcessId(dwParentProcessId);
	if (szParentName.empty())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_NAME_FAIL;
		goto _Complete;
	}

	szServicesName = xorstr("services.exe").crypt_get();
	if (szServicesName != szParentName)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_CHECK_FAIL;
		goto _Complete;
	}
#endif

	// Check dependencies
	szModuleWithPath = szCurrentPath + xorstr("\\").crypt_get() + g_nmApp->InitMgrInstance()->m_szModuleName;
	if (m_nmDirFunctions->IsFileExist(szModuleWithPath) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_MODULE_NOT_FOUND;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Module exist check step completed!");

	// Anti analysis
	if (bManualMapCompleted == false)
		CAntiDebug::AntiCuckoo();

	DEBUG_LOG(LL_SYS, "Anti cuckoo step completed!");

	if (bManualMapCompleted == true)
	{
		goto _PostMMap;
	}

	// Remap windows modules
	if (m_nmManualMapHelper->RefreshModules() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_MMAP_RELOAD_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Manualmap refresh routine step completed!");

	bManualMapCompleted = true;
	goto _PreMMap;

_PostMMap:
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

	// Check service status
	szLowerServiceName = g_nmApp->FunctionsInstance()->szLower(g_nmApp->InitMgrInstance()->m_szServiceName);
	if (CProcessFunctions::GetProcessCountFromProcessName(szLowerServiceName.c_str()) != 1)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_EXIST_ENUM_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Service count check step completed!");

	if (CProcessFunctions::GetProcessIdFromProcessName(szLowerServiceName.c_str()) != g_winapiApiTable->GetCurrentProcessId())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_EXIST_PID_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Service pid check step completed!");

	// Create server pipe
	m_pSvcPipeManager = std::make_shared<CPipeManager>(g_nmApp->InitMgrInstance()->m_szServicePipe, 100UL /* ms */);
	if (!IS_VALID_SMART_PTR(m_pSvcPipeManager))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SRV_PIPE_CLASS_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe helper alloc step completed!");

	// Initialize pipe watcher thread
	if (m_pSvcPipeManager->CreateServerWatcherThread() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SRV_SVC_PIPE_RECV_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe helper create routine step completed!");

	// Build driver path
	if (g_nmApp->FunctionsInstance()->IsX64System())
		szDriverPath = szCurrentPath + xorstr("\\").crypt_get() + g_nmApp->InitMgrInstance()->m_szDriverName_x64;
	else
		szDriverPath = szCurrentPath + xorstr("\\").crypt_get() + g_nmApp->InitMgrInstance()->m_szDriverName_x86;

	if (g_nmApp->DirFunctionsInstance()->IsFileExist(szDriverPath) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_NOT_FOUND_WITH_PATH;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver exist check step completed!");

	// Driver management
	pServiceHelper = std::make_unique<CServiceHelper>(g_nmApp->InitMgrInstance()->m_szDriverID, g_nmApp->InitMgrInstance()->m_szDriverDisplayName, szDriverPath);
	if (!IS_VALID_SMART_PTR(pServiceHelper))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_HELPER_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver helper alloc step completed!");

_LoadRoutinePre:
	if (pServiceHelper->LoadService(gsc_iDriverLoadType, &dwDriverHelperErr) == false)
	{
//		DEBUG_LOG(LL_SYS, "Load service return with: %u", dwDriverHelperErr);

		if (dwDriverHelperErr == ERROR_SERVICE_EXISTS) // Driver object is already created
		{
			if (bReCheckDriverLoad == true) // If we tried destroy old object and re-load but still looks already exist
			{
				if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_ALREADY_EXIST;
				goto _Complete;
			}

			auto dwServiceStatus = pServiceHelper->GetServiceStatus(); // check current status before than unload, if driver is running at the first stop it
			DEBUG_LOG(LL_SYS, "Service load fail, Target service already exist! Service status: %u", dwServiceStatus);

			if (dwServiceStatus == SERVICE_START_PENDING || dwServiceStatus == SERVICE_RUNNING || dwServiceStatus == SERVICE_CONTINUE_PENDING)
			{
				if (pServiceHelper->StopDriver(&dwDriverHelperErr) == false)
				{
					DEBUG_LOG(LL_SYS, "Driver stop fail! Error: %u", dwDriverHelperErr);
					if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_PRE_STOP_FAIL;
					goto _Complete;
				}
				DEBUG_LOG(LL_SYS, "Driver pre stop step completed!");
			}

			g_winapiApiTable->Sleep(2000); // Wait 2sec for complete stop operation

			if (pServiceHelper->UnloadDriver(&dwDriverHelperErr) == false)
			{
				DEBUG_LOG(LL_SYS, "Driver unload fail! Error: %u", dwDriverHelperErr);
				if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_UNLOAD_FAIL;
				goto _Complete;
			}
			bReCheckDriverLoad = true;

			DEBUG_LOG(LL_SYS, "Driver unload step completed!");
			g_winapiApiTable->Sleep(2000); // Wait 2sec for complete unload operation

			goto _LoadRoutinePre;
		}
		else
		{
			DEBUG_LOG(LL_SYS, "Driver load fail! Error: %u", dwDriverHelperErr);
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_LOAD_ERR_BASE + dwDriverHelperErr;
			goto _Complete;
		}
	}
	DEBUG_LOG(LL_SYS, "Driver load step completed!");

_StartRoutinePre:
	if (gsc_iDriverLoadType == SERVICE_FILE_SYSTEM_DRIVER)
	{
		if (!pServiceHelper->SetupFilterInstance(g_nmApp->InitMgrInstance()->m_szDriverID, xorstr("DefInst").crypt_get(), xorstr("260000").crypt_get(), 0, TRUE))
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_FILTER_SETUP_FAIL;
			goto _Complete;
		}
	}
	DEBUG_LOG(LL_SYS, "Driver filter setup step completed!");

	if (pServiceHelper->StartDriver(&dwDriverHelperErr) == false)
	{
		if (dwDriverHelperErr == ERROR_INVALID_IMAGE_HASH) // Unsigned NoMercy driver
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_SIGN_FAIL;
			goto _Complete;
		}
		else if (dwDriverHelperErr == ERROR_SERVICE_ALREADY_RUNNING) // Driver is already running
		{
			if (bReCheckDriverStart == true) // If we tried stop and re-start but still looks already running
			{
				if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_ALREADY_RUNNING;
				goto _Complete;
			}

			if (pServiceHelper->StopDriver(&dwDriverHelperErr) == false)
			{
				DEBUG_LOG(LL_SYS, "Driver stop(2) fail! Error: %u", dwDriverHelperErr);
				if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_STOP_FAIL;
				goto _Complete;
			}

			g_winapiApiTable->Sleep(2000); // Wait 2sec for complete stop operation

			bReCheckDriverStart = true;
			goto _StartRoutinePre;
		}
		else
		{
			DEBUG_LOG(LL_SYS, "Driver start(2) fail! Error: %u", dwDriverHelperErr);
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_START_ERR_BASE + dwDriverHelperErr;
			goto _Complete;
		}
	}
	DEBUG_LOG(LL_SYS, "Driver start step completed!");

	// Driver communication
	pSvcCommHelper = std::make_unique<CServiceComm>();
	if (!IS_VALID_SMART_PTR(pSvcCommHelper))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_COMM_HELPER_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver communication helper alloc step completed!");

	if (pSvcCommHelper->Initialize(g_nmApp->InitMgrInstance()->m_szDriverSymLink) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_COMM_HELPER_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver communication helper init step completed!");

	if (pSvcCommHelper->Ping() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_COMM_HELPER_PING_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver communication helper ping step completed!");

	// Check test digital sign is enabled
	if (m_nmScannerInterface->IsTestSignEnabled(&dwTestSignRet))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_TEST_SIGN_ENABLED_BASE + dwTestSignRet;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Test sign check step completed!");

	// Check secure boot is capable but not enabled
	if (m_nmScannerInterface->IsSecureBootEnabled() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SCR_BOOT_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Secure boot check step completed!");

	// HWID Manager
	if (m_nmHwidManager->Initilize() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HWID_MGR_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Hwid manager init step completed!");

	// Antis
	if (m_dynamicWinapi->CheckModulesIntegrity(&szBadModuleName, &dwModuleCheckRet) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_WIN_MOD_CHECK_FAIL_BASE + dwModuleCheckRet;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Module check step completed!");

#ifdef _DEBUG
	if (g_winapiApiTable->IsDebuggerPresent() == false) // Allow debugging for debug build but still run anti debug routine if a debugger not attached
#endif
	{
		if (CAntiDebug::InitAntiDebug(&dwDebugRet) == false)
		{
			if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DEBUG_CHECK_FAIL_BASE + dwDebugRet;
			goto _Complete;
		}
	}
	DEBUG_LOG(LL_SYS, "Anti debug init step completed!");

	if (CAntiEmulation::InitAntiEmulation(&dwEmulationRet) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_EMULATION_CHECK_FAIL_BASE + dwEmulationRet;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Anti emulation init step completed!");

#ifndef ALLOW_VIRTUAL_ENVIRONMENT
	if (CAntiDebug::AntiVirtualize(&dwVirtualizeRet) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_VIRTUALIZE_CHECK_FAIL_BASE + dwVirtualizeRet;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Anti virtualize init step completed!");
#endif

	if (CAntiBreakpoint::HasHardwareBreakpoint())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HWBP_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Anti hwbp check step completed!");

	if (CAntiBreakpoint::HasEntrypointBreakpoint())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_EPBP_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Anti entrypoint bp check step completed!");

#ifndef ALLOW_VIRTUAL_ENVIRONMENT
	if (CAntiDebug::HypervisorCheckTriggered())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_HYPERVISOR_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Anti hypervisor step completed!");
#endif

	// Check wow64 callgate redirection
	if (g_nmApp->FunctionsInstance()->IsWow64Process(NtCurrentProcess))
	{
		auto pCurrTeb = GetCurrentTeb();
		if (pCurrTeb && pCurrTeb->WOW32Reserved)
		{
			// Disables file system redirection for the calling thread.
			PVOID OldValue = NULL;
			if (g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &OldValue))
			{
				char szSystemDirectory[MAX_PATH] = { 0x0 };
				GetSystemDirectoryA(szSystemDirectory, MAX_PATH);

				auto szWow64Cpu = g_nmApp->FunctionsInstance()->szLower(szSystemDirectory) + xorstr("\\wow64cpu.dll").crypt_get();
				if (g_nmApp->DirFunctionsInstance()->IsFileExist(szWow64Cpu))
				{
					auto szWow32Owner = m_nmFunctions->GetModuleOwnerName(pCurrTeb->WOW32Reserved);

					if (szWow64Cpu != szWow32Owner)
					{
						if (pDwErrorCode) *pDwErrorCode = INIT_ERR_WOW64_RDR_CHECK_FAIL;
						goto _Complete;
					}
				}

				// Restore file system redirection for the calling thread.
				g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, OldValue, nullptr);
			}
		}
	}

	// Memory watchdog
	if (m_nmScannerInterface->InitializeMemoryWatchdogs() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_MEM_WATCHDOG_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Memory watchdog init step completed!");

	// TODO: Dacl rules
	// TODO: Self security for NM_Service when driver is OK, e.g Close NM_Service targeted handles, runtime manual map system dlls, runtime anti debug etc.

	hWatcherThread = g_winapiApiTable->CreateThread(nullptr, 0, ProtectedProcessWatcherRoutine, nullptr, 0, &dwProcessWatcherTid);
	if (!IS_VALID_HANDLE(hWatcherThread))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PROCESS_WATCHER_THREAD_FAIL;
		goto _Complete;
	}
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hWatcherThread);
	DEBUG_LOG(LL_SYS, "Protected process watcher thread initialized! TID: %u", dwProcessWatcherTid);

	if (InitializeFilterCallbacks() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_FILTER_CALLBACK_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Service filter callback initilizations completed!");

	if (InitializeServiceThreads() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_THREAD_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Service threads initilization completed!");

	bRet = true;

_Complete:
	DEBUG_LOG(LL_SYS, "OnServiceInit routine completed!");
	return bRet;
}

bool CNM_App::InitializeFilterCallbacks()
{
	// Check driver status
	// Communication routine with driver just like client (ping)

	BOOL Status = FALSE;

	// -------------------------------------------- ObRegisterCallbacks listener --------------------------------------------------------

	gs_pObCallbackFilter = std::make_shared<CCommPortListener <KB_FLT_OB_CALLBACK_INFO, KbObCallbacks> >();
	if (!IS_VALID_SMART_PTR(gs_pObCallbackFilter))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_OB_CALLBACK_WATCHER_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// Callback routine
	Status = gs_pObCallbackFilter->Subscribe([](CCommPort& Port, CMessagePacket<KB_FLT_OB_CALLBACK_INFO>& Message) -> VOID
	{
		auto Data = static_cast<PKB_FLT_OB_CALLBACK_INFO>(Message.GetData());

		auto from	= Data->Target.ProcessId;
		auto to		= Data->Client.ProcessId;
		auto newacs = Data->CreateResultAccess;
		auto oldacs = Data->CreateDesiredAccess;

//		DEBUG_LOG(LL_CRI, "ObCallback; From: %lld -> To: %lld With access: %p", from, to, newacs);

		CReplyPacket<KB_FLT_OB_CALLBACK_INFO> Reply(Message, ERROR_SUCCESS, *Data);
		Port.Reply(Reply);
	});

	if (!Status)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_OB_CALLBACK_WATCHER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// -------------------------------------------- PsSetCreateProcessNotifyRoutine listener --------------------------------------------------------

	gs_pProcessCallbackFilter = std::make_shared<CCommPortListener <KB_FLT_PS_PROCESS_INFO, KbPsProcess> >();
	if (!IS_VALID_SMART_PTR(gs_pProcessCallbackFilter))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_PROCESS_CALLBACK_WATCHER_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// Callback routine
	Status = gs_pProcessCallbackFilter->Subscribe([](CCommPort& Port, CMessagePacket<KB_FLT_PS_PROCESS_INFO>& Message) -> VOID
	{
		auto Data = static_cast<PKB_FLT_PS_PROCESS_INFO>(Message.GetData());

//		DEBUG_LOG(LL_CRI, "Process Callback; IsCreated: %d PID: %lld Parent PID: %lld", Data->Created, Data->ProcessId, Data->ParentId);
	});

	if (!Status)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_PROCESS_CALLBACK_WATCHER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}


	// -------------------------------------------- PsSetCreateThreadNotifyRoutine listener --------------------------------------------------------

	gs_pThreadCallbackFilter = std::make_shared<CCommPortListener <KB_FLT_PS_THREAD_INFO, KbPsThread> >();
	if (!IS_VALID_SMART_PTR(gs_pThreadCallbackFilter))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_THREAD_CALLBACK_WATCHER_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// Callback routine
	Status = gs_pThreadCallbackFilter->Subscribe([](CCommPort& Port, CMessagePacket<KB_FLT_PS_THREAD_INFO>& Message) -> VOID
	{
		auto Data = static_cast<PKB_FLT_PS_THREAD_INFO>(Message.GetData());

//		DEBUG_LOG(LL_CRI, "Thread Callback; IsCreated: %d PID: %lld TID: %lld", Data->Created, Data->ProcessId, Data->ThreadId);
	});

	if (!Status)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_THREAD_CALLBACK_WATCHER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

#if 0
	// -------------------------------------------- PsSetCreateImageNotifyRoutine listener --------------------------------------------------------

	gs_pImageCallbackFilter = std::make_shared<CCommPortListener <KB_FLT_PS_IMAGE_INFO, KbPsImage> >();
	if (!IS_VALID_SMART_PTR(gs_pImageCallbackFilter))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_IMAGE_CALLBACK_WATCHER_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// Callback routine
	Status = gs_pImageCallbackFilter->Subscribe([](CCommPort& Port, CMessagePacket<KB_FLT_PS_IMAGE_INFO>& Message) -> VOID
	{
		auto Data = static_cast<PKB_FLT_PS_IMAGE_INFO>(Message.GetData());

//		DEBUG_LOG(LL_CRI, "Image Callback; PID: %lld Base: %p Name: %ls", Data->ProcessId, Data->BaseAddress, Data->FullImageName);
	});

	if (!Status)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_IMAGE_CALLBACK_WATCHER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

#if 0
	// -------------------------------------------- IRP_MJ_CREATE listener --------------------------------------------------------

	gs_pDevHandleCallbackFilter = std::make_shared<CCommPortListener <KB_FLT_CREATE_INFO, KbFltPreCreate> >();
	if (!IS_VALID_SMART_PTR(gs_pDevHandleCallbackFilter))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_DEVICE_HANDLE_WATCHER_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// Callback routine
	Status = gs_pDevHandleCallbackFilter->Subscribe([](CCommPort& Port, CMessagePacket<KB_FLT_CREATE_INFO>& Message) -> VOID
	{
		auto Data = static_cast<PKB_FLT_CREATE_INFO>(Message.GetData());

//		DEBUG_LOG(LL_CRI, "Device handle Callback; PID: %lld TID: %lld Name: %ls", Data->ProcessId, Data->ThreadId, Data->Path);
	});

	if (!Status)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_DEVICE_HANDLE_WATCHER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

#if 0
	// -------------------------------------------- IRP_MJ_DEVICE_CONTROL listener --------------------------------------------------------

	gs_pDevIOCallbackFilter = std::make_shared<CCommPortListener <KB_FLT_DEVICE_CONTROL_INFO, KbFltPreDeviceControl> >();
	if (!IS_VALID_SMART_PTR(gs_pDevIOCallbackFilter))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_DEVICE_IO_WATCHER_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// Callback routine
	Status = gs_pDevIOCallbackFilter->Subscribe([](CCommPort& Port, CMessagePacket<KB_FLT_DEVICE_CONTROL_INFO>& Message) -> VOID
	{
		auto Data = static_cast<PKB_FLT_DEVICE_CONTROL_INFO>(Message.GetData());

//		DEBUG_LOG(LL_CRI, "Device IO Callback; PID: %lld TID: %lld Name: %ls", Data->ProcessId, Data->ThreadId, Data->Path);
	});

	if (!Status)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FILTER_DEVICE_IO_WATCHER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	return true;
}

bool CNM_App::InitializeServiceThreads()
{
	auto ntStatus = g_winapiApiTable->RtlCreateTimer(gs_hTimerQueue, &gs_hNetworkTimer, NetworkCheckRoutine, NULL, 60 * 1000, 60 * 1000, WT_EXECUTEINUITHREAD);
	if (!NT_SUCCESS(ntStatus) || !IS_VALID_HANDLE(gs_hNetworkTimer))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_NETWORK_TIMER_CREATE_FAIL, ntStatus);
		return false;
	}

#if (ENABLE_SERVICE_TO_BACKEND_CONNECTION == TRUE)
	m_nmNetworkMgr->RegisterOnConnectedCallback(std::bind(&CNM_App::OnBackEndConnected, this));
	m_nmNetworkMgr->RegisterOnDisconnectedCallback(std::bind(&CNM_App::OnBackEndDisconnected, this));

	if (m_nmNetworkMgr->InitSocketConnection() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_NETWORK_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	// TODO: Start scanner's local db scanner routine

#if 0
	if (InitCheckModuleModifications() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_MMAP_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (CAntiBreakpoint::InitAntiSoftBP() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SOFTBP_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (m_nmDirFunctions->InitializeFolderCheck() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_FOLDER_CHECK_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	// TODO: Create ScanInterface thread for manage all scans

	m_nmWMI->InitWMIWatcher(); // WMI process, module etc. watcher  

//	m_nmAccessHelper->InitAdjustPrivThread(); // Debug access decrease thread
	LPThreads->InitThreadCheck();
	manual mapscan

	// Thread checker
	LPThreads->InitThreadTickCheck();
#endif

	return true;
}

// ---

HANDLE CNM_App::ServiceRunProgram(HANDLE hTokenOwner, LPCSTR lpFilename, LPCSTR lpArguments, LPCSTR lpDir, LPPROCESS_INFORMATION ProcessInformation, BOOL Inherit, HANDLE hParent)
{
	bool bCompleted = false;

	HANDLE hProcessToken = NULL, hUserToken = NULL;
	LPVOID pEnvironment = NULL;

	PPROC_THREAD_ATTRIBUTE_LIST pAttributeList = NULL;
	SIZE_T cbAttributeListSize = 0;

	STARTUPINFOEXA si = { 0 };
	si.StartupInfo.cb = sizeof(STARTUPINFOEXA);

	if (!ProcessInformation)
	{
		DEBUG_LOG(LL_ERR, "Null process information!");
		goto _Complete;
	}

	// TODO: Decrease reqested token access priv
	if (!OpenProcessToken(hTokenOwner, TOKEN_ALL_ACCESS, &hProcessToken))
	{
		DEBUG_LOG(LL_ERR, "OpenProcessToken fail! Error: %u", LI_FIND(GetLastError)());
		goto _Complete;
	}

	if (!DuplicateTokenEx(hProcessToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hUserToken))
	{
		DEBUG_LOG(LL_ERR, "DuplicateTokenEx fail! Error: %u", LI_FIND(GetLastError)());
		goto _Complete;
	}

	if (!CreateEnvironmentBlock(&pEnvironment, hUserToken, TRUE))
	{
		DEBUG_LOG(LL_ERR, "CreateEnvironmentBlock fail! Error: %u", LI_FIND(GetLastError)());
		goto _Complete;
	}

	InitializeProcThreadAttributeList(NULL, 1, 0, &cbAttributeListSize);

	pAttributeList = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(cbAttributeListSize, __FUNCTION__));
	if (!pAttributeList)
	{
		DEBUG_LOG(LL_ERR, "HeapAlloc fail! Error: %u", LI_FIND(GetLastError)());
		goto _Complete;
	}

	if (!InitializeProcThreadAttributeList(pAttributeList, 1, 0, &cbAttributeListSize))
	{
		DEBUG_LOG(LL_ERR, "InitializeProcThreadAttributeList fail! Error: %u", LI_FIND(GetLastError)());
		goto _Complete;
	}

	if (!UpdateProcThreadAttribute(pAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hParent, sizeof(HANDLE), NULL, NULL))
	{
		DEBUG_LOG(LL_ERR, "UpdateProcThreadAttribute fail! Error: %u", LI_FIND(GetLastError)());
		goto _Complete;
	}

	si.lpAttributeList = pAttributeList;
	if (!CreateProcessAsUserA(hUserToken, lpFilename, const_cast<LPSTR>(lpArguments), NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT | CREATE_SUSPENDED | EXTENDED_STARTUPINFO_PRESENT, pEnvironment, lpDir, reinterpret_cast<LPSTARTUPINFOA>(&si), ProcessInformation))
	{
		DEBUG_LOG(LL_ERR, "CreateProcessAsUserA fail! Error: %u", LI_FIND(GetLastError)());
		goto _Complete;
	}

	bCompleted = true;

_Complete:
	if (pEnvironment)
		DestroyEnvironmentBlock(pEnvironment);

	if (IS_VALID_HANDLE(hUserToken))
		LI_FIND(CloseHandle)(hUserToken);

	if (IS_VALID_HANDLE(ProcessInformation->hThread))
		LI_FIND(CloseHandle)(ProcessInformation->hThread);

	if (IS_VALID_HANDLE(hProcessToken))
		LI_FIND(CloseHandle)(hProcessToken);

	if (pAttributeList)
	{
		DeleteProcThreadAttributeList(pAttributeList);
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pAttributeList);
	}

	return bCompleted ? ProcessInformation->hProcess : nullptr;
}
