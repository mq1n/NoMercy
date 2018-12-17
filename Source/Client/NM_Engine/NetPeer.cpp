#include "NM_Index.h"
#include "NM_Main.h"
#include "NetLib.h"
#include "Defines.h"
#include "ExceptionHandlers.h"

#include <boost/bind.hpp>

NetPeer::NetPeer(boost::asio::io_service& service) : 
	m_ip(""), m_port(0), m_service(service), m_socket(service), disconnect_timer(service)
{
}
NetPeer::~NetPeer()
{
}

const std::string & NetPeer::GetIP() const
{
	try
	{
		std::const_pointer_cast<NetPeer>(shared_from_this())->m_ip = m_socket.remote_endpoint().address().to_string();
	}
	catch (...) {}
	return m_ip;
}
uint16_t NetPeer::GetPort() const
{
	try
	{
		std::const_pointer_cast<NetPeer>(shared_from_this())->m_port = m_socket.remote_endpoint().port();
	}
	catch(...) { }
	return m_port;
}
bool NetPeer::IsConnected() const 
{ 
	return m_socket.is_open(); 
}
boost::asio::ip::tcp::socket& NetPeer::GetSocket() 
{ 
	return m_socket;
}
const boost::asio::ip::tcp::socket& NetPeer::GetSocket() const 
{ 
	return m_socket; 
}


void NetPeer::Disconnect(const boost::system::error_code& e)
{
	try
	{
		disconnect_timer.cancel();

		if (m_socket.is_open())
		{			
			OnDisconnect(e);

			m_socket.shutdown(boost::asio::socket_base::shutdown_both);
			m_socket.close();
		}
	}
	catch (const boost::system::system_error& e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_ERR, "Exception occured: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif
//		CExceptionHandlers::OnExceptionThrowed(nullptr);
	}
	catch (...)
	{
		NETWORK_LOG(LL_ERR, "Unhandled xception occured!");
//		CExceptionHandlers::OnExceptionThrowed(nullptr);
	}
}

void NetPeer::Send(const void* data, std::size_t length, const std::string & szRequestId)
{
	if (length)
	{
		if (length > NETWORK_MAX_ALLOWED_SIZE)
		{
			NETWORK_LOG(LL_CRI, "Currently maximum supported packet size: %u for a block!", NETWORK_MAX_ALLOWED_SIZE);
			return;
		}

		NETWORK_LOG(LL_SYS, "Send called! Data: %p Size: %u", data, length);

		// Send pre write notification and get final packet size
		auto ulPacketSize = OnWritePre(data, length);

		// Async send
		auto pBuffer = std::make_shared<boost::asio::mutable_buffer>(const_cast<void*>(data), ulPacketSize);
		boost::asio::async_write(m_socket, *pBuffer, boost::bind(&NetPeer::HandleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, ulPacketSize, szRequestId));
	}
}

void NetPeer::HandleWrite(std::weak_ptr <NetPeer> self, const boost::system::error_code & e, std::size_t ulSuccessedByteCount, std::size_t ulDataSize, const std::string & szRequestId)
{
	std::shared_ptr <NetPeer> _this(self.lock());
	if (_this)
	{
		NETWORK_LOG(LL_SYS, "Socket write returns: %s Data size: %u Writed size: %u", e.message().c_str(), ulDataSize, ulSuccessedByteCount);

		if (!e)
		{
			if (ulSuccessedByteCount > 0)
			{
				if (ulSuccessedByteCount == ulDataSize)
				{
					_this->OnWritePost(true, szRequestId);
				}
				else
				{
					NETWORK_LOG(LL_CRI, "Unknown data size! Data: %u Writed: %u", ulDataSize, ulSuccessedByteCount);
					_this->OnWritePost(false, szRequestId);
					_this->OnError(NET_PEER_ERR_WRITE_SIZE_MISMATCH, e);
				}
			}
			else
			{
				NETWORK_LOG(LL_CRI, "Writed byte count NULL! Error: %u(%s)", e.value(), e.message().c_str());
				_this->OnError(NET_PEER_ERR_WRITE_SIZE_NULL, e);
				_this->Disconnect(e);
			}
		}
		else
		{
			if (e != boost::asio::error::operation_aborted)
			{
				NETWORK_LOG(LL_CRI, "Write operation fail! Error: %u(%s)", e.value(), e.message().c_str());
				_this->OnError(NET_PEER_ERR_WRITE_ERROR, e);
				_this->Disconnect(e);
			}
		}
	}
}


void NetPeer::BeginRead()
{
	boost::asio::streambuf::mutable_buffers_type mutableBuffer = m_readstream.prepare(NETWORK_MAX_ALLOWED_SIZE);
	boost::asio::async_read(m_socket, mutableBuffer, boost::asio::transfer_at_least(1), boost::bind(&NetPeer::HandleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void NetPeer::HandleRead(std::weak_ptr <NetPeer> self, const boost::system::error_code & e, std::size_t ulSuccessedByteCount)
{
	std::shared_ptr <NetPeer> _this(self.lock());
	if (_this)
	{
		NETWORK_LOG(LL_SYS, "Socket read returns: %s Read size: %u", e.message().c_str(), ulSuccessedByteCount);

		if (!e)
		{
			if (ulSuccessedByteCount > 0)
			{
				_this->m_readstream.commit(ulSuccessedByteCount);

				auto bufs = _this->m_readstream.data();
				auto data = std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_end(bufs));

				auto size = _this->OnRead(data.c_str(), data.size());

				NETWORK_LOG(LL_SYS, "Consumed %u bytes", size);

				_this->m_readstream.consume(size);

				NETWORK_LOG(LL_SYS, "Still on buffer %u bytes", _this->m_readstream.size());
			}
			else
			{
				NETWORK_LOG(LL_CRI, "Read byte count NULL! Error: %u(%s)", e.value(), e.message().c_str());
				_this->OnError(NET_PEER_ERR_READ_SIZE_NULL, e);
				_this->Disconnect(e);
			}

			_this->BeginRead();
		}
		else
		{
			if (e != boost::asio::error::operation_aborted)
			{
				NETWORK_LOG(LL_CRI, "Read operation fail! Error: %u(%s)", e.value(), e.message().c_str());
				_this->OnError(NET_PEER_ERR_READ_ERROR, e);
				_this->Disconnect(e);
			}
		}
	}
}

void NetPeer::DelayedDisconnect(int timeOut)
{
	disconnect_timer.expires_from_now(boost::posix_time::milliseconds(timeOut));
	disconnect_timer.async_wait(boost::bind(&NetPeer::HandleDelayedDisconnect, shared_from_this(), boost::asio::placeholders::error));
}
void NetPeer::HandleDelayedDisconnect(std::weak_ptr <NetPeer> self, const boost::system::error_code & e)
{
	std::shared_ptr <NetPeer> _this(self.lock());
	if (_this)
	{
		if (!e)
		{
			NETWORK_LOG(LL_SYS, "Delayed disconnect timeout! Connection dropped");
			_this->Disconnect(e);
		}
	}
}
