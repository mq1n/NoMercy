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

#include "transfer_buffer.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	/// \brief Transfer Vector
	///
	template<typename Type>
	class TransferVector : public TransferBuffer
	{
	public:
		/// \brief Constructs a null instance.
		TransferVector()
		{
		}

		/// \brief Constructs a TransferVector
		///
		/// \param gc = Graphic Context
		/// \param size = value
		/// \param usage = Buffer Usage
		TransferVector(GraphicContext &gc, int size, BufferUsage usage = usage_dynamic_copy)
			: TransferBuffer(gc, size * sizeof(Type), usage)
		{
		}

		/// \brief Constructs a TransferVector
		///
		/// \param gc = Graphic Context
		/// \param data = void
		/// \param size = value
		/// \param usage = Buffer Usage
		TransferVector(GraphicContext &gc, Type *data, int size, BufferUsage usage = usage_dynamic_copy)
			: TransferBuffer(gc, data, size * sizeof(Type), usage)
		{
		}

		TransferVector(GraphicContext &gc, const std::vector<Type> &data, BufferUsage usage = usage_dynamic_copy)
			: TransferBuffer(gc, data.empty() ? (Type*)0 : &data[0], data.size() * sizeof(Type), usage)
		{
		}

		/// \brief Constructs a TransferVector from an existing buffer
		explicit TransferVector(const TransferBuffer &transfer_buffer)
			: TransferBuffer(transfer_buffer)
		{
		}

		/// \brief Retrieves a pointer to the mapped buffer.
		Type *get_data() { return reinterpret_cast<Type*>(TransferBuffer::get_data()); }

		Type &operator[](int index) { return get_data()[index]; }
		Type &operator[](unsigned int index) { return get_data()[index]; }

		/// \brief Uploads data to transfer buffer.
		void upload_data(GraphicContext &gc, int offset, const Type *data, int size)
		{
			TransferBuffer::upload_data(gc, offset * sizeof(Type), data, size * sizeof(Type));
		}

		/// \brief Uploads data to transfer buffer.
		void upload_data(GraphicContext &gc, int offset, const std::vector<Type> &data)
		{
			if (!data.empty())
				TransferBuffer::upload_data(gc, offset * sizeof(Type), &data[0], data.size() * sizeof(Type));
		}
	};

	/// \}
}
