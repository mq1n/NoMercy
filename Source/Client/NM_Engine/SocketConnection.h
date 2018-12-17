#pragma once
#include "SocketServer.h"
#include "SocketClient.h"

class CNetwork
{
	typedef std::function<void()> TOnConnectedCallback;
	typedef std::function<void()> TOnDisconnectedCallback;

	public:
		CNetwork(bool bClient);
		~CNetwork() = default;

		bool InitializeNetwork();

		void RegisterOnConnectedCallback(TOnConnectedCallback ConnectedCallbackFunc);
		void SendConnectedSignal();

		void RegisterOnDisconnectedCallback(TOnDisconnectedCallback DisconnectedCallbackFunc);
		void SendDisconnectedSignal();

		bool InitSocketConnection(); // Thread

		bool InitializeClientSocket();
		bool InitializeServerSocket();

		auto IsNetworkInitialized() { return m_bIsInitialized;	};
		auto IsClientNetwork()		{ return m_bIsClient;		};

		auto GetServerInstance()	{ return m_pNetServer;				};
		auto GetClientInstance()	{ return m_pNetClient;				};
		auto NetServiceHasStopped()	{ return m_NetService.HasStopped();	};

		uint16_t CreateChecksum(const void* data, std::size_t length);

#ifdef _DEBUG
		auto LogInstance()			{ return m_logHelper; };
#endif

	private:
		TOnConnectedCallback				m_ConnectedCallbackFunc;
		TOnDisconnectedCallback				m_DisconnectedCallbackFunc;

		bool								m_bIsInitialized;
		bool								m_bIsClient;
		std::string							m_szIPAddress;
		std::uint16_t						m_wPort;
		std::uint16_t						m_wPort2;

		NetService							m_NetService;
		std::shared_ptr <CNetworkServer>	m_pNetServer;
		std::shared_ptr <CNetworkClient>	m_pNetClient;

#ifdef _DEBUG
		std::shared_ptr <CLogger>			m_logHelper;
#endif
};
