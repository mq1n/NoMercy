#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "NM_InitilizationManager.h"
#include "PipeManager.h"
#include "ExceptionHandlers.h"
#include "ManualMap.h"
#include "WinVerHelper.h"
#include "AntiDebug.h"
#include "AntiEmulation.h"
#include "AntiBreakpoint.h"
#include "ProcessFunctions.h"
#include "ServiceCommunication.h"
#include "ThreadExitCallback.h"
#include "AntiMacro.h"

// Global
static auto gs_CommunicationTimerID				= 0;
static auto gs_iWatchdogTimerID					= 0;
static auto gs_hSyncProc						= HANDLE(INVALID_HANDLE_VALUE);
static auto gs_bWatchdogTimerFirstWorkCompleted = false;
static auto gs_bThreadsInitialized				= false;
static auto gs_bTimerCheckCompleted				= false;
static auto gs_WatchdogTimer					= CTimer<std::chrono::milliseconds>();
static auto gs_TimerChecker						= CTimer<std::chrono::milliseconds>();

static bool ClientMainRoutine()
{
	MSG message;
	while (g_winapiApiTable->GetMessageA(&message, NULL, 0, 0))
	{
		DEBUG_LOG(LL_TRACE, "Message: %u handled! IsTimer: %d", message.message, message.message == WM_TIMER);

		if (message.message == WM_QUIT)
		{
//			DEBUG_LOG(LL_CRI, "Quit message handled!");
			break;
		}

		g_winapiApiTable->TranslateMessage(&message);
		g_winapiApiTable->DispatchMessageA(&message);
	}
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------

// Utils routine

void NTAPI OnSyncWait(PVOID pParam, BOOLEAN)
{
	static auto bBlocked = false;
	if (bBlocked)
		return;

	static auto iIndex = 0;
	if (iIndex >= INT_MAX - 1) iIndex = 1;
	DEBUG_LOG(LL_TRACE, "Sync callback works! tid: %u idx: %d", g_winapiApiTable->GetCurrentThreadId(), iIndex++);

	if (gs_bThreadsInitialized)
	{
		if (gs_WatchdogTimer.diff() > 30000) // 15sec for timer watchdog
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_SELF_INTEGRITY_TIMEOUT_1, g_winapiApiTable->GetLastError());
			bBlocked = true;
			return;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------

void NTAPI OnTimerCheckExit(PVOID pParam, BOOLEAN)
{
	DEBUG_LOG(LL_CRI, "Timer check thread terminated! Check result: %d", gs_bTimerCheckCompleted);

	if (gs_bTimerCheckCompleted == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SELF_INTEGRITY_TIMEOUT_2, g_winapiApiTable->GetLastError());
		return;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------

// Communication check
inline void CommunicationCheckRoutineEx()
{
	static auto bBlocked = false;
	if (bBlocked)
		return;

	static auto iIndex = 0;
	if (iIndex >= INT_MAX - 1) iIndex = 1;
	DEBUG_LOG(LL_SYS, "[%d] Communication check routine has been started!", iIndex++);

	static auto pServicePipeManager = g_nmApp->ClientToServicePipeMgr();
	if (!IS_VALID_SMART_PTR(pServicePipeManager))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_CTOS_PIPE_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		bBlocked = true;
		return;
	}

	if (pServicePipeManager->SendPing() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_CTOS_PIPE_PING_FAIL, g_winapiApiTable->GetLastError());
		bBlocked = true;
		return;
	}

	static auto pDriverCommHelper = g_nmApp->ClientToDriverCommHelper();
	if (!IS_VALID_SMART_PTR(pDriverCommHelper))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_CTOD_COMM_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		bBlocked = true;
		return;
	}

	if (pDriverCommHelper->Ping() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_CTOD_COMM_PING_FAIL, g_winapiApiTable->GetLastError());
		bBlocked = true;
		return;
	}

	DEBUG_LOG(LL_SYS, "[%d] Communication check routine completed!", iIndex);
}
VOID CALLBACK CommunicationCheckRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	CommunicationCheckRoutineEx();
}
VOID CALLBACK CommunicationCheckRoutine(HWND hwnd, UINT message, UINT_PTR iCurrTimerID, DWORD dwTime)
{
	if (gs_CommunicationTimerID != iCurrTimerID)
		return;

	CommunicationCheckRoutineEx();
}

// --------------------------------------------------------------------------------------------------------------------------------------------

