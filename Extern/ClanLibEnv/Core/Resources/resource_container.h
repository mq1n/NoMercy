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

#include "resource_object.h"
#include <memory>
#include <map>

namespace clan
{
	/// \addtogroup clanCore_Resources clanCore Resources
	/// \{

	class ResourceContainer_Impl
	{
	public:
		std::map<std::string, ResourceObject> resources;
	};

	class ResourceContainer
	{
	public:
		ResourceContainer() : impl(std::make_shared<ResourceContainer_Impl>())
		{
		}

		template<typename Type>
		Resource<Type> get(const std::string &name)
		{
			auto it = impl->resources.find(name);
			if (it != impl->resources.end())
				return it->second.cast<Type>();

			Resource<Type> resource;
			impl->resources[name] = resource;
			return resource;
		}

	private:
		std::shared_ptr<ResourceContainer_Impl> impl;
	};

	/// \}
}
