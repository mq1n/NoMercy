#include "NM_Index.h"
#include "NM_Main.h"
#include "NetLib.h"

#include <boost/bind.hpp>

NetClient::NetClient(boost::asio::io_service & service, uint32_t autoReconnectCycle) :
	NetPeer(service), m_autoReconnectCycle(autoReconnectCycle), m_reconnectTimer(service), m_isReconnectOnce(false) 
{
}
NetClient::~NetClient() 
{
}

bool NetClient::Connect(const std::string& host, uint16_t port)
{
	char szPort[64] = { 0 };
	return Connect(host, std::to_string(port));
}
bool NetClient::Connect(const std::string& host, const std::string& service)
{
	boost::asio::ip::tcp::resolver resolver(GetSocket().get_io_service());
	boost::asio::ip::tcp::resolver::query q(host, service);
	m_endpointStart = resolver.resolve(q);
	return Connect(m_endpointStart);
}
bool NetClient::Reconnect()
{
	if (!m_isReconnectOnce)
	{
		m_isReconnectOnce = true;

		m_reconnectTimer.expires_from_now(boost::posix_time::milliseconds(m_autoReconnectCycle));
		m_reconnectTimer.async_wait(boost::bind(&NetClient::TryReconnect, std::static_pointer_cast<NetClient>(shared_from_this()), boost::asio::placeholders::error));

		return true;
	}
	return false;
}

void NetClient::SetAutoReconnectCycle(uint32_t autoReconnectCycle) 
{ 
	m_autoReconnectCycle = autoReconnectCycle; 
}
uint32_t NetClient::GetAutoReconnectCycle() const
{
	return m_autoReconnectCycle; 
}

void NetClient::TryReconnect(const boost::system::error_code& e)
{
	if (!e)
	{
		Connect(m_endpointStart);
	}
}
bool NetClient::Connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	boost::asio::async_connect(GetSocket(), m_endpointStart, boost::bind(&NetClient::HandleConnect, std::static_pointer_cast<NetClient>(shared_from_this()), boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	return true;
}
void NetClient::HandleConnect(std::weak_ptr<NetClient> self, const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	std::shared_ptr<NetClient> _this(self.lock());
	if (_this)
	{
		if (!e)
		{
			_this->m_isReconnectOnce = false;
			_this->OnConnect();
		}

		else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
		{
			// Before starting new connection close used socket in the previous connection attempt
			_this->GetSocket().close();

			_this->Connect(endpoint_iterator);
		}

		else
		{
			_this->OnError(NET_PEER_ERR_CONN_FAIL, e);
		}

		if (_this->m_isReconnectOnce)
		{
			_this->m_isReconnectOnce = false;
			_this->Reconnect();
		}
	}
}

