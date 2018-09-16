
#pragma once

#include "network_condition_variable.h"
#include <memory>
#include <vector>

namespace clan
{
	class SocketName;
	class TCPConnection;
	class TCPSocket;

	/// \brief Listens for incoming TCP/IP socket connections
	class TCPListen : public NetworkEvent
	{
	public:
		/// \brief Create null object
		TCPListen();

		/// \brief Create a listening socket for the specified end point
		TCPListen(const SocketName &endpoint, int backlog = 5, bool reuse_address = true);

		~TCPListen();

		/// \brief Returns true if it is a null object
		bool is_null() const { return !impl; }

		/// \brief Stops listening for incoming messages and closes the socket
		void close();

		/// \brief Accept an incoming connection
		/// \param end_point Socket name of the accepted peer
		/// \return A TcpConnection for the accepted connection, or a null object if no incoming connections are available
		TCPConnection accept(SocketName &end_point);

	protected:
		SocketHandle *get_socket_handle() override;

	private:
		std::shared_ptr<TCPSocket> impl;
	};
}
