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

#include "../Render/element_array_buffer.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	/// \brief Element Array Buffer provider.
	class ElementArrayBufferProvider
	{
	public:
		virtual ~ElementArrayBufferProvider() { }

		/// \brief Constructs a element array buffer.
		virtual void create(int size, BufferUsage usage) = 0;

		/// \brief Create
		///
		/// \param data = void
		/// \param size = value
		/// \param usage = Buffer Usage
		virtual void create(void *data, int size, BufferUsage usage) = 0;

		/// \brief Uploads data to element array buffer.
		///
		/// The size specified must match the size of the buffer and is only included to help guard against buffer overruns.
		virtual void upload_data(GraphicContext &gc, const void *data, int size) = 0;

		/// \brief Copies data from transfer buffer
		virtual void copy_from(GraphicContext &gc, TransferBuffer &buffer, int dest_pos, int src_pos, int size) = 0;

		/// \brief Copies data to transfer buffer
		virtual void copy_to(GraphicContext &gc, TransferBuffer &buffer, int dest_pos, int src_pos, int size) = 0;
	};

	/// \}
}
