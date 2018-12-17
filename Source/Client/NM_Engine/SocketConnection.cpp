#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Defines.h"
#include "SocketConnection.h"

CNetwork::CNetwork(bool bClient) :
	m_bIsClient(bClient), m_bIsInitialized(false)
{
	m_szIPAddress	= BACKEND_SERVER_IP;
	m_wPort			= BACKEND_SERVER_PORT;
	m_wPort2		= BACKEND_SERVER_PORT_2;
}

bool CNetwork::InitializeNetwork()
{
	auto szPID = std::to_string(g_winapiApiTable->GetCurrentProcessId());
	std::string szLoggerName = xorstr("NoMercy_Network").crypt_get();
	auto szLoggerFileName = xorstr("NoMercy/").crypt_get() + szLoggerName + xorstr("_").crypt_get() + szPID + xorstr(".log").crypt_get();

#ifdef _DEBUG
	m_logHelper = std::make_shared<CLogger>(szLoggerName, szLoggerFileName.c_str());
	if (!IS_VALID_SMART_PTR(m_logHelper))
	{
		DEBUG_LOG(LL_ERR, "Network logger class can NOT initialized!");
//		g_nmApp->OnCloseRequest(EXIT_ERR_NET_LOGGER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	return true;
}

void CNetwork::RegisterOnConnectedCallback(TOnConnectedCallback ConnectedCallbackFunc)
{
	m_ConnectedCallbackFunc = ConnectedCallbackFunc;
}

void CNetwork::SendConnectedSignal()
{
	if (m_ConnectedCallbackFunc)
		m_ConnectedCallbackFunc();
}

void CNetwork::RegisterOnDisconnectedCallback(TOnDisconnectedCallback DisconnectedCallbackFunc)
{
	m_DisconnectedCallbackFunc = DisconnectedCallbackFunc;
}

void CNetwork::SendDisconnectedSignal()
{
	if (m_DisconnectedCallbackFunc)
		m_DisconnectedCallbackFunc();
}

uint16_t CNetwork::CreateChecksum(const void* data, std::size_t length)
{
	std::size_t nStartOffset = length;
	auto pBulk = (unsigned char*)data;
#ifdef _M_X64
	length = std::min(65535ULL, length);
#else
	length = std::min(65535U, length);
#endif

	auto nCheckSum = 0UL;
	for (std::size_t i = nStartOffset; i < length; i++)
		nCheckSum += pBulk[i];

	nCheckSum -= (pBulk[0] + pBulk[1] + pBulk[2] + pBulk[3]);
	auto nShortCheckSum = HIWORD(nCheckSum) + LOWORD(nCheckSum);
	return nShortCheckSum;
}


DWORD WINAPI NetworkRoutine(LPVOID)
{
	NETWORK_LOG(LL_SYS, "Network thread event has been started");

	auto pNetworkTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread = std::shared_ptr<CThread>();
	do
	{
		if (pNetworkTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_NETWORK_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_NETWORKROUTINE);

	} while (!IS_VALID_SMART_PTR(currentThread));

	bool bRet = false;
	if (g_nmApp->NetworkMgrInstance()->IsClientNetwork())
		bRet = g_nmApp->NetworkMgrInstance()->InitializeClientSocket();
	else
		bRet = g_nmApp->NetworkMgrInstance()->InitializeServerSocket();

#if 0
	auto szLog = g_nmApp->FunctionsInstance()->CreateString(xorstr("Network initilization fail!. Error: %d").crypt_get(), g_winapiApiTable->GetLastError());
//	g_nmApp->FunctionsInstance()->CloseProcess(szLog.c_str(), false, "");
#endif

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc5()
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

bool CNetwork::InitSocketConnection()
{
	NETWORK_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)NetworkRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		NETWORK_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_NETWORKROUTINE, NetworkRoutine, ulFuncSize, nullptr, 0, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		NETWORK_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	NETWORK_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}

