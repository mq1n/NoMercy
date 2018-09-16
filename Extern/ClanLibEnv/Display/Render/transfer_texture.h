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

#include <memory>
#include "graphic_context.h"
#include "../Image/image_import_description.h"
#include "../Image/texture_format.h"
#include "../Image/pixel_buffer.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class Color;
	class Point;
	class PixelBuffer;
	class PixelFormat;
	class DataBuffer;
	class PixelBufferProvider;
	class TransferTexture_Impl;
	class SharedGCData_Impl;

	/// \brief Texture Transfer class.
	class TransferTexture : public PixelBuffer
	{
	public:
		/// \brief Constructs a null instance.
		TransferTexture();

		/// \brief Constructs a GPU Transfer Buffer
		///
		/// \param provider = Font Provider
		TransferTexture(PixelBufferProvider *provider);

		/// \brief Constructs a GPU Transfer Buffer
		///
		/// \param gc = Graphic Context
		/// \param width = value
		/// \param height = value
		/// \param direction = prefered direction that you will use tranferring the pixel buffer data
		/// \param sized_format = Texture Format
		/// \param data = data to copy from (May be NULL)
		/// \param usage = Buffer Usage
		TransferTexture(GraphicContext &gc, int width, int height, PixelBufferDirection direction = data_to_gpu, TextureFormat texture_format = tf_rgba8, const void *data = nullptr, BufferUsage usage = usage_stream_draw);

		/// \brief Constructs a GPU Transfer Buffer from an existing pixelbuffer
		///
		/// \param gc = Graphic Context
		/// \param pbuff = The pixelbuffer to copy from
		/// \param direction = prefered direction that you will use tranferring the pixel buffer data
		/// \param usage = Buffer Usage
		TransferTexture(GraphicContext &gc, const PixelBuffer &pbuff, PixelBufferDirection direction = data_to_gpu, BufferUsage usage = usage_stream_draw);

		~TransferTexture();
	};

	/// \}
}
