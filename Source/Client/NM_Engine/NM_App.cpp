#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "Timer.h"
#include "MiniDump.h"
#include "WinVerHelper.h"
#include "NM_SelfThreadManager.h"
#include "SelfProtection.h"
#include "NM_SDK_Manager.h"
#include "WorkQueue.h"
#include "Terminator.h"
#include "ExceptionHandlers.h"
#include "FilterBridge.h"

#include <lazy_importer.hpp>

#pragma optimize("", off)
CNM_App::CNM_App() :
	m_bAppIsInitiliazed(false), m_bIsMMapLoaded(false), m_bConnectedToBackEnd(false), m_bConnectionDisconnected(false), m_bFinalizeTriggered(false), 
	m_bIsCloseTriggered(false), m_dwHelperProcessId(0)
{
}

void CNM_App::TestMsg()
{
	LI_FIND(MessageBoxA)(0,0,0,0);
}

// Routines
bool CNM_App::InitializeServer(LPDWORD pDwErrorCode)
{
	DEBUG_LOG(LL_SYS, "Server initilization started!");

	// Elevation
	try 
	{
		if (m_nmAccessHelper->IsProcessElevated() == FALSE && m_nmData->GetAppType() != NM_TESTAPP)
			g_nmApp->OnCloseRequest(EXIT_ERR_SERVER_ELEVATION_1_FAIL, g_winapiApiTable->GetLastError());
	}
	catch (DWORD dwError) 
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("IsProcessElevated failed w/err: %p\n").crypt_get(), dwError);
		return false;
	}

	try
	{
		if (m_nmAccessHelper->IsRunAsAdmin() == FALSE && m_nmData->GetAppType() != NM_TESTAPP)
			g_nmApp->OnCloseRequest(EXIT_ERR_SERVER_ELEVATION_2_FAIL, g_winapiApiTable->GetLastError());
	}
	catch (DWORD dwError) 
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("IsRunAsAdmin failed w/err: %p\n").crypt_get(), dwError);
		return false;
	}

	// Network
	m_nmNetworkMgr->InitSocketConnection();

	return true;
}


bool CNM_App::InitializeEx(bool bClient, bool bProtected, LPDWORD pDwErrorCode)
{
	DEBUG_LOG(LL_SYS, "NoMercy Initilization has been started! App type: %d", m_nmData->GetAppType());

	auto bRet = false;

	if (bClient == false)
	{
		bRet = InitializeServer(pDwErrorCode);
	}
	else
	{
		switch (m_nmData->GetAppType())
		{
			case NM_CLIENT:
			case NM_TESTAPP:
			{
				bRet = OnClientInit(pDwErrorCode);
			} break;

			case NM_LAUNCHER:
			{
				bRet = OnLauncherInit(bProtected, pDwErrorCode);
			} break;

			case NM_SERVICE:
			{
				bRet = OnServiceInit(bProtected, pDwErrorCode);
			} break;

			case NM_HELPERAPP:
			{
				bRet = OnHelperAppInit(pDwErrorCode);
			} break;

			default: 
			{
				DEBUG_LOG(LL_CRI, "Unknown app type: %d", m_nmData->GetAppType());
				return false;
			} break;
		}
	}

	DEBUG_LOG(LL_SYS, "NoMercy Initilization completed! Result: %d", bRet);
	return bRet;
}

bool CNM_App::InitializeTestMode(bool bClient, LPDWORD pDwErrorCode)
{
	DEBUG_LOG(LL_SYS, "Test Mode Initilization started!");

	// Priv
	BOOLEAN bAdjustPrivRet;
	auto ntStatus = g_winapiApiTable->RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, FALSE, &bAdjustPrivRet);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "RtlAdjustPrivilege fail! Ntstatus: %p", ntStatus);
	}

	// Routine
	InitTestFunctions();

	return true;
}

