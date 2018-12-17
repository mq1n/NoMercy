#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "WorkQueue.h"

IScanner::IScanner()
{
}

IScanner::~IScanner()
{
}

bool IScanner::InitializeScanner()
{
	auto szPID					= std::to_string(g_winapiApiTable->GetCurrentProcessId());
	std::string szLoggerName	= xorstr("NoMercy_Scanner").crypt_get();
	auto szLoggerFileName		= g_nmApp->DirFunctionsInstance()->ExePath() + xorstr("\\NoMercy\\").crypt_get() + szLoggerName + xorstr("_").crypt_get() + szPID + xorstr(".log").crypt_get();

#ifdef _DEBUG
	m_logHelper = std::make_shared<CLogger>(szLoggerName, szLoggerFileName.c_str());
	if (!IS_VALID_SMART_PTR(m_logHelper)) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SCANNER_LOGGER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	SCANNER_LOG(LL_SYS, "Scanner initilization has been started!");

	// TODO build local quarentine db

	return true;
}

bool IScanner::FinalizeScanner()
{
	return true;
}


bool IScanner::BuildLocalDB()
{
	return true;
}

bool IScanner::CheckLocalDB()
{
	return true;
}


bool IScanner::RunFirstTimeScans()
{
	return true;
}

bool IScanner::SendViolationMessageToMasterServer(DWORD dwViolationID, DWORD dwSystemErrorCode, const std::string & szMessage, bool bWait)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	SCANNER_LOG(LL_SYS, "Violation send routine has been started!");

	if (g_nmApp->IsConnectedToBackend() == false)
	{
		SCANNER_LOG(LL_ERR, "Backend is NOT connected yet!");
		return false;
	}

	auto pCheatDetectQueueData = std::make_shared<SCheatDetectQueueData>();
	if (!IS_VALID_SMART_PTR(pCheatDetectQueueData))
	{
		SCANNER_LOG(LL_ERR, "Cheat queue data container can NOT allocated!");
		return false;
	}

	pCheatDetectQueueData->dwViolation = static_cast<DWORD>(dwViolationID);
	pCheatDetectQueueData->dwErrorCode = dwSystemErrorCode;
	strcpy(pCheatDetectQueueData->szMessage, szMessage.c_str());
	// TODO: Screenshot stream

	if (g_pkUnprocessedCheatObjects->InsertObject(pCheatDetectQueueData) == false) // if not success, it's mean already listed item
	{
		SCANNER_LOG(LL_ERR, "Cheat object is already attached to queue!");
		return true;
	}

	if (bWait)
	{
		auto bCompleted = false;
		auto pCheckTimer = CTimer<std::chrono::milliseconds>();
		while (pCheckTimer.diff() < 5000)
		{
			if (g_pkUnprocessedCheatObjects->HasListed(pCheatDetectQueueData) == false && g_pkUnprocessedCheatObjects->HasProcessed(pCheatDetectQueueData))
			{
				bCompleted = true;
				break;
			}
		}

		if (bCompleted)
		{
			SCANNER_LOG(LL_SYS, "Notification succesfully sent to master server!");
//			g_winapiApiTable->Sleep(1000); // Simple wait 1 second for network routine
			return true;
		}
		
		SCANNER_LOG(LL_ERR, "Notification can NOT sent to master server!");
		return false;
	}

	SCANNER_LOG(LL_SYS, "Notification attached to queue!");
	return true;
}

