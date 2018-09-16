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

namespace clan
{
	/// \brief ComPtr
	template <typename Type>
	class ComPtr
	{
	public:
		ComPtr() : ptr(0) { }
		explicit ComPtr(Type *ptr) : ptr(ptr) { }
		ComPtr(const ComPtr &copy) : ptr(copy.ptr) { if (ptr) ptr->AddRef(); }
		~ComPtr() { clear(); }
		ComPtr &operator =(const ComPtr &copy)
		{
			if (this == &copy)
				return *this;
			if (copy.ptr)
				copy.ptr->AddRef();
			if (ptr)
				ptr->Release();
			ptr = copy.ptr;
			return *this;
		}

		template<typename That>
		explicit ComPtr(const ComPtr<That> &that)
			: ptr(static_cast<Type*>(that.ptr))
		{
			if (ptr)
				ptr->AddRef();
		}

		bool operator ==(const ComPtr &other) const { return ptr == other.ptr; }
		bool operator !=(const ComPtr &other) const { return ptr != other.ptr; }
		bool operator <(const ComPtr &other) const { return ptr < other.ptr; }
		bool operator <=(const ComPtr &other) const { return ptr <= other.ptr; }
		bool operator >(const ComPtr &other) const { return ptr > other.ptr; }
		bool operator >=(const ComPtr &other) const { return ptr >= other.ptr; }

		// const does not exist in COM, so we drop the const qualifier on returned objects to avoid needing mutable variables elsewhere

		Type * const operator ->() const { return const_cast<Type*>(ptr); }
		Type *operator ->() { return ptr; }
		operator Type *() const { return const_cast<Type*>(ptr); }
		operator bool() const { return ptr != 0; }

		bool is_null() const { return ptr == 0; }
		void clear() { if (ptr) ptr->Release(); ptr = 0; }
		Type *get() const { return const_cast<Type*>(ptr); }
		Type **output_variable() { clear(); return &ptr; }

		Type *ptr;
	};
}
