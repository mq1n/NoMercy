/*
**  ClanLib SDK
**  Copyright (c) 1997-2016 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include <vector>
#include <string>
#include "event.h"

namespace clan
{
	/// \addtogroup clanNetwork_NetGame clanNetwork NetGame
	/// \{

	class NetGameConnectionSite;
	class NetGameConnection_Impl;
	class SocketName;
	class TCPConnection;

	/// \brief NetGameConnection
	class NetGameConnection
	{
	public:
		/// \brief Constructs a NetGameConnection
		///
		/// \param site = Net Game Connection Site
		/// \param connection = TCPConnection
		NetGameConnection(NetGameConnectionSite *site, const TCPConnection &connection);
		NetGameConnection(NetGameConnectionSite *site, const SocketName &socket_name);

		~NetGameConnection();

		/// \brief Set data
		///
		/// \param name = String Ref
		/// \param data = void
		void set_data(const std::string &name, void *data);

		/// \brief Get data
		///
		/// \param name = String Ref
		///
		/// \return void
		void *get_data(const std::string &name) const;

		/// \brief Send event
		///
		/// \param game_event = Net Game Event
		void send_event(const NetGameEvent &game_event);

		/// \brief Disconnects a client
		void disconnect();

		/// \brief Get Remote name
		///
		/// \return remote_name
		SocketName get_remote_name() const;

	private:
		/// \brief Disallow copy constructors
		NetGameConnection(NetGameConnection &other) = delete;
		NetGameConnection &operator =(const NetGameConnection &other) = delete;

		NetGameConnection_Impl *impl;
	};

	/// \}
}