bool CNM_App::Initialize(bool bClient, bool bProtected, LPDWORD pDwErrorCode)
{
	DEBUG_LOG(LL_SYS, "Initialize started! Build Date: %s Time: %s App type: %d", __DATE__, __TIME__, m_nmData->GetAppType());

	auto bRet				= false;
	auto InitializeTimer	= CTimer<std::chrono::milliseconds>();

#ifdef _DEBUG
	if (m_nmData->GetAppType() != NM_TESTAPP)
		m_nmFunctions->OpenConsoleWindow();

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	auto dwErrorStep = 0UL;
	if (m_dynamicWinapi->Initialize(&dwErrorStep) == false)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Windows API Engine initilization fail! Step: %u Error: %u\n").crypt_get(), dwErrorStep, LI_FIND(GetLastError)());

		if (pDwErrorCode) *pDwErrorCode = DWORD(-1);
		goto _Complete;
	}

	// Remove old logs
	if (IS_VALID_SMART_PTR(m_nmDirFunctions))
	{
		m_nmDirFunctions->DeleteDirectory(xorstr("NoMercy").crypt_get());
	}

	if (InitSelfProtection(m_nmData->GetAntiModuleInformations()) == false)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Self protection fail! Error: %u\n").crypt_get(), LI_FIND(GetLastError)());

		if (pDwErrorCode) *pDwErrorCode = DWORD(-2);
		goto _Complete;
	}

	if (m_nmData->GetAppType() == NM_SHADOW) // Do NOT initialize others for shadow process
		return true;

	if (CMiniDump::InitMiniDump() == false)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Crash handler initilization fail! Error: %u\n").crypt_get(), LI_FIND(GetLastError)());

		if (pDwErrorCode) *pDwErrorCode = DWORD(-3);
		goto _Complete;
	}

	if (m_nmManualMapHelper->MapSystemModules() == false)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Manual mapper initilization fail! Error: %u\n").crypt_get(), LI_FIND(GetLastError)());

		if (pDwErrorCode) *pDwErrorCode = DWORD(-4);
		goto _Complete;
	}

	if (m_nmScannerInterface->InitializeScanner() == false)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Scanner initilization fail! Error: %u\n").crypt_get(), LI_FIND(GetLastError)());

		if (pDwErrorCode) *pDwErrorCode = DWORD(-5);
		goto _Complete;
	}

	if (m_nmSyscallMgr->BuildSyscalls() == false)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Syscall manager initilization fail! Error: %u\n").crypt_get(), LI_FIND(GetLastError)());

		if (pDwErrorCode) *pDwErrorCode = DWORD(-6);
		goto _Complete;
	}

	if (m_nmNetworkMgr->InitializeNetwork() == false)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Network manager initilization fail! Error: %u\n").crypt_get(), LI_FIND(GetLastError)());

		if (pDwErrorCode) *pDwErrorCode = DWORD(-7);
		goto _Complete;
	}

	// ---

	if (m_nmData->GetAppType() == NM_TESTAPP)
	{
		bRet = InitializeTestMode(bClient, pDwErrorCode);
	}
	else
	{
#if (TEST_MODE == TRUE)
		bRet = InitializeTestMode(bClient, pDwErrorCode);
#else
		bRet = InitializeEx(bClient, bProtected, pDwErrorCode);
#endif
	}

