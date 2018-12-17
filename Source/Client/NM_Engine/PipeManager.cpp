#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "PipeManager.h"
#include "NamedPipe.h"
#include "Defines.h"

typedef struct _IO_THREAD_PARAMS
{
	DWORD dwThreadId;
	HANDLE hThread;
} SIOThreadParams, *PIOThreadParams;
static auto gs_vIOThreads = std::vector<std::shared_ptr<SIOThreadParams>>();

CPipeManager::CPipeManager() :
	m_szPipeName(""), m_dwTick(0UL), m_hServerWatcherThread(INVALID_HANDLE_VALUE)
{
}

CPipeManager::CPipeManager(const std::string & szPipeName, DWORD dwTick) :
	m_szPipeName(szPipeName), m_dwTick(dwTick), m_hServerWatcherThread(INVALID_HANDLE_VALUE)
{
	gs_vIOThreads.clear();
	m_vServerPipes.clear();
}

// ---

bool CPipeManager::InitPipeManagerClientside()
{
	// Check base class
	m_pClientPipe = std::make_shared<CNamedPipe>(m_szPipeName);
	if (!IS_VALID_SMART_PTR(m_pClientPipe))
	{
		DEBUG_LOG(LL_ERR, "Named pipe helper pointer is null!");
		return false;
	}
	DEBUG_LOG(LL_SYS, "Pipe helper class succesfully allocated!");

	// Await server
	if (m_pClientPipe->IsServerAvailable(5000 /* NMPWAIT_WAIT_FOREVER */) == false)
	{
		DEBUG_LOG(LL_ERR, "Pipe wait fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Pipe server is available!");

	// Create pipe handle
	if (m_pClientPipe->Open(GENERIC_READ | GENERIC_WRITE, NULL) == false)
	{
		DEBUG_LOG(LL_ERR, "Pipe open fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Pipe handle succesfully created!");

	if (m_pClientPipe->SetMode(TRUE, TRUE) == false)
	{
		DEBUG_LOG(LL_ERR, "Pipe set mode fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Pipe clientside initilization completed!");
	return true;
}

bool CPipeManager::DestroyPipeManagerClientside()
{
	if (m_pClientPipe->IsOpen() == false)
	{
		DEBUG_LOG(LL_ERR, "Pipe is not open!");
		return false;
	}

	if (m_pClientPipe->Flush() == false)
	{
		DEBUG_LOG(LL_ERR, "Pipe flush fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	if (m_pClientPipe->Close() == false)
	{
		DEBUG_LOG(LL_ERR, "Pipe can not closed! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	m_pClientPipe->Detach();
	DEBUG_LOG(LL_SYS, "Pipe clientside finalization completed!");
	return true;
}

// ---

bool CPipeManager::SendPing()
{
	DEBUG_LOG(LL_SYS, "Ping sent!");

	SPipeRequest req = { PIPE_HEADER_NAME, HEADER_PIPE_CMD_PING };
	SPipeResponse res;

	Request(req, res);

	m_pClientPipe->Flush();
	return res.nStatus;
}

bool CPipeManager::SendGameLaunchNotification(const std::string & szAppName, const std::string & szCommandLine, bool bForceCloseWhenFail)
{
	DEBUG_LOG(LL_SYS, "Game launch notification sent! App name: %s Command Line: %s Force: %d", szAppName.c_str(), szCommandLine.c_str(), bForceCloseWhenFail);

	ReqProcessName req;

	strcpy(req.szMagic, PIPE_HEADER_NAME);
	req.nCommand = HEADER_PIPE_CMD_GAMELAUNCH;
	strcpy(req.szProcessName, szAppName.c_str());
	strcpy(req.szCommandLine, szCommandLine.c_str());
	req.bForceCloseWhenFail = bForceCloseWhenFail;

	SPipeResponse res;
	Request(req, res);

	m_pClientPipe->Flush();
	return res.nStatus;
}

bool CPipeManager::SendClientInformation(DWORD dwProcessId, DWORD dwParentProcessId, DWORD dwAppCode, DWORD dwGameCode, const std::string & szPath, 
	const std::string & szCommandline, BOOL bCloseOnExit, BOOL bIsX64, DWORD dwNoMercyVersion)
{
	DEBUG_LOG(LL_SYS, "Client informations sent! PID: %u Parent: %u App: %u Game: %u Path: %s Cmd: %s Params: %d-%d-%d",
		dwProcessId, dwParentProcessId, dwAppCode, dwGameCode, szPath.c_str(), szCommandline.c_str(), bCloseOnExit, bIsX64, dwNoMercyVersion);

	ReqGameInformation req;

	strcpy(req.szMagic, PIPE_HEADER_NAME);
	req.nCommand				= HEADER_PIPE_CMD_GAMEINFO;
	req.dwGamePid				= dwProcessId;
	req.dwParentPid				= dwParentProcessId;
	req.dwAppCode				= dwAppCode;
	req.dwGameCode				= dwGameCode;
	req.bCloseOnExit			= bCloseOnExit;
	req.bIsX64					= bIsX64;
	req.dwNoMercyVersion		= dwNoMercyVersion;
	strcpy(req.szPath, szPath.c_str());
	strcpy(req.szCommandLine, szCommandline.c_str());

	SPipeResponse res;
	Request(req, res);

	m_pClientPipe->Flush();
	return res.nStatus;
}

bool CPipeManager::SendServiceLogInformation(int iLevel, const std::string & szContent)
{
	DEBUG_LOG(LL_SYS, "Service log sent!");

	ReqServiceLog req;

	strcpy(req.szMagic, PIPE_HEADER_NAME);
	req.nCommand = HEADER_PIPE_CMD_SERVICELOG;
	req.iLogLevel = iLevel;
	strcpy(req.szLogContent, szContent.c_str());

	SPipeResponse res;
	Request(req, res);

	m_pClientPipe->Flush();
	return res.nStatus;
}

bool CPipeManager::SendInitFailNotification(DWORD dwErrorCode, DWORD dwSystemErrorCode)
{
	DEBUG_LOG(LL_SYS, "Initilization fail notification sent! Error codes: %u-%u", dwErrorCode, dwSystemErrorCode);

	ReqClientInitFail req;

	strcpy(req.szMagic, PIPE_HEADER_NAME);
	req.nCommand = HEADER_PIPE_CMD_CLIENTINITFAIL;
	req.dwErrorCode = dwErrorCode;
	req.dwSystemErrorCode = dwSystemErrorCode;

	SPipeResponse res;
	Request(req, res);

	m_pClientPipe->Flush();
	return res.nStatus;
}

bool CPipeManager::SendCloseRequest(DWORD dwErrorCode, DWORD dwSystemErrorCode)
{
	DEBUG_LOG(LL_SYS, "Close request sent! Error codes: %u-%u", dwErrorCode, dwSystemErrorCode);

	ReqClientCloseRequest req;

	strcpy(req.szMagic, PIPE_HEADER_NAME);
	req.nCommand = HEADER_PIPE_CMD_CLIENTCLOSEREQ;
	req.dwErrorCode = dwErrorCode;
	req.dwSystemErrorCode = dwSystemErrorCode;

	SPipeResponse res;
	Request(req, res);

	m_pClientPipe->Flush();
	return res.nStatus;
}

bool CPipeManager::SendServiceCloseRequest(DWORD dwErrorCode, DWORD dwSystemErrorCode)
{
	DEBUG_LOG(LL_SYS, "Service close request sent! Error codes: %u-%u", dwErrorCode, dwSystemErrorCode);

	ReqServiceCloseRequest req;

	strcpy(req.szMagic, PIPE_HEADER_NAME);
	req.nCommand = HEADER_PIPE_CMD_SERVICECLOSEREQ;
	req.dwErrorCode = dwErrorCode;
	req.dwSystemErrorCode = dwSystemErrorCode;

	SPipeResponse res;
	Request(req, res);

	m_pClientPipe->Flush();
	return res.nStatus;
}

// ---

bool CPipeManager::DestroyPipeManagerServerside()
{
	if (gs_vIOThreads.empty() == false)
	{
		for (const auto & pCurrThread : gs_vIOThreads)
		{
			if (IS_VALID_SMART_PTR(pCurrThread))
			{
				if (IS_VALID_HANDLE(pCurrThread->hThread))
				{
					g_winapiApiTable->TerminateThread(pCurrThread->hThread, 0);
					g_winapiApiTable->CloseHandle(pCurrThread->hThread);
				}
			}
		}
	}
	gs_vIOThreads.clear();

	if (m_vServerPipes.empty() == false)
	{
		for (const auto & pCurrPipe : m_vServerPipes)
		{
			if (pCurrPipe->IsOpen())
			{
				pCurrPipe->Flush();
				pCurrPipe->DisconnectClient();
				pCurrPipe->Close();
				pCurrPipe->Detach();
			}
		}
	}

	g_winapiApiTable->TerminateThread(m_hServerWatcherThread, 0);
	g_winapiApiTable->CloseHandle(m_hServerWatcherThread);
	return true;
}

typedef struct _IO_THREAD_DATA
{
	CNamedPipe * pPipeHelper;
	DWORD dwTick;
} SIOThreadData, *PIOThreadData;

DWORD WINAPI IORoutine(LPVOID lpParam)
{
	DEBUG_LOG(LL_SYS, "IO routine started! TID: %u", g_winapiApiTable->GetCurrentThreadId());

	auto lpIOParam = reinterpret_cast<SIOThreadData*>(lpParam);
	if (!lpIOParam || !lpIOParam->pPipeHelper)
	{
		DEBUG_LOG(LL_ERR, "IO routine null param!");
		return 0;
	}
	DEBUG_LOG(LL_SYS, "IO routine params processed! TID: %u Pipe helper: %p Tick: %u", g_winapiApiTable->GetCurrentThreadId(), lpIOParam->pPipeHelper, lpIOParam->dwTick);

	try
	{
		BYTE pData[MAX_PACKET_SIZE];
		while (true)
		{
			if (lpIOParam->pPipeHelper->Read(pData) == false)
			{
				auto dwError = g_winapiApiTable->GetLastError();
				if (dwError == ERROR_BROKEN_PIPE)
				{
					DEBUG_LOG(LL_ERR, "IO Client disconnected.");
					break;
				}
				else if (!lpIOParam->pPipeHelper->Get() || g_nmApp->DynamicWinapiInstance()->IsValidHandle(lpIOParam->pPipeHelper->Get()) == false)
				{
					DEBUG_LOG(LL_ERR, "Client has closed the connection or pipe got broken");
					break;
				}
				else
				{
					DEBUG_LOG(LL_ERR, "IO ReadFile fail! Error: %u", dwError);
					break;
				}
			}
			DEBUG_LOG(LL_SYS, "Request received!");

			if (pData[0] != 'N' || pData[1] != 'M' || pData[2] != 'C' || pData[3] != 'Y')
			{
				DEBUG_LOG(LL_ERR, "Magic check fail, Unknown packet! Last Error: %u", g_winapiApiTable->GetLastError());
				continue;
			}

			auto bHandled = false;
			auto pCommand = ((SPipeRequest*)(pData))->nCommand;
			DEBUG_LOG(LL_SYS, "Accepted request: r->Command: 0x%X", pCommand);

			ReqProcessName			* glData	= nullptr;
			ReqGameInformation		* giData	= nullptr;
			ReqServiceLog			* slData	= nullptr;
			ReqClientInitFail		* cifData	= nullptr;
			ReqClientCloseRequest	* ccrData	= nullptr;
			ReqServiceCloseRequest	* scrData	= nullptr;

			// Handle & process
			switch (pCommand)
			{
				case HEADER_PIPE_CMD_PING:
				{
					DEBUG_LOG(LL_TRACE, "Pinging back");
					bHandled = true;
				} break;

				case HEADER_PIPE_CMD_GAMELAUNCH:
				{
					glData = (ReqProcessName*)pData;

					DEBUG_LOG(LL_SYS, "Game launch notification received! Process name: %s Command Line: %s", glData->szProcessName, glData->szCommandLine);

					g_nmApp->OnLaunchGameNotification(glData->szProcessName, glData->szCommandLine, glData->bForceCloseWhenFail);

					DEBUG_LOG(LL_SYS, "Game launch callback called!");
					bHandled = true;
				} break;

				case HEADER_PIPE_CMD_GAMEINFO:
				{
					giData = (ReqGameInformation*)pData;

					DEBUG_LOG(LL_SYS, "Game information received!");

					g_nmApp->OnGameInformationNotification(giData->dwGamePid, giData->dwParentPid, giData->dwAppCode, giData->dwGameCode, giData->szPath, 
						giData->szCommandLine, giData->bCloseOnExit, giData->bIsX64, giData->dwNoMercyVersion);

					DEBUG_LOG(LL_SYS, "Game information callback called!");
					bHandled = true;
				} break;

				case HEADER_PIPE_CMD_SERVICELOG:
				{
					slData = (ReqServiceLog*)pData;;

					DEBUG_LOG(LL_SYS, "Service log information received!");

					g_nmApp->OnServiceLog(slData->iLogLevel, slData->szLogContent);

					DEBUG_LOG(LL_SYS, "Service log callback called!");
					bHandled = true;

				} break;

				case HEADER_PIPE_CMD_CLIENTINITFAIL:
				{
					cifData = (ReqClientInitFail*)pData;;

					DEBUG_LOG(LL_SYS, "Client init fail notification received!");

					g_nmApp->OnClientInitilizationFailNotification(cifData->dwErrorCode, cifData->dwSystemErrorCode);

					DEBUG_LOG(LL_SYS, "Client init fail callback called!");
					bHandled = true;

				} break;

				case HEADER_PIPE_CMD_CLIENTCLOSEREQ:
				{
					ccrData = (ReqClientCloseRequest*)pData;;

					DEBUG_LOG(LL_SYS, "Client close request received!");

					g_nmApp->OnClientCloseRequestNotification(cifData->dwErrorCode, cifData->dwSystemErrorCode);

					DEBUG_LOG(LL_SYS, "Client close request callback called!");
					bHandled = true;

				} break;

				case HEADER_PIPE_CMD_SERVICECLOSEREQ:
				{
					scrData = (ReqServiceCloseRequest*)pData;;

					DEBUG_LOG(LL_SYS, "Service close request received!");

					g_nmApp->OnServiceCloseRequestNotification(scrData->dwErrorCode, scrData->dwSystemErrorCode);

					DEBUG_LOG(LL_SYS, "Service close request callback called!");
					bHandled = true;

				} break;
			}

			// Send response
			if (bHandled)
			{
//				DEBUG_LOG(LL_SYS, "Response send routine has been started!");

				SPipeResponse res;
				res.nStatus = HEADER_PIPE_STATUS_SUCCESS;
				auto bPongRet = lpIOParam->pPipeHelper->Write(res);
				if (!bPongRet)
				{
					DEBUG_LOG(LL_ERR, "Reply can not send! Error: %u", g_winapiApiTable->GetLastError());
				}

//				DEBUG_LOG(LL_SYS, "Response succesfully sent!");
				
				lpIOParam->pPipeHelper->Flush();

//				DEBUG_LOG(LL_SYS, "Pipe flush'd!");
			}

//			ZeroMemory(&pData, MAX_PACKET_SIZE);
			g_winapiApiTable->Sleep(lpIOParam->dwTick);
		}
	}
	catch (std::exception & e)
	{
#ifdef _DEBUG
		DEBUG_LOG(LL_ERR, "Exception occured: %s (%u)", e.what(), g_winapiApiTable->GetLastError());
#else
		UNREFERENCED_PARAMETER(e);
#endif
	}

	for (const auto & pCurrThread : gs_vIOThreads)
	{
		if (IS_VALID_SMART_PTR(pCurrThread))
		{
			if (pCurrThread->dwThreadId == GetCurrentThreadId())
			{
				g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(pCurrThread->hThread);
				gs_vIOThreads.erase(std::remove(gs_vIOThreads.begin(), gs_vIOThreads.end(), pCurrThread), gs_vIOThreads.end());
			}
		}
	}

	if (lpIOParam->pPipeHelper)
	{
		if (lpIOParam->pPipeHelper->IsOpen())
		{
			lpIOParam->pPipeHelper->Flush();
			lpIOParam->pPipeHelper->DisconnectClient();
			lpIOParam->pPipeHelper->Close();
			lpIOParam->pPipeHelper->Detach();
		}

		delete lpIOParam->pPipeHelper;
		lpIOParam->pPipeHelper = nullptr;
	}

	if (lpIOParam)
	{
		delete lpIOParam;
		lpIOParam = nullptr;
	}

	return 0;
}


DWORD CPipeManager::PipeServerWatcherRoutine(void)
{
	DEBUG_LOG(LL_SYS, "Pipe recv routine started!");

	try
	{
		while (true)
		{
			DEBUG_LOG(LL_SYS, "Creating pipe");

			auto pNamedPipe = new CNamedPipe(m_szPipeName);
			if (!pNamedPipe)
			{
				DEBUG_LOG(LL_ERR, "Named pipe helper class allocation fail! Error: %u", g_winapiApiTable->GetLastError());
				return 0;
			}
			DEBUG_LOG(LL_SYS, "Pipe helper allocated!");

			if (pNamedPipe->Create(PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, MAX_PACKET_SIZE, MAX_PACKET_SIZE, 0) == false)
			{
				DEBUG_LOG(LL_ERR, "Pipe can not created! Error: %u", g_winapiApiTable->GetLastError());
				return 0;
			}
			DEBUG_LOG(LL_SYS, "Pipe: %s has been created, awaiting client", m_szPipeName.c_str());

			if (pNamedPipe->ConnectClient(NULL))
			{
				DEBUG_LOG(LL_SYS, "Client has connected");

				auto pIOParam = new SIOThreadData();
				if (pIOParam)
				{
					DEBUG_LOG(LL_SYS, "IO Param created: %p", pIOParam);

					pIOParam->pPipeHelper	= pNamedPipe;
					pIOParam->dwTick		= m_dwTick;

					auto dwThreadId = 0UL;
					auto hIOThread = g_winapiApiTable->CreateThread(nullptr, 0, IORoutine, pIOParam, 0, &dwThreadId);
					if (IS_VALID_HANDLE(hIOThread))
					{
						DEBUG_LOG(LL_SYS, "IO Thread created: %p(%u)", hIOThread, dwThreadId);

						auto pIOParams = std::make_shared<SIOThreadParams>();
						if (IS_VALID_SMART_PTR(pIOParams))
						{
							pIOParams->dwThreadId	= dwThreadId;
							pIOParams->hThread		= hIOThread;

							gs_vIOThreads.push_back(pIOParams);
						}
					}
					else
					{
						DEBUG_LOG(LL_ERR, "IO thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());

						if (pNamedPipe->IsOpen())
						{
							pNamedPipe->Flush();
							pNamedPipe->DisconnectClient();
							pNamedPipe->Close();
							pNamedPipe->Detach();
						}

						if (pIOParam)
						{
							delete pIOParam;
							pIOParam = nullptr;
						}

						if (pNamedPipe)
						{
							delete pNamedPipe;
							pNamedPipe = nullptr;
						}
					}
				}
			}

			g_winapiApiTable->Sleep(m_dwTick);
		}
	}
	catch (std::exception & e) 
	{
#ifdef _DEBUG
		DEBUG_LOG(LL_ERR, "Exception occured: %s (%u)", e.what(), GetLastError());
#else
		UNREFERENCED_PARAMETER(e);
#endif
	}
	catch (...)
	{
		DEBUG_LOG(LL_ERR, "Unhandled exception occured!");
	}

	DEBUG_LOG(LL_SYS, "Closing pipe..");
	DestroyPipeManagerServerside();
	return 0;
}

DWORD WINAPI CPipeManager::StartServerWatcherThread(LPVOID lpParam)
{
	auto This = (CPipeManager*)lpParam;
	return This->PipeServerWatcherRoutine();
}

bool CPipeManager::CreateServerWatcherThread()
{
	DWORD dwThreadId = 0;
	m_hServerWatcherThread = g_winapiApiTable->CreateThread(nullptr, 0, StartServerWatcherThread, (void*)this, 0, &dwThreadId);
	if (!IS_VALID_HANDLE(m_hServerWatcherThread))
	{
		DEBUG_LOG(LL_ERR, "Server watcher thread can not created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Server watcher thread created! TID: %u", dwThreadId);
	return true;
}


