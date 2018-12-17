#pragma once
#include <string>
#include <vector>
#include <memory>

enum EPrivCodes
{
	SE_MIN_WELL_KNOWN_PRIVILEGE			= 2,
	SE_CREATE_TOKEN_PRIVILEGE			= SE_MIN_WELL_KNOWN_PRIVILEGE,
	SE_ASSIGNPRIMARYTOKEN_PRIVILEGE		= 3,
	SE_LOCK_MEMORY_PRIVILEGE			= 4,
	SE_INCREASE_QUOTA_PRIVILEGE			= 5,
	SE_UNSOLICITED_INPUT_PRIVILEGE		= 6, /* obsolete */
	SE_MACHINE_ACCOUNT_PRIVILEGE		= 6,
	SE_TCB_PRIVILEGE					= 7,
	SE_SECURITY_PRIVILEGE            	= 8,
	SE_TAKE_OWNERSHIP_PRIVILEGE      	= 9,
	SE_LOAD_DRIVER_PRIVILEGE        	= 10,
	SE_SYSTEM_PROFILE_PRIVILEGE     	= 11,
	SE_SYSTEMTIME_PRIVILEGE         	= 12,
	SE_PROF_SINGLE_PROCESS_PRIVILEGE 	= 13,
	SE_INC_BASE_PRIORITY_PRIVILEGE   	= 14,
	SE_CREATE_PAGEFILE_PRIVILEGE    	= 15,
	SE_CREATE_PERMANENT_PRIVILEGE  		= 16,
	SE_BACKUP_PRIVILEGE					= 17,
	SE_RESTORE_PRIVILEGE            	= 18,
	SE_SHUTDOWN_PRIVILEGE           	= 19,
	SE_DEBUG_PRIVILEGE               	= 20,
	SE_AUDIT_PRIVILEGE               	= 21,
	SE_SYSTEM_ENVIRONMENT_PRIVILEGE 	= 22,
	SE_CHANGE_NOTIFY_PRIVILEGE			= 23,
	SE_REMOTE_SHUTDOWN_PRIVILEGE   		= 24,
	SE_UNDOCK_PRIVILEGE					= 25,
	SE_SYNC_AGENT_PRIVILEGE        		= 26,
	SE_ENABLE_DELEGATION_PRIVILEGE 		= 27,
	SE_MANAGE_VOLUME_PRIVILEGE      	= 28,
	SE_IMPERSONATE_PRIVILEGE        	= 29,
	SE_CREATE_GLOBAL_PRIVILEGE      	= 30,
	SE_MAX_WELL_KNOWN_PRIVILEGE			= SE_CREATE_GLOBAL_PRIVILEGE
};

typedef struct _GAME_LAUNCH_PARAMS
{
	DWORD	dwProcessId;
	HANDLE	hProcess;
	char	szPath[MAX_PATH];
} SGameLaunchParams, *PGameLaunchParams;

typedef struct _GAME_LAUNCH_THREAD_PARAMS
{
	DWORD	dwProcessId;
	DWORD	dwLauncherThreadId;
	bool	bCompleted;
} SGameLaunchThreadParams, *PGameLaunchThreadParams;


class CNM_InitMgr
{
	public:
		CNM_InitMgr();
		~CNM_InitMgr();

		bool	SafeRemoteCheck(LPDWORD pdwErrorCode);
		bool	IsProcessProtected();
		int		CheckElevation();
		bool	RequestPrivilege(ULONG ulPriv);
		bool	RestartAsAdmin();

	protected:
		bool	RemoteCheckRoutine(LPDWORD pdwErrorCode);

	public:
//	private:
		std::string m_szSplashHideOption;
		std::string m_szOpenFaqPageOption;
		std::string m_szProtectedAppOption;

		std::string m_szDriverID;
		std::string m_szDriverDisplayName;
		std::string m_szDriverSymLink;
		std::string m_szServiceID;
		std::string m_szServiceDisplayName;
		std::string m_szServicePipe;
		std::string m_szServiceMutex;
		std::string m_szServiceName;
		std::string m_szLauncherName;
		std::string m_szHelperAppName;
		std::string m_szHelperAppPipe;
		std::string m_szModuleName;
		std::string m_szDriverName_x86;
		std::string m_szDriverName_x64;
		std::string m_szLauncherConfigName;
		std::string m_szRemoteWebURL;
		std::string m_szRemoteWebMainCheckURL;
		std::string m_szRemoteWebLicenseCheckURL;
		std::string m_szRemoteWebChecksumCheckURL;

		std::vector <std::shared_ptr <SGameLaunchParams>>		m_vLaunchedProcesses;
		std::vector <std::shared_ptr <SGameLaunchThreadParams>>	m_vLauncherThreads;
};

