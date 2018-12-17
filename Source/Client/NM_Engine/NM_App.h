#pragma once
#include "NM_InitilizationManager.h"
#include "SpdLog_Wrapper.h"
#include "ThreadManagerHelper.h"
#include "DynamicWinapi.h"
#include "Functions.h"
#include "Data.h"
#include "DirFunctions.h"
#include "HW-Info.h"
#include "Access.h"
#include "Watchdog.h"
#include "WMI.h"
#include "Analyser.h"
#include "NM_SelfThreadManager.h"
#include "Hooks.h"
#include "SocketConnection.h"
#include "WorkQueue.h"
#include "NM_SDK_Manager.h"
#include "StackTrace.h"
#include "Plugin.h"
#include "ServiceCommunication.h"
#include "PipeManager.h"
#include "ErrorCodes.h"
#include "ManualMap.h"
#include "ScannerInterface.h"
#include "Quarentine.h"
#include "SyscallHelper.h"

enum EAppTypes
{
	NM_NULL_APP_TYPE,
	NM_CLIENT,
	NM_SERVER,
	NM_LAUNCHER,
	NM_SERVICE,
	NM_SHADOW,
	NM_TESTAPP,
	NM_HELPERAPP,	
	NM_MASTERSERVER,	 // TODO
	NM_MAX_APP_TYPE
};

static std::string GetAppTypeName(int8_t iAppType)
{
	static std::map <int8_t, std::string> s_mAppTypes =
	{
		{ NM_CLIENT,		xorstr("NM_CLIENT").crypt_get()			},
		{ NM_SERVER,		xorstr("NM_SERVER").crypt_get()			},
		{ NM_LAUNCHER,		xorstr("NM_LAUNCHER").crypt_get()		},
		{ NM_SERVICE,		xorstr("NM_SERVICE").crypt_get()		},
		{ NM_SHADOW,		xorstr("NM_SHADOW").crypt_get()			},
		{ NM_TESTAPP,		xorstr("NM_TESTAPP").crypt_get()		},
		{ NM_HELPERAPP,		xorstr("NM_HELPERAPP").crypt_get()		},
		{ NM_MASTERSERVER,	xorstr("NM_MASTERSERVER").crypt_get()	},
	};

	if (iAppType <= NM_NULL_APP_TYPE || iAppType >= NM_MAX_APP_TYPE)
		return std::string("");

	auto it = s_mAppTypes.find(iAppType);
	if (it == s_mAppTypes.end())
		return std::string(xorstr("Undefined app type: ").crypt_get() + iAppType);

	return it->second;
}


extern std::shared_ptr <CCheatDetectQueue> g_pkUnprocessedCheatObjects;

class CNM_App : public std::enable_shared_from_this <CNM_App>
{
	public:
		CNM_App();
		~CNM_App() = default;

		// Initilziation
		bool PreInitialize(EAppTypes eAppType, LPDWORD pDwErrorCode);
		bool Initialize(bool bClient, bool bProtected, LPDWORD pDwErrorCode);
		bool InitializeClientThreads();
		bool InitializeServiceThreads();
		bool InitializeFilterCallbacks();

		bool OnClientInit(LPDWORD pDwErrorCode);
		bool OnLauncherInit(bool bProtected, LPDWORD pDwErrorCode);
		bool OnServiceInit(bool bProtected, LPDWORD pDwErrorCode);
		bool OnHelperAppInit(LPDWORD pDwErrorCode);

		// Finalization
		bool Finalize(LPDWORD pDwErrorCode);

		void OnClientFinalize();
		void OnLauncherFinalize();
		void OnServiceFinalize();
		void OnHelperFinalize();

		// Routine
		bool InitializeMainCheckThread();
		bool InitCheckModuleModifications();
		bool InitSelfProtection(LPVOID pModuleInfo);

