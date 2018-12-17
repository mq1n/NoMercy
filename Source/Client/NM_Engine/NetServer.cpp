#include "NM_Index.h"
#include "NM_Main.h"
#include "NetLib.h"

#include <boost/bind.hpp>

NetServer::NetServer(boost::asio::io_service& service) : m_service(service), m_acceptor(service)
{
}
NetServer::~NetServer()
{
}

bool NetServer::Bind(const boost::asio::ip::tcp::endpoint& endpoint)
{
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::linger(true, 10));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();

	AsyncAccept();

	return true;
}
bool NetServer::Bind(uint16_t port)
{
	return Bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
}
bool NetServer::Bind(const std::string& address, uint16_t port)
{
	return Bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(address), port));
}

void NetServer::AsyncAccept()
{
	std::shared_ptr <NetPeer> peer = NewPeer();
	m_acceptor.async_accept(peer->GetSocket(), boost::bind(&NetServer::HandleAccept, shared_from_this(), boost::asio::placeholders::error, peer));
}
void NetServer::HandleAccept(std::weak_ptr<NetServer> self, const boost::system::error_code& e, std::shared_ptr<NetPeer> peer)
{
	std::shared_ptr <NetServer> _this(self.lock());
	if (_this)
	{
		if (!e)
		{
			if (peer && peer.get())
			{
				peer->OnConnect();
			}
		}

		_this->AsyncAccept();
	}
}