_Complete:
	if (bRet == false && pDwErrorCode && *pDwErrorCode)
	{
		if (m_nmData->GetAppType() == NM_LAUNCHER)	// UAC Check
		{
			if (*pDwErrorCode == INIT_ERR_ELEVATION_FAIL_FIRST || *pDwErrorCode == INIT_ERR_ELEVATION_FAIL_SECOND)
			{
				DEBUG_LOG(LL_SYS, "Game should be start as administrator, Game will be restart!");
				// TODO: Give log msg to user

				if (m_initMgr->RestartAsAdmin() == true)
				{
					CTerminator::TerminateProcess(NtCurrentProcess);
					return true;
				}
				else
				{
					DEBUG_LOG(LL_ERR, "Game restart fail! Last error: %u", LI_FIND(GetLastError)());
				}
			}
		}

		// Send notification to service via pipe
		if (IS_VALID_SMART_PTR(m_pCTOSPipeManager))
		{
			m_pCTOSPipeManager->SendInitFailNotification(*pDwErrorCode, g_winapiApiTable->GetLastError());
		}

		// Parse error message
		auto szMsg = "Initialize | Suspecious event handled! Index: " + std::to_string(*pDwErrorCode) + " Last error: " + std::to_string(LI_FIND(GetLastError)());
	
		// SDK Wrapper
		m_nmSDKHelper->SendMessageToClient(NM_DATA_RECV_SUSPECIOUS_EVENT, szMsg.c_str(), nullptr);

		// Show message
		ParseErrorCode(*pDwErrorCode);

		// File log
		Logf(CUSTOM_LOG_FILENAME, szMsg.c_str());
	}

	DEBUG_LOG(LL_SYS, "Initialization function completed in %lld ms!", InitializeTimer.diff());
	return bRet;
}

// Finalization
bool CNM_App::Finalize(LPDWORD pDwErrorCode) // TODO
{
	Logf(CUSTOM_LOG_FILENAME, xorstr("Finalize routine started! InitRet: %d\n").crypt_get(), m_bAppIsInitiliazed);
	DEBUG_LOG(LL_SYS, "Finalize started!");

	m_bFinalizeTriggered = true;
	
#if 0

#ifdef _DEBUG
	if (m_nmData->GetAppType() != NM_TESTAPP)
		g_winapiApiTable->FreeConsole();
#endif

	/*
	Check app type and forward to their finalize routine
	*/

	// Finalize common routines

	// Network
	if (IS_VALID_SMART_PTR(m_nmNetworkMgr))
	{
		if (m_nmNetworkMgr->IsClientNetwork())
		{
			if (IS_VALID_SMART_PTR(m_nmNetworkMgr->GetClientInstance()))
			{
				boost::system::error_code e;
				m_nmNetworkMgr->GetClientInstance()->Disconnect(e);
			}
		}
		else
		{
			if (IS_VALID_SMART_PTR(m_nmNetworkMgr->GetServerInstance()))
			{
				m_nmNetworkMgr->GetServerInstance()->Shutdown();
			}
		}
	}

	// finalize winapis /* SHOULD BE LAST THING */ 
	m_dynamicWinapi->Finalize();

	if (pDwErrorCode)
		*pDwErrorCode = g_winapiApiTable->GetLastError();

#endif

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();

	if (IsDebuggerPresent())
		DebugBreak();
#endif

	return true;
}

