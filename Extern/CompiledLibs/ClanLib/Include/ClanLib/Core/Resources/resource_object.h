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

#include "resource.h"

namespace clan
{
	/// \addtogroup clanCore_Resources clanCore Resources
	/// \{

	/// \brief Resource proxy of any type
	class ResourceObject
	{
	public:
		ResourceObject()
		{
		}

		template<typename Type>
		ResourceObject(const Resource<Type> &resource)
			: object(resource.handle())
		{
		}

		bool is_null() const { return !object; }

		template<typename Type>
		Resource<Type> cast()
		{
			if (object)
			{
				std::shared_ptr<Resource_Impl<Type> > resource = std::dynamic_pointer_cast<Resource_Impl<Type>>(object);
				if (!resource)
					throw Exception("ResourceObject type mismatch");
				return Resource<Type>(resource);
			}
			else
			{
				throw Exception("ResourceObject is null");
			}
		}

		bool operator<(const ResourceObject &other) const { return object < other.object; }
		bool operator<=(const ResourceObject &other) const { return object <= other.object; }
		bool operator>(const ResourceObject &other) const { return object > other.object; }
		bool operator>=(const ResourceObject &other) const { return object >= other.object; }
		bool operator==(const ResourceObject &other) const { return object == other.object; }
		bool operator!=(const ResourceObject &other) const { return object != other.object; }

	private:
		std::shared_ptr<Resource_BaseImpl> object;
	};

	/// \}
}
