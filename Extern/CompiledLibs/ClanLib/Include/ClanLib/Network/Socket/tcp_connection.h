
#pragma once

#include "network_condition_variable.h"

#include <memory>
#include <vector>
#include <mutex>

namespace clan
{
	class SocketName;
	class Event;
	class TCPSocket;

	/// \brief TCP/IP socket connection
	class TCPConnection : public NetworkEvent
	{
	public:
		static void init_sockets();

		/// \brief Create null object
		TCPConnection();

		/// \brief Blocking connect to end point
		TCPConnection(const SocketName &endpoint);

		~TCPConnection();

		/// \brief Returns true if it is a null object
		bool is_null() const { return !impl; }

		/// \brief Returns the socket name of the local end point
		SocketName get_local_name();

		/// \brief Returns the socket name of the peer end point
		SocketName get_remote_name();

		/// \brief Close connection
		void close();

		/// \brief Write data to TCP socket
		/// \return Bytes written, or -1 if buffer is full
		int write(const void *data, int size);

		/// \brief Read data from TCP socket
		/// \return Bytes read, 0 if remote closed connection, or -1 if buffer is empty
		int read(void *data, int size);

		/// \internal Constructs a TCPConnection instance based on a socket handle
		TCPConnection(const std::shared_ptr<TCPSocket> &impl);

	protected:
		SocketHandle *get_socket_handle() override;

	private:
		std::shared_ptr<TCPSocket> impl;
	};

	// To do: QOSAddSocketToFlow
}
