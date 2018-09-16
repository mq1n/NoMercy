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
**    Mark Page
*/

#pragma once

#include "../Render/element_array_buffer.h"
#include "../Image/pixel_buffer.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	/// \brief Element Array Buffer provider.
	class PixelBufferProvider
	{
	public:
		virtual ~PixelBufferProvider() { }

		/// \brief Create
		///
		/// \param data = void
		/// \param new_width = New width
		/// \param new_height = New height
		/// \param direction = Direction
		/// \param new_format = New format
		/// \param usage = Buffer Usage
		virtual void create(const void *data, const Size &new_size, PixelBufferDirection direction, TextureFormat new_format, BufferUsage usage) = 0;

		/// \brief Retrieves a pointer to the mapped buffer.
		virtual void *get_data() = 0;

		/// \brief Retrieves the pitch of the mapped buffer. Use this after get_data()
		virtual int get_pitch() const = 0;

		virtual Size get_size() const = 0;

		/// \brief Returns true if this pixel buffer is a GPU based one
		virtual bool is_gpu() const = 0;

		/// \brief Returns the pixel format
		virtual TextureFormat get_format() const = 0;

		/// \brief Maps buffer into system memory.
		virtual void lock(GraphicContext &gc, BufferAccess access) = 0;

		/// \brief Unmaps element buffer.
		virtual void unlock() = 0;

		/// \brief Uploads data to buffer.
		virtual void upload_data(GraphicContext &gc, const Rect &dest_rect, const void *data) = 0;
	};

	/// \}
}
