#pragma once

class NetClient : public NetPeer
{
	public:
		NetClient(boost::asio::io_service& service, uint32_t autoReconnectCycle = 0);
		virtual ~NetClient();
	
		bool Connect(const std::string& host, uint16_t port);
		bool Connect(const std::string& host, const std::string& service);
	
		bool Reconnect();
	
		void SetAutoReconnectCycle(uint32_t autoReconnectCycle);
		uint32_t GetAutoReconnectCycle() const;
	
	protected:
		boost::asio::ip::tcp::resolver::iterator m_endpointStart;
		uint32_t m_autoReconnectCycle;
		boost::asio::deadline_timer m_reconnectTimer;
		bool m_isReconnectOnce;
	
	private:
		void TryReconnect(const boost::system::error_code& e);
		bool Connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
		static void HandleConnect(std::weak_ptr<NetClient> self, const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
};

