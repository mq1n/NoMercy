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

#include <memory>
#include "../Signals/signal.h"

namespace clan
{
	/// \addtogroup clanCore_System clanCore System
	/// \{

	class Service_Impl;

	/// \brief Service/daemon class.
	class Service
	{
	public:
		/// \brief Constructs a service object.
		Service(const std::string &service_name);
		~Service();

		/// \brief Returns the service name.
		const std::string &get_service_name() const;

		/// \brief Process command line and run service.
		int main(int argc, char **argv);

	protected:
		/// \brief Called when the service is asked to start.
		virtual void service_start(std::vector<std::string> &args) = 0;

		/// \brief Called when the service is asked to stop.
		virtual void service_stop() = 0;

		/// \brief Called when the service is asked to reload its configuration.
		virtual void service_reload() = 0;

	private:
		inline Service(const Service &/*copy*/) { }
		inline Service &operator =(const Service &) { return *this; }

		std::shared_ptr<Service_Impl> impl;

		friend class Service_Impl;
	};

	/// \}
}