// Watchdog
inline void WatchdogRoutineEx()
{
	if (gs_bWatchdogTimerFirstWorkCompleted == false)
		gs_bWatchdogTimerFirstWorkCompleted = true;

	static auto s_iWatchdogLoopIdx = 0;
	if (s_iWatchdogLoopIdx >= INT_MAX - 1) s_iWatchdogLoopIdx = 1;

	DEBUG_LOG(LL_TRACE, "Watchdog timer is running! Idx: %d Delay: %u", s_iWatchdogLoopIdx++, gs_WatchdogTimer.diff());

	gs_WatchdogTimer.reset();

	// Check routine
	if (gs_WatchdogTimer.diff() > 10000)
	{
		// Check window watchdog validation
		if (g_nmApp->WatchdogInstance()->GetInitilizationStatus() == true && g_nmApp->WatchdogInstance()->CheckLoadedWatchdogs() == false)
		{
//			g_nmApp->OnCloseRequest(EXIT_ERR_WATCHDOG_CORRUPTED, g_winapiApiTable->GetLastError());
			return;
		}

		// Check sync owned handle validation
		if (g_nmApp->DynamicWinapiInstance()->IsValidHandle(gs_hSyncProc) == false)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_SYNC_HANDLE_CORRUPTED, g_winapiApiTable->GetLastError());
			return;
		}

		gs_WatchdogTimer.reset();
	}

	// Initialize routine after than setup game's wndproc
	if (gs_bThreadsInitialized == false)
	{
		if (IS_VALID_SMART_PTR(g_nmApp))
		{
			if (IS_VALID_SMART_PTR(g_nmApp->FunctionsInstance()))
			{
				auto hMainWnd = g_nmApp->FunctionsInstance()->GetMainWindow(g_winapiApiTable->GetCurrentProcessId());
				if (hMainWnd)
				{
					gs_bThreadsInitialized = true;

#if 0
					if (InitializeWindowProtection(hMainWnd) == false)
					{
						DEBUG_LOG(LL_ERR, "Window protection fail!");
					}
#endif

					if (g_nmApp->InitializeClientThreads() == false)
					{
						DEBUG_LOG(LL_ERR, "Client threads initilization fail! Last error: %u", g_winapiApiTable->GetLastError());
						g_nmApp->OnCloseRequest(EXIT_ERR_CLIENT_THREADS_INIT_FAIL, g_winapiApiTable->GetLastError());
						return;
					}

					if (g_nmApp->WatchdogInstance()->InitializeWatchdog() == false)
					{
						DEBUG_LOG(LL_ERR, "Watchdog initilization fail! Last error: %u", g_winapiApiTable->GetLastError());
						g_nmApp->OnCloseRequest(EXIT_ERR_WINDOWS_WATCHDOG_THREAD_FAIL, g_winapiApiTable->GetLastError());
						return;
					}
				}
			}
		}
	}
}

VOID CALLBACK WatchdogRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	WatchdogRoutineEx();
}
VOID CALLBACK WatchdogRoutine(HWND hwnd, UINT message, UINT_PTR iCurrTimerID, DWORD dwTime)
{
	if (gs_iWatchdogTimerID != iCurrTimerID)
		return;

	WatchdogRoutineEx();
}

// --------------------------------------------------------------------------------------------------------------------------------------------

DWORD WINAPI TimerCheckThreadRoutine(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Timer check thread event has been started");

	auto pTimerCheckThreadTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread			= std::shared_ptr<CThread>();
	do
	{
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_TIMER_CHECKER);

		if (pTimerCheckThreadTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_TIMER_CHECK_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);

	} while (!IS_VALID_SMART_PTR(currentThread));

	while (1)
	{
		if (gs_TimerChecker.diff() > 60000) // 60 sec
		{
			DEBUG_LOG(LL_SYS, "Timer check started! Diff: %u Result: %d", gs_TimerChecker.diff(), gs_bWatchdogTimerFirstWorkCompleted);

			if (gs_bWatchdogTimerFirstWorkCompleted == false)
			{
				g_nmApp->OnCloseRequest(EXIT_ERR_WATCHDOG_TIMER_INTEGRITY_FAIL, g_winapiApiTable->GetLastError());
			}
			else
			{
				gs_bTimerCheckCompleted = true;
				break;
			}
		}

		// Update ticks
		g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_TIMER_CHECKER);
		g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_TIMER_CHECKER, GetCurrentTime());

		g_winapiApiTable->Sleep(1000);
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc1()
{
	__asm
	{
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
	}
}
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------


