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

#include "system.h"
#include <memory>

namespace clan
{
	/// \addtogroup clanCore_System clanCore System
	/// \{

	class BlockAllocator_Impl;

	/// \brief Memory allocator that allocates in blocks.
	///
	/// <p>This allocator will never free any memory until free() is called or
	///    the allocator object is destroyed. If any objects have been new'ed using
	///    BlockAllocated they have to be delete'ed before free() is called,
	///    otherwise the destructors of the objects will not get called. Same rules
	///    apply to the new_obj and delete_obj template functions.</p>
	///    <p>The allocator allocates memory from the heap in blocks. Each time the
	///    allocated block is exhausted, the allocator will double the block size
	///    and allocate more memory.</p>
	class BlockAllocator
	{
	public:
		/// \brief Block Allocator constructor
		BlockAllocator();

		/// \brief Allocate memory (See note on this class for the allocation method)
		/**
			param: size = Size to allocate (in bytes)
			\return The memory*/
		void *allocate(int size);

		/// \brief Free the allocated memory
		/** If required, use delete_obj() to call the destructor before using this function*/
		void free();

	private:
		std::shared_ptr<BlockAllocator_Impl> impl;
	};

	/// \brief Class with operator new/delete overloads for BlockAllocator.
	///
	///    <p>To use this class, derive your class from BlockAllocated. Then, to
	///    allocate your class, use the following operator new syntax:</p>
	///    <pre>
	///      BlockAllocator allocator;
	///      MyObject *obj = new(&allocator) MyObject(..);
	///    </pre>
	///    <p>Before freeing up the memory using BlockAllocator::free() you have
	///    to delete the object first using operator delete. If you fail to do this
	///    the destructor will not get called.</p>
	class BlockAllocated
	{
	public:
		void *operator new(size_t size, BlockAllocator *allocator);
		void operator delete(void *data, size_t size);
		void operator delete(void *data, BlockAllocator *allocator);
	};

	/// \}
}
