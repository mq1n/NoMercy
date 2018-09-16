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

#include "texture.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	/// 2D texture object class.
	class Texture2D : public Texture
	{
	public:
		/// Constructs a null instance.
		Texture2D();

		/** Constructs a texture from an implementation.
		 *  \param impl The Texture object implementation.
		 */
		Texture2D(const std::shared_ptr<Texture_Impl> &impl) : Texture(impl) { }

		/** Constructs a new Texture object.
		 *  \param context  Graphic context to construct the texture on.
		 *  \param width    Width of the new texture.
		 *  \param height   Height of the new texture.
		 *  \param format   Data format of the new texture.
		 *  \param levels   Number of mipmap levels for the new texture. Setting
		 *                  this to `0` enables all levels.
		 */
		Texture2D(GraphicContext &context, int width, int height, TextureFormat format = tf_rgba8, int levels = 1);

		/** Constructs a new Texture object.
		 *  \param context  Graphic context to construct the texture on.
		 *  \param size     Size of the new texture.
		 *  \param format   Data format of the new texture.
		 *  \param levels   Number of mipmap levels for the new texture. Setting
		 *                  this to `0` enables all levels.
		 */
		Texture2D(GraphicContext &context, const Size &size, TextureFormat texture_format = tf_rgba8, int levels = 1);

		Texture2D(GraphicContext &context, const std::string &fullname, const ImageImportDescription &import_desc = {});
		Texture2D(GraphicContext &context, const std::string &filename, const FileSystem &fs, const ImageImportDescription &import_desc = {});
		Texture2D(GraphicContext &context, IODevice &file, const std::string &image_type, const ImageImportDescription &import_desc = {});

		Texture2D(GraphicContext &context, const PixelBuffer &image, bool is_srgb = false);
		Texture2D(GraphicContext &context, const PixelBuffer &image, const Rect &src_rect, bool is_srgb = false);

		/// Retrieves the actual width of the texture in the display.
		int get_width() const;

		/// Retrieves the actual height of the texture in the display.
		int get_height() const;

		/// Retrieves the actual size of the texture.
		Size get_size() const { return Size{ get_width(), get_height() }; }

		/** Retrieves the pixel ratio of this texture.
		 *  \return The display pixel ratio set for this texture.
		 *          A zero value implies that no pixel ratio has been set
		 */
		float get_pixel_ratio() const;

		/// Returns the device independent width of this texture.
		float get_dip_width() const { return get_width() / get_pixel_ratio(); }

		/// Returns the device independent height of this texture.
		float get_dip_height() const { return get_height() / get_pixel_ratio(); }

		/// Returns the device independent size of this texture.
		Sizef get_dip_size() const { return Sizef{ get_dip_width(), get_dip_height() }; }

		/// Retrieve image data from texture.
		PixelBuffer get_pixeldata(GraphicContext &gc, int level = 0) const;

		/** Retrieve image data from this texture.
		 *  \param format Output data format.
		 *  \param level  Mipmap level of the texture to retrieve data from.
		 */
		PixelBuffer get_pixeldata(GraphicContext &gc, TextureFormat texture_format, int level = 0) const;

		/// Get the texture wrap mode for the s coordinate.
		TextureWrapMode get_wrap_mode_s() const;

		/// Get the texture wrap mode for the t coordinate.
		TextureWrapMode get_wrap_mode_t() const;

		/** Upload image to this texture.
		 *  \param context Graphic context to use for the request.
		 *  \param image   Image to upload.
		 *  \param level   Mipmap level-of-detail number.
		 */
		void set_image(
			GraphicContext &context,
			const PixelBuffer &image,
			int level = 0);

		/** Upload image to sub-texture.
		 *  \param context Graphic context to use for the request.
		 *  \param x       The horizontal point in the texture to write the new
		 *                 sub-texture image onto.
		 *  \param y       The vertical point in the texture to write the new
		 *                 sub-texture image onto.
		 *  \param image   Image to upload.
		 *  \param level   Mipmap level-of-detail number.
		 */
		void set_subimage(
			GraphicContext &context,
			int x,
			int y,
			const PixelBuffer &image,
			const Rect &src_rect,
			int level = 0);

		/** Upload image to sub-texture.
		 *  \param context Graphic context to use for the request.
		 *  \param point   Point in the texture to write the new sub-texture image.
		 *                 onto.
		 *  \param image   Image to upload.
		 *  \param level   Mipmap level-of-detail number.
		 */
		void set_subimage(
			GraphicContext &context,
			const Point &point,
			const PixelBuffer &image,
			const Rect &src_rect,
			int level = 0);

		/// Copy image data from a graphic context.
		void copy_image_from(
			GraphicContext &context,
			int level,
			TextureFormat texture_format = tf_rgba8);

		void copy_image_from(
			GraphicContext &context,
			int x,
			int y,
			int width,
			int height,
			int level = 0,
			TextureFormat texture_format = tf_rgba8);

		void copy_image_from(
			GraphicContext &context,
			const Rect &pos,
			int level = 0,
			TextureFormat texture_format = tf_rgba8);

		/// Copy sub image data from a graphic context.
		void copy_subimage_from(
			GraphicContext &context,
			int offset_x,
			int offset_y,
			int x,
			int y,
			int width,
			int height,
			int level = 0);

		void copy_subimage_from(
			GraphicContext &context,
			const Point &offset,
			const Rect &pos,
			int level = 0);

		void set_wrap_mode(
			TextureWrapMode wrap_s,
			TextureWrapMode wrap_t);

		/// Sets the display pixel ratio for this texture.
		void set_pixel_ratio(float ratio);
	};

	/// \}
}
