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
**    Harry Storbacka
*/

#pragma once

#include "../Image/pixel_buffer.h"
#include "../../Core/IOData/file_system.h"

namespace clan
{
	/// \addtogroup clanDisplay_Image_Providers clanDisplay Image Providers
	/// \{

	class FileSystem;
	class PNGOutputDescription_Impl;
	class DateTime;

	enum PNGColorType
	{
		png_color_type_gray,
		png_color_type_gray_alpha,
		png_color_type_palette,
		png_color_type_rgb,
		png_color_type_rgb_alpha,
		png_color_mask_palette,
		png_color_mask_color,
		png_color_mask_alpha
	};

	enum PNGInterlaceType
	{
		png_interlace_none,
		png_interlace_adam7
	};

	enum PNGFilterMethod
	{
		png_filter_type_default,
		png_intrapixel_differencing
	};

	enum PNGsRGBIntent
	{
		png_srgb_intent_saturation,
		png_srgb_intent_perceptual,
		png_srgb_intent_absolute,
		png_srgb_intent_relative
	};

	enum PNGUnknownChunkLocation
	{
		png_before_plte,
		png_before_idat,
		png_after_idat
	};

	enum PNGOffsetUnitType
	{
		png_offset_pixel,
		png_offset_micrometer
	};

	enum PNGResolutionUnitType
	{
		png_resolution_unknown,
		png_resolution_meter
	};


	class PNGOutputDescriptionPalette
	{
	public:
		PNGOutputDescriptionPalette();

	public:
		void set_colors(const std::vector<Color> &palette_colors);
		void set_key_color(const Color &color);
		void set_tranparent_color_indexes(const std::vector<int> &alpha_indexes);

	private:
		std::vector<Color> colors;
		bool use_keycolor;
	};


	/// \brief Set PNG output options. Used with PNGProvider::save().
	class PNGOutputDescription
	{
	public:
		/// \brief Create output description.
		///
		/// \param bit_depth Number of bits per pixel. valid values are 1, 2, 4, 8, 16 - depending on color_type.
		/// \param color_type Color mode of output image.
		PNGOutputDescription(int bit_depth = 8, PNGColorType color_type = png_color_type_rgb_alpha);

		void add_palette(const PNGOutputDescriptionPalette &palette);

		void add_comment(
			const std::string &comment,
			std::string &keyword, // max 79 characters.
			const std::string &lang,
			const std::string &translated_keyword);

		void add_unknown_chunk(const std::string &name, const DataBuffer &chunk_data, PNGUnknownChunkLocation location);

		void set_background_color(const Color &background_color);
		void set_chromacities(const Vec2f &white, const Vec2f &red, const Vec2f &green, const Vec2f &blue);
		void set_chromacities_XYZ(const Vec3f &red, const Vec3f &green, const Vec3f &blue);
		void set_key_color(const Color &color);
		void set_filter_method(PNGFilterMethod filter);
		void set_gamma(float gamma);
		void set_icc_profile(const std::string &profile_name, const DataBuffer &profile_data);
		void set_interlace_type(PNGInterlaceType type);
		void set_modification_time(const DateTime &datetime);
		void set_offset_from_screen_topleft(const Vec2i &offset, PNGOffsetUnitType offset_unit_type);
		void set_physical_resolution(const Vec2i &resolution, PNGResolutionUnitType resolution_type);
		void set_physical_scale(int scale_units, const Sized &pixel_size_in_scale_units);
		void set_srgb_intent(PNGsRGBIntent intent);
		void set_significant_bits(int num_bits);

	private:
		std::shared_ptr<PNGOutputDescription_Impl> impl;
	};

	/// \}
}