		// Callback
		void OnCloseRequest(EExitErrorCodes ullErrorCode, DWORD dwSystemErrorCode, LPVOID lpParam = nullptr);
		bool OnLaunchGameNotification(const std::string & szProcessName, const std::string & szCommandLine, bool bForceCloseWhenFail = false);
		bool OnServiceCloseRequestNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode);
		bool OnClientInitilizationFailNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode);
		bool OnClientCloseRequestNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode);
		bool OnGameInformationNotification(DWORD dwProcessId, DWORD dwParentProcessId, DWORD dwAppCode, DWORD dwGameCode, const std::string & szPath,
			const std::string & szCommandline, BOOL bCloseOnExit, BOOL bIsX64, DWORD dwNoMercyVersion);
		bool OnServiceLog(int iLevel, const std::string & szContent);
		void OnBackEndConnected();
		void OnBackEndDisconnected();
		void OnServiceMessageHandled(int iMessageID);

		// Set
		void SetManualMapStatus(bool bNewStat)	{ m_bIsMMapLoaded		= bNewStat; };
		void SetInitializeStatus(bool bNewStat)	{ m_bAppIsInitiliazed	= bNewStat; };

		// Get
		auto AppIsInitiliazed()			{ return m_bAppIsInitiliazed;			};
		auto IsConnectedToBackend()		{ return m_bConnectedToBackEnd;			};
		auto ManualMapIsReady()			{ return m_bIsMMapLoaded;				};
		auto GetSelfModuleList()		{ return m_vSelfModuleList;				};
		auto GetHelperProcessId()		{ return m_dwHelperProcessId;			};
		bool GetScreenProtectionStatus(HWND hWnd);

		// Error manager
		bool ParseErrorCode(DWORD dwErrorCode);

		// Utils
		void AddModuleToModuleList(HMODULE hModule)		{ m_vSelfModuleList.push_back(hModule); };
		bool IsModuleExistOnModuleList(HMODULE hModule) { return std::find(m_vSelfModuleList.begin(), m_vSelfModuleList.end(), hModule) != m_vSelfModuleList.end(); };

		HANDLE ServiceRunProgram(HANDLE hTokenOwner, LPCSTR lpFilename, LPCSTR lpArguments, LPCSTR lpDir, LPPROCESS_INFORMATION ProcessInformation, BOOL Inherit, HANDLE hParent);
		
		bool ActivateScreenProtection(HWND hWnd, bool bEnabled);
		void CheckScreenProtection(HWND hWnd);
		void InitScreenProtection(HWND hWnd);

		void TestMsg();

	public:
		auto InitMgrInstance()			{ return m_initMgr;						};
		auto LogHelperInstance()		{ return m_logHelper;					};
		auto ThreadManagerInstance()	{ return m_threadMgr;					};
		auto DynamicWinapiInstance()	{ return m_dynamicWinapi;				};
		auto FunctionsInstance()		{ return m_nmFunctions;					};
		auto DataInstance()				{ return m_nmData;						};
		auto DirFunctionsInstance()		{ return m_nmDirFunctions;				};
		auto HwidManagerInstance()		{ return m_nmHwidManager;				};
		auto AccessHelperInstance()		{ return m_nmAccessHelper;				};
		auto WatchdogInstance()			{ return m_nmWatchdog;					};
		auto WMIInstance()				{ return m_nmWMI;						};
		auto AnalyserInstance()			{ return m_nmAnalyser;					};
		auto SelfThreadMgrInstance()	{ return m_nmSelfThreadMgr;				};
		auto SelfHooksInstance()		{ return m_nmSelfHooks;					};
		auto NetworkMgrInstance()		{ return m_nmNetworkMgr;				};
		auto SDKHelperInstance()		{ return m_nmSDKHelper;					};
		auto StackHelperInstance()		{ return m_pStackHelper;				};
		auto PluginInstance()			{ return m_nmPluginMgr;					};
		auto ManualMapInstance()		{ return m_nmManualMapHelper;			};
		auto ScannerInstance()			{ return m_nmScannerInterface;			};
		auto QuarentineInstance()		{ return m_nmQuarentineMgr;				};
		auto SyscallInstance()			{ return m_nmSyscallMgr;				};

		auto CheatDetectQueue()			{ return g_pkUnprocessedCheatObjects;	};

		auto ClientToDriverCommHelper() { return m_pCTODCommHelper;				};
		auto ClientToServicePipeMgr()	{ return m_pCTOSPipeManager;			};
		auto ServiceToHelperPipeMgr()	{ return m_pSTOHPipeManager;			};
		auto HelperPipeMgr()			{ return m_pHelperPipeManager;			};
		auto ServicePipeMgr()			{ return m_pSvcPipeManager;				};

	protected:
		__forceinline bool ClassInitFailed();
		__forceinline bool InitializeClasses(EAppTypes eAppType);
		__forceinline bool PreInitializeEx(EAppTypes eAppType);
	
		__forceinline bool InitializeEx(bool bClient, bool bProtected, LPDWORD pDwErrorCode);
		__forceinline bool InitializeServer(LPDWORD pDwErrorCode);
		__forceinline bool InitializeTestMode(bool bClient, LPDWORD pDwErrorCode);

		void InitTestFunctions();

	private:
		bool	m_bAppIsInitiliazed;
		bool	m_bConnectedToBackEnd;
		bool	m_bConnectionDisconnected;
		bool	m_bIsMMapLoaded;
		bool	m_bFinalizeTriggered;
		bool	m_bIsCloseTriggered;

		DWORD	m_dwHelperProcessId;

		std::vector		<HMODULE>				m_vSelfModuleList;
		std::map		<HWND, bool>			m_mScreenProtectionStatus;

		std::shared_ptr <CServiceComm>			m_pCTODCommHelper;		// Client to Driver communication helper
		std::shared_ptr <CPipeManager>			m_pCTOSPipeManager;		// Client to Service client pipe manager
		std::shared_ptr <CPipeManager>			m_pSTOHPipeManager;		// Service to Helper client pipe manager
		std::shared_ptr <CPipeManager>			m_pHelperPipeManager;	// Helper app server pipe manager
		std::shared_ptr <CPipeManager>			m_pSvcPipeManager;		// Service app server pipe manager

		std::shared_ptr <CNM_InitMgr>			m_initMgr;
		std::shared_ptr <CLogger>				m_logHelper;
		std::shared_ptr <CThreadManager>		m_threadMgr;
		std::shared_ptr <CDynamicWinapi>		m_dynamicWinapi;
		std::shared_ptr <CFunctions>			m_nmFunctions;
		std::shared_ptr <CData>					m_nmData;
		std::shared_ptr <CDirFunctions>			m_nmDirFunctions;
		std::shared_ptr <CHwidManager>			m_nmHwidManager;
		std::shared_ptr <CAccess>				m_nmAccessHelper;
		std::shared_ptr <CWatchdog>				m_nmWatchdog;
		std::shared_ptr <CWMI>					m_nmWMI;
		std::shared_ptr <CAnalyser>				m_nmAnalyser;
		std::shared_ptr <CSelfThreadManager>	m_nmSelfThreadMgr;
		std::shared_ptr <CSelfApiHooks>			m_nmSelfHooks;
		std::shared_ptr <CNetwork>				m_nmNetworkMgr;
		std::shared_ptr <CNM_SDK_Helper>		m_nmSDKHelper;
		std::shared_ptr <CStackTrace>			m_pStackHelper;
		std::shared_ptr <CPlugin>				m_nmPluginMgr;
		std::shared_ptr <CManualMap>			m_nmManualMapHelper;
		std::shared_ptr <IScanner>				m_nmScannerInterface;
		std::shared_ptr <CQuarentine>			m_nmQuarentineMgr;
		std::shared_ptr <CSyscallHelper>		m_nmSyscallMgr;
};

