#pragma once

#ifndef NETWORK_MAX_ALLOWED_SIZE
	#define NETWORK_MAX_ALLOWED_SIZE	boost::asio::detail::default_max_transfer_size_t::default_max_transfer_size
#endif

enum ENetPeerErrorTypes
{
	NET_PEER_ERR_NULL,
	NET_PEER_ERR_WRITE_SIZE_MISMATCH,
	NET_PEER_ERR_WRITE_SIZE_NULL,
	NET_PEER_ERR_WRITE_ERROR,
	NET_PEER_ERR_READ_SIZE_NULL,
	NET_PEER_ERR_READ_ERROR,
	NET_PEER_ERR_CONN_FAIL
};

class NetPeer : boost::noncopyable , public std::enable_shared_from_this <NetPeer>
{
	public:
		NetPeer(boost::asio::io_service & service);
		virtual ~NetPeer();
	
		boost::asio::ip::tcp::socket& GetSocket();
		const boost::asio::ip::tcp::socket& GetSocket() const;
		const std::string& GetIP() const;
		uint16_t GetPort() const;
		bool IsConnected() const;

		void Send(const void* data, std::size_t length, const std::string & szRequestId = "");
		void BeginRead();

		void Disconnect(const boost::system::error_code & e);
		void DelayedDisconnect(int timeOut);
	
		virtual void OnConnect() = 0;
		virtual void OnDisconnect(const boost::system::error_code & e) = 0;
		virtual std::size_t OnRead(const void* data, std::size_t length) = 0;
		virtual std::size_t OnWritePre(const void* data, std::size_t length) = 0;
		virtual void OnWritePost(bool bCompleted, const std::string & szRequestId) = 0;
		virtual void OnError(std::uint32_t ulErrorType, const boost::system::error_code & e) = 0;

	//	bool HasPendingRequest() 

	private:
		boost::asio::io_service&		m_service;
		boost::asio::ip::tcp::socket	m_socket;

		boost::asio::streambuf			m_readstream;

		std::string						m_ip;
		uint16_t						m_port;

		boost::asio::deadline_timer		disconnect_timer;
	
	private:
		static void HandleRead(std::weak_ptr <NetPeer> self, const boost::system::error_code & e, std::size_t ulSuccessedByteCount);
		static void HandleWrite(std::weak_ptr <NetPeer> self, const boost::system::error_code & e, std::size_t ulSuccessedByteCount, std::size_t ulDataSize, const std::string & szRequestId);
		static void HandleDelayedDisconnect(std::weak_ptr <NetPeer> self, const boost::system::error_code & e);
};