// Initilization
bool CNM_App::ClassInitFailed()
{
#ifdef _DEBUG
	if (!IS_VALID_SMART_PTR(m_logHelper))
	{
		Logf(CUSTOM_LOG_FILENAME, "Log helper initilization fail! Last error: %u\n", LI_FIND(GetLastError)());
		return false;
	}

	DEBUG_LOG(LL_SYS, "m_initMgr:\t%p-%p",			m_initMgr,				m_initMgr.get());
	DEBUG_LOG(LL_SYS, "m_logHelper:\t%p-%p",		m_logHelper,			m_logHelper.get());
	DEBUG_LOG(LL_SYS, "m_threadMgr:\t%p-%p",		m_threadMgr,			m_threadMgr.get());
	DEBUG_LOG(LL_SYS, "m_dynamicWinapi:%p-%p",		m_dynamicWinapi,		m_dynamicWinapi.get());
	DEBUG_LOG(LL_SYS, "m_nmFunctions:\t%p-%p",		m_nmFunctions,			m_nmFunctions.get());
	DEBUG_LOG(LL_SYS, "m_nmData:\t%p-%p",			m_nmData,				m_nmData.get());
	DEBUG_LOG(LL_SYS, "m_nmDirFunctions:%p-%p",		m_nmDirFunctions,		m_nmDirFunctions.get());
	DEBUG_LOG(LL_SYS, "m_nmHwidManager:%p-%p",		m_nmHwidManager,		m_nmHwidManager.get());
	DEBUG_LOG(LL_SYS, "m_nmAccessHelper:%p-%p",		m_nmAccessHelper,		m_nmAccessHelper.get());
	DEBUG_LOG(LL_SYS, "m_nmWatchdog:%p-%p",			m_nmWatchdog,			m_nmWatchdog.get());
	DEBUG_LOG(LL_SYS, "m_nmWMI:%p-%p",				m_nmWMI,				m_nmWMI.get());
	DEBUG_LOG(LL_SYS, "m_nmAnalyser:%p-%p",			m_nmAnalyser,			m_nmAnalyser.get());
	DEBUG_LOG(LL_SYS, "m_nmSelfThreadMgr:%p-%p",	m_nmSelfThreadMgr,		m_nmSelfThreadMgr.get());
	DEBUG_LOG(LL_SYS, "m_nmSelfHooks:%p-%p",		m_nmSelfHooks,			m_nmSelfHooks.get());
	DEBUG_LOG(LL_SYS, "m_nmNetworkMgr:%p-%p",		m_nmNetworkMgr,			m_nmNetworkMgr.get());
	DEBUG_LOG(LL_SYS, "m_nmSDKHelper:%p-%p",		m_nmSDKHelper,			m_nmSDKHelper.get());
	DEBUG_LOG(LL_SYS, "m_pStackHelper:%p-%p",		m_pStackHelper,			m_pStackHelper.get());
	DEBUG_LOG(LL_SYS, "m_nmPluginMgr:%p-%p",		m_nmPluginMgr,			m_nmPluginMgr.get());
	DEBUG_LOG(LL_SYS, "m_nmManualMapHelper:%p-%p",	m_nmManualMapHelper,	m_nmManualMapHelper.get());
	DEBUG_LOG(LL_SYS, "m_nmScannerInterface:%p-%p", m_nmScannerInterface,	m_nmScannerInterface.get());
	DEBUG_LOG(LL_SYS, "m_nmQuarentineMgr:%p-%p",	m_nmQuarentineMgr,		m_nmQuarentineMgr.get());
	DEBUG_LOG(LL_SYS, "m_nmSyscallMgr:%p-%p",		m_nmSyscallMgr,			m_nmSyscallMgr.get());

	DEBUG_LOG(LL_SYS, "m_pkCheatDetectQueue:%p-%p", g_pkUnprocessedCheatObjects, g_pkUnprocessedCheatObjects.get());
#endif

	return (
		m_initMgr				== nullptr		|| m_initMgr.get()				== nullptr	||
		m_logHelper				== nullptr		|| m_logHelper.get()			== nullptr	||
		m_threadMgr				== nullptr		|| m_threadMgr.get()			== nullptr	||
		m_dynamicWinapi			== nullptr		|| m_dynamicWinapi.get()		== nullptr	||
		m_nmFunctions			== nullptr		|| m_nmFunctions.get()			== nullptr	||
		m_nmData				== nullptr		|| m_nmData.get()				== nullptr	||
		m_nmDirFunctions		== nullptr		|| m_nmDirFunctions.get()		== nullptr	||
		m_nmHwidManager			== nullptr		|| m_nmHwidManager.get()		== nullptr	||
		m_nmAccessHelper		== nullptr		|| m_nmAccessHelper.get()		== nullptr	||
		m_nmWatchdog			== nullptr		|| m_nmWatchdog.get()			== nullptr	||
		m_nmWMI					== nullptr		|| m_nmWMI.get()				== nullptr	||
		m_nmAnalyser			== nullptr		|| m_nmAnalyser.get()			== nullptr	||
		m_nmSelfThreadMgr		== nullptr		|| m_nmSelfThreadMgr.get()		== nullptr	||
		m_nmSelfHooks			== nullptr		|| m_nmSelfHooks.get()			== nullptr  ||
		m_nmNetworkMgr			== nullptr		|| m_nmNetworkMgr.get()			== nullptr	||
		m_nmSDKHelper			== nullptr		|| m_nmSDKHelper.get()			== nullptr	||
		m_pStackHelper			== nullptr		|| m_pStackHelper.get()			== nullptr	||
		m_nmPluginMgr			== nullptr		|| m_nmPluginMgr.get()			== nullptr	||
		m_nmManualMapHelper		== nullptr		|| m_nmManualMapHelper.get()	== nullptr	||
		m_nmScannerInterface	== nullptr		|| m_nmScannerInterface.get()	== nullptr	||
		m_nmQuarentineMgr		== nullptr		|| m_nmQuarentineMgr.get()		== nullptr	||
		m_nmSyscallMgr			== nullptr		|| m_nmSyscallMgr.get()			== nullptr	||

		g_pkUnprocessedCheatObjects == nullptr	|| g_pkUnprocessedCheatObjects.get() == nullptr
	);
}

