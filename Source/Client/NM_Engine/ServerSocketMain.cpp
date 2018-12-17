#include "NM_Index.h"
#include "NM_Main.h"
#include "SocketConnection.h"
#include "SocketServer.h"
#include "Defines.h"
#include "NetLib.h"
#include "ExceptionHandlers.h"

bool CNetwork::InitializeServerSocket()
{
	m_pNetServer = std::make_shared<CNetworkServer>(m_NetService);
	if (!IS_VALID_SMART_PTR(m_pNetServer))
	{
		NETWORK_LOG(LL_SYS, "m_pNetServer can NOT allocated! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	m_bIsInitialized = true;

	try
	{
		m_pNetServer->Init();
		m_pNetServer->Run();
		m_pNetServer->Shutdown();
	}
	catch (std::exception& e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_ERR, "Exception handled: %s", e.what());
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

	m_pNetServer.reset();
	return true;
}
