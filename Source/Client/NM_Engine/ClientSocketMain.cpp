#include "NM_Index.h"
#include "NM_Main.h"
#include "SocketConnection.h"
#include "SocketClient.h"
#include "Defines.h"
#include "NetLib.h"
#include "ExceptionHandlers.h"

bool CNetwork::InitializeClientSocket()
{
	NETWORK_LOG(LL_SYS, "Network client initialize has been started!");

	bool bRet = false;

	m_pNetClient = std::make_shared<CNetworkClient>(m_NetService);
	if (!m_pNetClient || !m_pNetClient.get()) 
	{
		NETWORK_LOG(LL_ERR, "m_pNetClient can NOT allocated! Last error: %u", g_winapiApiTable->GetLastError());
		return bRet;
	}

	try
	{
		bRet = m_pNetClient->Connect(m_szIPAddress, m_wPort);
		if (bRet == false)
		{
			NETWORK_LOG(LL_ERR, "m_pNetClient->Connect(m_wPort) fail! Last error: %u", g_winapiApiTable->GetLastError());

			bRet = m_pNetClient->Connect(m_szIPAddress, m_wPort2);
		}

		if (bRet == false)
		{
			NETWORK_LOG(LL_ERR, "m_pNetClient->Connect(m_wPort2) fail! Last error: %u", g_winapiApiTable->GetLastError());
			return bRet;
		}

		m_bIsInitialized = true;

		m_NetService.Run();
	}
	catch (std::exception& e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_ERR, "Exception handled: %s", e.what());
		CExceptionHandlers::OnExceptionThrowed(nullptr);
#else
		UNREFERENCED_PARAMETER(e);
#endif
	}
	catch (DWORD dwNumber)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_ERR, "Exception handled: %p", dwNumber);
		CExceptionHandlers::OnExceptionThrowed(nullptr);
#else
		UNREFERENCED_PARAMETER(dwNumber);
#endif
	}
	catch (...)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_ERR, "Unhandled exception!");
		CExceptionHandlers::OnExceptionThrowed(nullptr);
#endif
	}

	return bRet;
}