bool CNM_App::InitializeClasses(EAppTypes eAppType)
{
	auto bCreateDir = LI_FIND(CreateDirectoryA)(xorstr("NoMercy").crypt_get(), NULL);
	if (!bCreateDir && LI_FIND(GetLastError)() != ERROR_ALREADY_EXISTS)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("InitializeClasses | CreateDirectoryA fail! Error: %u\n").crypt_get(), LI_FIND(GetLastError)());
		return false;
	}

	auto bClient			= true;
	auto szLoggerName		= std::string("");

	switch (eAppType)
	{
		case NM_CLIENT:
		case NM_TESTAPP:
		{
			szLoggerName	= xorstr("NoMercy").crypt_get();
		} break;

		case NM_SERVER:
		{
			bClient			= false;
			szLoggerName	= xorstr("NoMercyServer").crypt_get();
		} break;

		case NM_SHADOW:
		{
			szLoggerName	= xorstr("NoMercyShadow").crypt_get();
		} break;

		case NM_LAUNCHER:
		{
			szLoggerName	= xorstr("NoMercyLauncher").crypt_get();
		} break;

		case NM_SERVICE:
		{
			szLoggerName	= xorstr("NoMercyService").crypt_get();
		} break;

		case NM_HELPERAPP:
		{
			szLoggerName	= xorstr("NoMercyHelper").crypt_get();
		} break;

		default:
			DEBUG_LOG(LL_CRI, "Unknown app type: %d", eAppType);
			return false;
	}

	auto szPID						= std::to_string(LI_FIND(GetCurrentProcessId)());
	std::string szLoggerFileName	= xorstr("NoMercy\\").crypt_get() + szLoggerName + xorstr("_").crypt_get() + szPID + xorstr(".log").crypt_get();

	m_initMgr				= std::make_shared<CNM_InitMgr>();
	m_logHelper				= std::make_shared<CLogger>(szLoggerName, szLoggerFileName.c_str());
	m_threadMgr				= std::make_shared<CThreadManager>();
	m_dynamicWinapi			= std::make_shared<CDynamicWinapi>();
	m_nmFunctions			= std::make_shared<CFunctions>();
	m_nmData				= std::make_shared<CData>(eAppType);
	m_nmDirFunctions		= std::make_shared<CDirFunctions>();
	m_nmHwidManager			= std::make_shared<CHwidManager>();
	m_nmAccessHelper		= std::make_shared<CAccess>();
	m_nmWatchdog			= std::make_shared<CWatchdog>();
	m_nmWMI					= std::make_shared<CWMI>();
	m_nmAnalyser			= std::make_shared<CAnalyser>();
	m_nmSelfThreadMgr		= std::make_shared<CSelfThreadManager>();
	m_nmSelfHooks			= std::make_shared<CSelfApiHooks>();
	m_nmNetworkMgr			= std::make_shared<CNetwork>(bClient);
	m_nmSDKHelper			= std::make_shared<CNM_SDK_Helper>();
	m_pStackHelper			= std::make_shared<CStackTrace>();
	m_nmPluginMgr			= std::make_shared<CPlugin>();
	m_nmManualMapHelper		= std::make_shared<CManualMap>();
	m_nmScannerInterface	= std::make_shared<IScanner>();
	m_nmQuarentineMgr		= std::make_shared<CQuarentine>();
	m_nmSyscallMgr			= std::make_shared<CSyscallHelper>();

	g_pkUnprocessedCheatObjects = std::make_shared<CCheatDetectQueue>();
	return true;
}

