#pragma once

class NetServer : boost::noncopyable, public std::enable_shared_from_this <NetServer>
{
	public:
		NetServer(boost::asio::io_service& service);
		virtual ~NetServer();

		bool Bind(uint16_t port);
		bool Bind(const std::string& address, uint16_t port);
		bool Bind(const boost::asio::ip::tcp::endpoint& endpoint);

	protected:
		boost::asio::io_service& m_service;
		boost::asio::ip::tcp::acceptor m_acceptor;

	private:
		void AsyncAccept();
		static void HandleAccept(std::weak_ptr<NetServer> self, const boost::system::error_code& e, std::shared_ptr<NetPeer> peer);
		virtual std::shared_ptr<NetPeer> NewPeer() = 0;
};

