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
#include "../../Core/Math/rect.h"
#include "../Render/texture.h"
#include "texture_format.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class PixelBuffer;
	class PixelBufferSet_Impl;

	/// \brief Set of images that combined form a complete texture
	class PixelBufferSet
	{
	public:
		/// \brief Constructs a null instance
		PixelBufferSet();

		/// \brief Constructs an image set of the specified dimensions type and internal format
		PixelBufferSet(TextureDimensions dimensions, TextureFormat format, int width, int height = 1, int slices = 1);

		/// \brief Constructs an image set with a single image using the dimensions and internal format of the pixel buffer
		PixelBufferSet(const PixelBuffer &image);

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the texture dimensions used by the image set
		TextureDimensions get_dimensions() const;

		/// \brief Returns the internal texture format used by the image
		TextureFormat get_format() const;

		/// \brief Returns the width of the image
		int get_width() const;

		/// \brief Returns the height of the image
		int get_height() const;

		/// \brief Returns the number of depth/array/cube slices in the set
		int get_slice_count() const;

		/// \brief Returns the lowest mip level specified in the set
		int get_base_level() const;

		/// \brief Returns the highest mip level specified in the set
		int get_max_level() const;

		/// \brief Returns the pixel buffer for a specific slice and level
		PixelBuffer get_image(int slice, int level);

		/// \brief Set the pixel buffer to be used for the specified slice and level
		void set_image(int slice, int level, const PixelBuffer &image);

	private:
		std::shared_ptr<PixelBufferSet_Impl> impl;
	};

	/// \}
}