// Pre initilization
bool CNM_App::PreInitializeEx(EAppTypes eAppType)
{
	__try
	{
		return InitializeClasses(eAppType);
	}
	__except (CExceptionHandlers::OnExceptionThrowed(GetExceptionInformation()))
	{
//		Logf(CUSTOM_LOG_FILENAME, xorstr("PreInitialize fail!\n").crypt_get());
	}

	return true;
}

bool CNM_App::PreInitialize(EAppTypes eAppType, LPDWORD pDwErrorCode)
{
	// Init classes..
	if (PreInitializeEx(eAppType) == false || ClassInitFailed() == true)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Classes allocation failed!\n").crypt_get());

		if (pDwErrorCode)
			*pDwErrorCode = LI_FIND(GetLastError)();

		return false;
	}

	return true;
}


// Self module protection
bool CNM_App::InitSelfProtection(LPVOID pModuleInfo)
{
	DEBUG_LOG(LL_SYS, "Self protection routine has been started!");

	auto p = reinterpret_cast<ANTI_MODULE_INFO *>(pModuleInfo);
	if (p)
	{
		auto hModule = reinterpret_cast<HMODULE>(p->BaseAddress);
		if (!hModule)
		{
			DEBUG_LOG(LL_CRI, "WHO AM I????");
			return true;
		}

		if (CSelfProtection::InitializeAntiDump(hModule) == false)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_ANTI_DUMP_INIT_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		CSelfProtection::HideModuleLinks(hModule);
		CSelfProtection::ProtectSelfPE(hModule);

		DEBUG_LOG(LL_SYS, "Self protection routine completed!");
		return true;
	}

	DEBUG_LOG(LL_ERR, "Self protection routine fail!");
	return false;
}


/// Network callbacks

// Backend connection callbacks
void CNM_App::OnBackEndDisconnected()
{
	if (m_bFinalizeTriggered)
		return;

	DEBUG_LOG(LL_SYS, "Disconnect callback triggered!");

	m_bConnectionDisconnected = true;

	// ...
}

void CNM_App::OnBackEndConnected()
{
	DEBUG_LOG(LL_SYS, "Connect callback triggered!");

	if (m_bConnectionDisconnected == true)
	{
		DEBUG_LOG(LL_ERR, "Network connection has disconnected");
		return;
	}

	if (!m_nmNetworkMgr->IsNetworkInitialized())
	{
		DEBUG_LOG(LL_ERR, "Network not yet initialized!");
		return;
	}

	if (m_nmNetworkMgr->NetServiceHasStopped())
	{
		DEBUG_LOG(LL_ERR, "Network service has stopped!");
		return;
	}

	m_bConnectedToBackEnd = true;

	// create queue watcher thread
	m_nmNetworkMgr->GetClientInstance()->CreateQueueWatcherThread();
}


