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

#include <string>
#include <memory>
#include <functional>

namespace clan
{
	/// \addtogroup clanXML_Resources clanXML Resources
	/// \{

	class ResourceManager;
	class XMLResourceDocument;

	/// \brief Resource manager loading from XMLResourceDocument
	class XMLResourceManager
	{
	public:
		/// \brief Creates a resource manager
		static ResourceManager create(const XMLResourceDocument &doc);

		/// \brief Adds a cache factory callback
		///
		/// Each factory callback is invoked every time a resource manager is created.
		static void add_cache_factory(std::function<void(ResourceManager &, const XMLResourceDocument &)> factory_callback);

		/// \brief Gets the XML resource document used by a resource manager.
		static XMLResourceDocument &get_doc(const ResourceManager &manager);
	};

	/// \}
}