// Routines
void CNM_App::OnClientFinalize() //todo
{
	// FinalizePlugins

	// UnloadPlugins

	// m_pCTOSPipeManager Finalize notification

	// m_pCTODCommHelper Finalize notification

	// UnregisterWait hSyncWaitObj

	// UnregisterWait pTimerCheckerThreadWatcher

	// TerminateThread pTimerCheckerThread

	if (gs_CommunicationTimerID)
		g_winapiApiTable->KillTimer(NULL, gs_CommunicationTimerID);

	if (gs_iWatchdogTimerID)
		g_winapiApiTable->KillTimer(NULL, gs_iWatchdogTimerID);

	if (IS_VALID_SMART_PTR(m_pCTOSPipeManager))
		m_pCTOSPipeManager->DestroyPipeManagerClientside();
}

bool CNM_App::OnClientInit(LPDWORD pDwErrorCode)
{
	DEBUG_LOG(LL_SYS, "Client routine started!");

	// Common Veriables
	auto bRet						= false;
	auto bManualMapCompleted		= false;
	auto bPipeInitCompleted			= false;
	auto bRegisterWaitRet			= false;
	auto bPackedResult				= false;	
	auto pTimerCheckerThread		= std::shared_ptr<CThread>();
	auto pTimerCheckerThreadWatcher = std::shared_ptr<CThreadExitWatcher>();
	auto szPipeName					= std::string("");
	auto szParentName				= std::string("");
	auto szServiceName				= std::string("");
	auto szBadModuleName			= std::string("");
	auto hParentProcess				= HANDLE(INVALID_HANDLE_VALUE);
	auto hSyncWaitObj				= HANDLE(INVALID_HANDLE_VALUE);
	auto hWatchdogTimer				= HANDLE(INVALID_HANDLE_VALUE);
	auto iElevationRet				= 0;
	auto iSysMetrics				= 0;
	auto iPipeOpenAttemptCount		= 0;
	auto dwParentPID				= 0UL;
	auto dwLastError				= 0UL;
	auto dwRemoteCheckErr			= 0UL;
	auto dwTestSignRet				= 0UL;
	auto dwDebugRet					= 0UL;
	auto dwEmulationRet				= 0UL;
	auto dwVirtualizeRet			= 0UL;
	auto dwModuleCheckRet			= 0UL;
	auto ulTimerCheckFuncSize		= 0UL;

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

	if (bManualMapCompleted == false && g_nmApp->InitMgrInstance()->RequestPrivilege(SE_DEBUG_PRIVILEGE) == false)
	{
#ifndef _DEBUG
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DEBUG_PRIV_FAIL;
		goto _Complete;
#endif
	}
	DEBUG_LOG(LL_SYS, "Request debug priv step completed!");

	if (g_winapiApiTable->SetPriorityClass(NtCurrentProcess, ABOVE_NORMAL_PRIORITY_CLASS) == FALSE)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PROC_PRIORITY_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Process priority adjust step completed!");

	if (bManualMapCompleted == false && CExceptionHandlers::InitExceptionHandlers() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_EXCEPTION_HANDLER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Exception handler setup step completed!");

	if (m_nmDirFunctions->IsFileExist(g_nmApp->InitMgrInstance()->m_szServiceName) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_NOT_FOUND;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Service exist check step completed!");

	// Anti analysis
	if (bManualMapCompleted == false)
	{
		CAntiDebug::AntiCuckoo();
	}
	DEBUG_LOG(LL_SYS, "Anti cuckoo step completed!");

	if (bManualMapCompleted == true)
	{
		DEBUG_LOG(LL_SYS, "Manual map already completed! Spawning to post section.");
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
	if (IsWindowsXPSP3OrGreater() == false || IsWindowsServer())
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

	if (g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32, xorstr("RegisterServiceProcess").crypt_get()))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_OLDEST_OS;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 3 completed!");

	if (IsFakeConditionalVersion())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_FAKE_OS_VERSION;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "OS check step 4 completed!");

	// Safe mode check
	if (m_nmFunctions->IsSafeModeEnabled())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SAFE_MODE;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Safe mode check step completed!");

	// Check process pack status
	bPackedResult = m_nmDirFunctions->IsPackedExecutable(m_nmDirFunctions->ExeNameWithPath());
	DEBUG_LOG(LL_SYS, "Main process packed result: %d", bPackedResult);
	m_nmData->SetPackedProcess(bPackedResult);

	// Remote web connections
	if (g_nmApp->InitMgrInstance()->SafeRemoteCheck(&dwRemoteCheckErr) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_REMOTE_ERROR_BASE + dwRemoteCheckErr;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Remote check step completed!");

	// Check service is it running
	szServiceName = g_nmApp->FunctionsInstance()->szLower(g_nmApp->InitMgrInstance()->m_szServiceName);
	if (CProcessFunctions::GetProcessCountFromProcessName(szServiceName.c_str()) == 0)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SERVICE_EXIST_ENUM_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Service running check step completed!");

	// Check parent process
	dwParentPID	= CProcessFunctions::GetProcessParentProcessId(g_winapiApiTable->GetCurrentProcessId());
	if (!dwParentPID)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_PID_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Parent process check step completed! Parent: %u", dwParentPID);

#ifndef _DEBUG
	szParentName = CProcessFunctions::GetProcessNameFromProcessId(dwParentPID);
	if (szParentName.empty())
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_NAME_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Parent process check step completed! Parent: %s", szParentName.c_str());

	if (szServiceName != szParentName)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PARENT_CHECK_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Parent compare check step completed!");
#endif

	// Build pipe name
	szPipeName = g_nmApp->InitMgrInstance()->m_szServicePipe;
	DEBUG_LOG(LL_SYS, "Target pipe: %s", szPipeName.c_str());

	// Create client pipe to launcher and connect service's server pipe
	m_pCTOSPipeManager = std::make_shared<CPipeManager>(szPipeName, 100UL /* ms */); 
	if (!IS_VALID_SMART_PTR(m_pCTOSPipeManager))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PIPE_CLASS_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe helper alloc step completed!");

	do
	{
		if (m_pCTOSPipeManager->InitPipeManagerClientside())
		{
			bPipeInitCompleted = true;
			break;
		}

		DEBUG_LOG(LL_ERR, "Clientside pipe initilization fail! Attempt: %d Error: %u", iPipeOpenAttemptCount, g_winapiApiTable->GetLastError());

		iPipeOpenAttemptCount += 1;
		g_winapiApiTable->Sleep(1000);

	} while (iPipeOpenAttemptCount <= 10);

	if (bPipeInitCompleted == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SVC_PIPE_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe helper init step completed!");

	if (m_pCTOSPipeManager->SendPing() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SVC_PIPE_PING_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Pipe ping step completed!");

	m_pCTOSPipeManager->SendClientInformation
	(
		g_winapiApiTable->GetCurrentProcessId(), dwParentPID, m_nmData->GetAppType(), m_nmData->GetGameCode(), m_nmDirFunctions->ExeNameWithPath(), 
		m_nmFunctions->GetProcessCommandLine(), CLOSE_GAME_ON_EXIT, IS_X64_APP, NOMERCY_ALLOWED_VERSION
	);
	DEBUG_LOG(LL_SYS, "Pipe client information send step completed!");

	// Driver communication
	m_pCTODCommHelper = std::make_shared<CServiceComm>();
	if (!IS_VALID_SMART_PTR(m_pCTODCommHelper))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_COMM_HELPER_ALLOC_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver communication helper alloc step completed!");

	if (m_pCTODCommHelper->Initialize(g_nmApp->InitMgrInstance()->m_szDriverSymLink) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_COMM_HELPER_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver communication helper init step completed!");

	if (m_pCTODCommHelper->Ping() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_DRIVER_COMM_HELPER_PING_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Driver communication helper ping step completed!");

	// ---
	DEBUG_LOG(LL_SYS, "Protection routine started! Build: %s Game: %d\n", m_nmFunctions->FixBuildDate().c_str(), m_nmData->GetAppType());

	// Communication check timer
	gs_CommunicationTimerID = g_winapiApiTable->SetTimer(NULL, NULL, 1000, CommunicationCheckRoutine);
	if (!gs_CommunicationTimerID)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_COMM_CHECK_TIMER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Communication timer init step completed!");

	// Setup sync self handle watcher
	gs_hSyncProc = m_dynamicWinapi->NTHelper()->OpenProcess(SYNCHRONIZE, g_winapiApiTable->GetCurrentProcessId());
	if (!IS_VALID_HANDLE(gs_hSyncProc))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SYNC_WATCHER_OPEN_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Self sync watcher handle create step completed!");

	bRegisterWaitRet = g_winapiApiTable->RegisterWaitForSingleObject(&hSyncWaitObj, gs_hSyncProc, OnSyncWait, nullptr, 1000, WT_EXECUTEINUITHREAD);
	if (bRegisterWaitRet == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_SYNC_WATCHER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Self sync watcher init step completed!");

	// Reset counters
	gs_WatchdogTimer.reset();

	// Watchdog timer
	gs_iWatchdogTimerID = g_winapiApiTable->SetTimer(NULL, 0, 1000, WatchdogRoutine);
	if (!gs_iWatchdogTimerID)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_WATCHDOG_TIMER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Watchdog timer created! ID: %d", gs_iWatchdogTimerID);

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

	// Access Rules
	if (m_nmAccessHelper->BlockAccess(NtCurrentProcess) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_ACCESS_ADJUST_1_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Access rule 1 adjust step completed!");

	if (m_nmAccessHelper->BlockAccess(NtCurrentThread) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_ACCESS_ADJUST_2_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Access rule 2 adjust step completed!");

	if (m_nmAccessHelper->DenyProcessAccess() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_ACCESS_ADJUST_3_FAIL;
		goto _Complete;
	} 
	DEBUG_LOG(LL_SYS, "Access rule 3 adjust step completed!");

	if (m_nmAccessHelper->DenyThreadAccess(NtCurrentThread) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_ACCESS_ADJUST_4_FAIL;
		goto _Complete;
	} 
	DEBUG_LOG(LL_SYS, "Access rule 4 adjust step completed!");

	if (IsWindowsVistaOrGreater())
	{
		m_nmAccessHelper->EnablePermanentDep();
	}
	DEBUG_LOG(LL_SYS, "Access rule 5 adjust step completed!");

	if (m_nmPluginMgr->LoadPlugins() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PLUGIN_LOAD_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Plugin load step completed!");

	if (m_nmPluginMgr->ExecutePlugins() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_PLUGIN_EXECUTE_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Plugin execute step completed!");

	// Timer checker
	gs_TimerChecker.reset();

	ulTimerCheckFuncSize = m_threadMgr->GetThreadFuncSize((LPBYTE)TimerCheckThreadRoutine);
	if (!ulTimerCheckFuncSize || ulTimerCheckFuncSize > 1024)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_TIMER_CHECK_THREAD_SIZE_FAIL;
		goto _Complete;
	}

	pTimerCheckerThread = m_threadMgr->CreateCustomThread(SELF_THREAD_TIMER_CHECKER, TimerCheckThreadRoutine, ulTimerCheckFuncSize, nullptr, 0, true);
	if (!IS_VALID_SMART_PTR(pTimerCheckerThread) || pTimerCheckerThread->IsCorrect() == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_TIMER_CHECK_THREAD_CREATE_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Timer check thread step completed! Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p", pTimerCheckerThread->GetId(), pTimerCheckerThread->GetHandle(), 
		pTimerCheckerThread->GetStartAddress(), pTimerCheckerThread->GetCustomCode(), pTimerCheckerThread->GetThreadCustomName().c_str(), pTimerCheckerThread, pTimerCheckerThread.get());

	pTimerCheckerThreadWatcher = std::make_shared<CThreadExitWatcher>();
	if (!IS_VALID_SMART_PTR(pTimerCheckerThreadWatcher))
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_TIMER_CHECK_THREAD_WATCHER_INIT_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Timer check thread watcher alloc step completed!");

	if (pTimerCheckerThreadWatcher->InitializeExitCallback(pTimerCheckerThread->GetHandle(), OnTimerCheckExit) == false)
	{
		if (pDwErrorCode) *pDwErrorCode = INIT_ERR_TIMER_CHECK_THREAD_WATCHER_SETUP_FAIL;
		goto _Complete;
	}
	DEBUG_LOG(LL_SYS, "Timer check thread watcher init step completed!");
	
	bRet = ClientMainRoutine();

_Complete:
	DEBUG_LOG(LL_SYS, "Client routine completed!");
	return bRet;
}

// -----------------

bool CNM_App::InitializeClientThreads()
{
	if (InitCheckModuleModifications() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_MMAP_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (InitializeMainCheckThread() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_MAIN_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (CAntiBreakpoint::InitAntiSoftBP() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SOFTBP_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (CAntiMacro::InitAntiMacro() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_ANTI_MACRO_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (m_nmScannerInterface->InitChecksumScan() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_CHECKSUM_SCAN_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (m_nmSelfThreadMgr->InitThreadTickChecker() == false)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_TICK_CHECK_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	return true;
}