// Error parser
bool CNM_App::ParseErrorCode(DWORD dwErrorCode)
{
	auto szMessage = std::string("");

	switch (dwErrorCode)
	{
		case INIT_ERR_SUCCESS:
		{
			szMessage = xorstr("Initilization succesfully completed!").crypt_get();
		} break;

		default:
		{
			szMessage = m_nmFunctions->CreateString(xorstr("Init - Unknown error handled! App: %d(%s): Error ID: %u").crypt_get(), 
				m_nmData->GetAppType(), GetAppTypeName(m_nmData->GetAppType()).c_str(), dwErrorCode);
		} break;
	}

	DEBUG_LOG(LL_ERR, szMessage.c_str());
	m_nmFunctions->ServiceMessageBox2(xorstr("NoMercy Error | Init").crypt_get(), szMessage.c_str(), MB_ICONERROR);

	return true;
}

// Close handler
void CNM_App::OnCloseRequest(EExitErrorCodes ullErrorCode, DWORD dwSystemErrorCode, LPVOID lpParam)
{
	if (m_bIsCloseTriggered)
	{
		DEBUG_LOG(LL_CRI, "Another close function request aleady works! Current code: %u Current system error: %u", ullErrorCode, dwSystemErrorCode);
		return;
	}
	m_bIsCloseTriggered = true;

	auto szMessageText = std::string("");

	switch (ullErrorCode)
	{
		case EXIT_ERR_CTOS_PIPE_ALLOC_FAIL:
		{
			szMessageText = m_nmFunctions->CreateString(xorstr("Client to Service communication setup fail! Error: %u").crypt_get(), dwSystemErrorCode);
		} break;

		case EXIT_ERR_CTOS_PIPE_PING_FAIL:
		{
			szMessageText = m_nmFunctions->CreateString(xorstr("Client to Service communication check fail! Error: %u").crypt_get(), dwSystemErrorCode);
		} break;

		case EXIT_ERR_CTOD_COMM_ALLOC_FAIL:
		{
			szMessageText = m_nmFunctions->CreateString(xorstr("Client to Driver communication setup fail! Error: %u").crypt_get(), dwSystemErrorCode);
		} break;

		case EXIT_ERR_CTOD_COMM_PING_FAIL:
		{
			szMessageText = m_nmFunctions->CreateString(xorstr("Client to Driver communication check fail! Error: %u").crypt_get(), dwSystemErrorCode);
		} break;

		default:
		{
			szMessageText = m_nmFunctions->CreateString(xorstr("Unknown error handled! App: %d(%s) Error ID: %u System error: %u").crypt_get(),
				m_nmData->GetAppType(), GetAppTypeName(m_nmData->GetAppType()).c_str(), static_cast<uint32_t>(ullErrorCode), dwSystemErrorCode);
		} break;
	}

	if (m_nmData->GetAppType() == NM_SERVICE)
	{
		// Send notification to helper via pipe
		if (IS_VALID_SMART_PTR(m_pSTOHPipeManager))
		{
			m_pSTOHPipeManager->SendServiceCloseRequest(static_cast<uint32_t>(ullErrorCode), dwSystemErrorCode);
		}
	}
	else if (m_nmData->GetAppType() == NM_SHADOW)
	{
		// TODO
	}
	else
	{
		// Send notification to service via pipe
		if (IS_VALID_SMART_PTR(m_pCTOSPipeManager))
		{
			m_pCTOSPipeManager->SendCloseRequest(static_cast<uint32_t>(ullErrorCode), dwSystemErrorCode);
		}
	}

	// File log
	Logf(CUSTOM_LOG_FILENAME, szMessageText.c_str());

	// Messsage box
	auto szMsgBoxTitle = m_nmFunctions->CreateString(xorstr("NoMercy Error").crypt_get(), m_nmData->GetAppType());
	m_nmFunctions->ServiceMessageBox2(szMsgBoxTitle.c_str(), szMessageText, MB_ICONERROR);

	// Terminate
	CTerminator::TerminateProcess(NtCurrentProcess);

	// BSOD
	m_nmFunctions->InvokeBSOD();
}

#pragma optimize("", on)

