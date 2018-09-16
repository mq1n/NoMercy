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
#include "../../Core/Math/vec4.h"
#include "texture_format.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class PixelConverter_Impl;

	/// \brief Low level pixel format converter class.
	class PixelConverter
	{
	public:
		/// \brief Constructs a pixel format converter
		PixelConverter();
		~PixelConverter();

		/// \brief Returns the premultiply alpha setting
		bool get_premultiply_alpha() const;

		/// \brief Returns the flip vertical setting
		bool flip_vertical() const;

		/// \brief Returns the gamma setting
		float get_gamma() const;

		/// \brief Returns the input channel used for each output channel
		Vec4i get_swizzle() const;

		/// \brief Returns the JPEG JFIF YCrCb input setting
		bool get_input_is_ycrcb() const;

		/// \brief Returns the JPEG JFIF YCrCb output setting
		bool get_output_is_ycrcb() const;

		/// \brief Set the premultiply alpha setting
		///
		/// This defaults to off.
		void set_premultiply_alpha(bool enable);

		/// \brief Set the flip vertical setting
		///
		/// This defaults to off.
		void set_flip_vertical(bool enable);

		/// \brief Set the gamma applied when converting
		///
		/// This defaults to 1.0 (off).
		void set_gamma(float gamma);

		/// \brief Set the input channel used for each output channel
		///
		/// Values 0-3 accepted. 0 = red, 1 = green, 2 = blue, 3 = alpha.
		void set_swizzle(int red_source, int green_source, int blue_source, int alpha_source);
		void set_swizzle(const Vec4i &swizzle);

		/// \brief Converts from JPEG JFIF YCrCb
		void set_input_is_ycrcb(bool enable);

		/// \brief Converts to JPEG JFIF YCrCb
		void set_output_is_ycrcb(bool enable);

		/// \brief Convert some pixel data
		void convert(void *output, int output_pitch, TextureFormat output_format, const void *input, int input_pitch, TextureFormat input_format, int width, int height);

	private:
		std::shared_ptr<PixelConverter_Impl> impl;
	};

	/// \}
}
