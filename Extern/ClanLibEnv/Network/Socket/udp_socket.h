
#pragma once

#include "network_condition_variable.h"
#include <memory>
#include <vector>

namespace clan
{
	class SocketName;
	class UDPSocketImpl;

	/// \brief UDP/IP socket class
	class UDPSocket : public NetworkEvent
	{
	public:
		/// \brief Create socket object
		UDPSocket();

		~UDPSocket();

		/// \brief Close connection
		void close();

		/// \brief Bind socket to end point
		void bind(const SocketName &endpoint);

		/// \brief Send UDP packet to end point
		void send(const void *data, int size, const SocketName &endpoint);

		/// \brief Read receved UDP packet
		/// \return Bytes read or 0 if no packet was available
		int read(void *data, int size, SocketName &endpoint);

	protected:
		SocketHandle *get_socket_handle() override;

	private:
		std::shared_ptr<UDPSocketImpl> impl;
	};
}
