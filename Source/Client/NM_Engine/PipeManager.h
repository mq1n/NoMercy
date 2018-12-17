#pragma once
#include <Windows.h>
#include "NamedPipe.h"

#define PIPE_HEADER_NAME "NMCY"
#define MAX_PACKET_SIZE 0x3000

enum EPipeCommand : uint64_t
{
	HEADER_PIPE_CMD_INVALID			= 0x0,
	HEADER_PIPE_CMD_PING			= 0x1,
	HEADER_PIPE_CMD_GAMELAUNCH		= 0x2,
	HEADER_PIPE_CMD_GAMEINFO		= 0x3,
	HEADER_PIPE_CMD_SERVICELOG		= 0x4,
	HEADER_PIPE_CMD_CLIENTINITFAIL	= 0x5,
	HEADER_PIPE_CMD_CLIENTCLOSEREQ	= 0x6,
	HEADER_PIPE_CMD_SERVICECLOSEREQ	= 0x7,
};

enum EPipeStatus : signed char
{
	HEADER_PIPE_STATUS_FAIL		= 0x0,
	HEADER_PIPE_STATUS_SUCCESS	= 0x1,
};

// ---

struct SPipeRequest
{
	char szMagic[4 + 1];
	EPipeCommand nCommand;
};

struct SPipeResponse
{
	EPipeStatus nStatus;
};

// ---

struct ReqProcessName : SPipeRequest
{
	CHAR szProcessName[MAX_PATH];
	CHAR szCommandLine[4096];
	BOOL bForceCloseWhenFail;
};

struct ReqGameInformation : SPipeRequest
{
	DWORD	dwGamePid;
	DWORD	dwParentPid;
	DWORD	dwAppCode;
	DWORD	dwGameCode;
	CHAR	szPath[MAX_PATH];
	CHAR	szCommandLine[4096];
	BOOL	bCloseOnExit;
	BOOL	bIsX64;
	DWORD	dwNoMercyVersion;
};

struct ReqServiceLog : SPipeRequest
{
	INT		iLogLevel;
	CHAR	szLogContent[9000];
};

struct ReqClientInitFail : SPipeRequest
{
	DWORD	dwErrorCode;
	DWORD	dwSystemErrorCode;
};

struct ReqClientCloseRequest : SPipeRequest
{
	DWORD	dwErrorCode;
	DWORD	dwSystemErrorCode;
};

struct ReqServiceCloseRequest : SPipeRequest
{
	DWORD	dwErrorCode;
	DWORD	dwSystemErrorCode;
};

// ---

class CPipeManager 
{
	public:
		CPipeManager();
		CPipeManager(const std::string & szPipeName, DWORD dwTick);
		~CPipeManager() = default;
		
		// Server
		bool CreateServerWatcherThread();
		bool DestroyPipeManagerServerside();

		// IO
		bool SendServiceCloseRequest(DWORD dwErrorCode, DWORD dwSystemErrorCode);
		bool SendInitFailNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode);
		bool SendCloseRequest(DWORD dwErrorCode, DWORD dwSystemErrorCode);
		bool SendGameLaunchNotification(const std::string & szAppName, const std::string & szCommandLine, bool bForceCloseWhenFail = false);
		bool SendClientInformation(DWORD dwProcessId, DWORD dwParentProcessId, DWORD dwAppCode, DWORD dwGameCode, const std::string & szPath,
			const std::string & szCommandline, BOOL bCloseOnExit, BOOL bIsX64, DWORD dwNoMercyVersion);
		bool SendServiceLogInformation(int iLevel, const std::string & szContent);
		bool SendPing();

		// Client
		bool InitPipeManagerClientside();
		bool DestroyPipeManagerClientside();

		// others
		template <class T_In, class T_Out> 
		void Request(T_In& in, T_Out& out)
		{
			m_pClientPipe->Write(in);
			m_pClientPipe->Read(out);
		}

		auto GetClientPipe() { return m_pClientPipe; };

	protected:
		DWORD					PipeServerWatcherRoutine(void);
		static DWORD WINAPI		StartServerWatcherThread(LPVOID lpParam);

	private:
		std::string						m_szPipeName;
		DWORD							m_dwTick;

		std::shared_ptr <CNamedPipe>	m_pClientPipe;
		std::vector		<CNamedPipe*>	m_vServerPipes;

		HANDLE							m_hServerWatcherThread;
};

