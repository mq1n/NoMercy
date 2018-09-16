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


#include "connection_site.h"	// TODO: Remove
#include "../../Core/Signals/signal.h"

namespace clan
{
	/// \addtogroup clanNetwork_NetGame clanNetwork NetGame
	/// \{

	class NetGameEvent;
	class NetGameConnection;
	class NetGameServer_Impl;

	/// \brief NetGameServer
	class NetGameServer : NetGameConnectionSite
	{
	public:
		NetGameServer();
		~NetGameServer();

		/// \brief Start
		///
		/// \param port = String
		void start(const std::string &port);

		/// \brief Start
		///
		/// \param address = String
		/// \param port = String
		void start(const std::string &address, const std::string &port);

		/// \brief Process events
		void process_events();

		/// \brief Stop
		void stop();

		/// \brief Send event
		///
		/// \param game_event = Net Game Event
		void send_event(const NetGameEvent &game_event);

		Signal<void(NetGameConnection *)> &sig_client_connected();
		Signal<void(NetGameConnection *, const std::string &)> &sig_client_disconnected();
		Signal<void(NetGameConnection *, const NetGameEvent &)> &sig_event_received();

	private:

		/// \brief Listen thread main
		void listen_thread_main();

		/// \brief Add network event
		///
		/// \param e = Net Game Network Event
		void add_network_event(const NetGameNetworkEvent &e) override;

		std::shared_ptr<NetGameServer_Impl> impl;
	};

	/// \}
}
