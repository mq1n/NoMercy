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
#include <map>

namespace clan
{
	/// \addtogroup clanCore_Resources clanCore Resources
	/// \{

	class Resource_BaseImpl
	{
	public:
		virtual ~Resource_BaseImpl() { }
	};

	template<typename Type>
	class Resource_Impl : public Resource_BaseImpl
	{
	public:
		Resource_Impl() : value(), generation(0) { }
		Resource_Impl(const Type &initial_value) : value(initial_value), generation(0) { }
		Type value;
		int generation;
	};

	/// \brief Resource proxy of a specific type
	template<typename Type>
	class Resource
	{
	public:
		Resource()
			: object(new Resource_Impl<Type>()), generation(-1)
		{
		}

		Resource(std::shared_ptr<Resource_Impl<Type> > object)
			: object(object), generation(-1)
		{
		}

		Resource(const Type &initial_value)
			: object(new Resource_Impl<Type>(initial_value)), generation(-1)
		{
		}

		Type *operator->()
		{
			return &object->value;
		}

		const Type *operator->() const
		{
			return &object->value;
		}

		bool updated()
		{
			bool updated = (generation != object->generation);
			generation = object->generation;
			return updated;
		}

		void set(const Type &value)
		{
			object->value = value;
			generation = ++object->generation;
		}

		Type &get()
		{
			return object->value;
		}

		const Type &get() const
		{
			return object->value;
		}

		operator Type&() { return object->value; }
		operator const Type&() const { return object->value; }

		const std::shared_ptr<Resource_Impl<Type> > &handle() const { return object; }

		bool operator<(const Resource &other) const { return object < other.object; }
		bool operator<=(const Resource &other) const { return object <= other.object; }
		bool operator>(const Resource &other) const { return object > other.object; }
		bool operator>=(const Resource &other) const { return object >= other.object; }
		bool operator==(const Resource &other) const { return object == other.object; }
		bool operator!=(const Resource &other) const { return object != other.object; }

	private:
		std::shared_ptr<Resource_Impl<Type> > object;
		int generation;
	};

	/// \}
}
