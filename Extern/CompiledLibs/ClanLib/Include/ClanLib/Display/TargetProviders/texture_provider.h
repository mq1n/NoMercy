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
**    Harry Storbacka
*/

#pragma once

#include "../Render/texture.h"
#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	enum TextureWrapMode;
	enum TextureFilter;
	enum TextureCompareMode;
	enum CompareFunction;
	class PixelBuffer;
	class PixelFormat;
	class GraphicContextProvider;

	/// \brief Interface for implementing a Texture target.
	class TextureProvider
	{
	public:
		virtual ~TextureProvider() { return; }

		/// \brief Create texture.
		virtual void create(int width, int height, int depth, int array_size, TextureFormat texture_format, int levels) = 0;

		/// \brief Retrieve image data from texture.
		virtual PixelBuffer get_pixeldata(GraphicContext &gc, TextureFormat texture_format, int level) const = 0;

		/// \brief Generate the mipmap
		virtual void generate_mipmap() = 0;

		/// \brief Copy image data to texture.
		virtual void copy_from(GraphicContext &gc, int x, int y, int slice, int level, const PixelBuffer &src, const Rect &src_rect) = 0;

		/// \brief Copy image data from a graphic context.
		virtual void copy_image_from(
			int x,
			int y,
			int width,
			int height,
			int level,
			TextureFormat texture_format,
			GraphicContextProvider *gc) = 0;

		/// \brief Copy sub image data from a graphic context.
		virtual void copy_subimage_from(
			int offset_x,
			int offset_y,
			int x,
			int y,
			int width,
			int height,
			int level,
			GraphicContextProvider *gc) = 0;

		/// \brief Set the minimum level of detail texture parameter.
		virtual void set_min_lod(double min_lod) = 0;

		/// \brief Set the maximum level of detail texture parameter.
		virtual void set_max_lod(double max_lod) = 0;

		/// \brief Sets the level of detail bias constant.
		virtual void set_lod_bias(double lod_bias) = 0;

		/// \brief Sets the texture base level texture parameter.
		virtual void set_base_level(int base_level) = 0;

		/// \brief Sets the texture max level texture parameter.
		virtual void set_max_level(int max_level) = 0;

		/// \brief Set the texture wrapping mode.
		virtual void set_wrap_mode(
			TextureWrapMode wrap_s,
			TextureWrapMode wrap_t,
			TextureWrapMode wrap_r) = 0;

		virtual void set_wrap_mode(
			TextureWrapMode wrap_s,
			TextureWrapMode wrap_t) = 0;

		virtual void set_wrap_mode(
			TextureWrapMode wrap_s) = 0;

		/// \brief Set the minification filter.
		virtual void set_min_filter(TextureFilter filter) = 0;

		/// \brief Set the magnification filter.
		virtual void set_mag_filter(TextureFilter filter) = 0;

		/// \brief Set the maximum degree of anisotropy.
		virtual void set_max_anisotropy(float v) = 0;

		/// \brief Sets the texture compare mode and compare function texture parameters.
		virtual void set_texture_compare(TextureCompareMode mode, CompareFunction func) = 0;

		/// \breif Creates a texture view for this texture
		virtual TextureProvider *create_view(TextureDimensions texture_dimensions, TextureFormat texture_format, int min_level, int num_levels, int min_layer, int num_layers) = 0;
	};

	/// \}
}
