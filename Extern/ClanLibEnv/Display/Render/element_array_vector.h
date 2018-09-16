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

#include "element_array_buffer.h"
#include "transfer_vector.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	/// \brief Element Array Vector
	///
	template<typename Type>
	class ElementArrayVector : public ElementArrayBuffer
	{
	public:
		/// \brief Constructs a null instance.
		ElementArrayVector()
		{
		}

		/// \brief Constructs a ElementArrayBuffer
		///
		/// \param gc = Graphic Context
		/// \param size = value
		/// \param usage = Buffer Usage
		ElementArrayVector(GraphicContext &gc, int size, BufferUsage usage = usage_static_draw)
			: ElementArrayBuffer(gc, size * sizeof(Type), usage)
		{
		}

		/// \brief Constructs a ElementArrayBuffer
		///
		/// \param gc = Graphic Context
		/// \param data = void
		/// \param size = value
		/// \param usage = Buffer Usage
		ElementArrayVector(GraphicContext &gc, Type *data, int size, BufferUsage usage = usage_static_draw)
			: ElementArrayBuffer(gc, data, size * sizeof(Type), usage)
		{
		}

		ElementArrayVector(GraphicContext &gc, const std::vector<Type> &data, BufferUsage usage = usage_static_draw)
			: ElementArrayBuffer(gc, data.empty() ? (Type*)0 : &data[0], data.size() * sizeof(Type), usage)
		{
		}

		/// \brief Uploads data to element array buffer.
		///
		/// The size specified must match the size of the buffer and is only included to help guard against buffer overruns.
		void upload_data(GraphicContext &gc, const Type *data, int size)
		{
			ElementArrayBuffer::upload_data(gc, data, size * sizeof(Type));
		}

		/// \brief Uploads data to element array buffer.
		void upload_data(GraphicContext &gc, const std::vector<Type> &data)
		{
			if (!data.empty())
				ElementArrayBuffer::upload_data(gc, &data[0], data.size() * sizeof(Type));
		}

		/// \brief Copies data from transfer buffer
		void copy_from(GraphicContext &gc, TransferVector<Type> &buffer, int dest_pos = 0, int src_pos = 0, int size = -1)
		{
			if (size != -1)
				size = size * sizeof(Type);
			ElementArrayBuffer::copy_from(gc, buffer, dest_pos * sizeof(Type), src_pos * sizeof(Type), size);
		}

		/// \brief Copies data to transfer buffer
		void copy_to(GraphicContext &gc, TransferVector<Type> &buffer, int dest_pos = 0, int src_pos = 0, int size = -1)
		{
			if (size != -1)
				size = size * sizeof(Type);
			ElementArrayBuffer::copy_to(gc, buffer, dest_pos * sizeof(Type), src_pos * sizeof(Type), size);
		}
	};

	/// \}
}
