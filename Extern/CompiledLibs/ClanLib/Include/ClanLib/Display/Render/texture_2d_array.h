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

	class Texture2D;

	/// 2D texture array object class.
	class Texture2DArray : public Texture
	{
	public:
		/// Constructs a null instance.
		Texture2DArray();

		/** Constructs a texture from an implementation.
		 *  \param impl The Texture object implementation.
		 */
		Texture2DArray(const std::shared_ptr<Texture_Impl> &impl) : Texture(impl) { }

		/** Constructs a new Texture object.
		 *  \param context    Graphic context to construct the texture on.
		 *  \param width      Width of the new texture.
		 *  \param height     Height of the new texture.
		 *  \param array_size Number of textures to allocate in this array.
		 *  \param format     Data format of the new texture.
		 *  \param levels     Number of mipmap levels for the new texture. Setting
		 *                    this to `0` enables all levels.
		 */
		Texture2DArray(GraphicContext &context, int width, int height, int array_size, TextureFormat texture_format = tf_rgba8, int levels = 1);

		/** Constructs a new Texture object.
		 *  \param context    Graphic context to construct the texture on.
		 *  \param size       Size of the new texture.
		 *  \param array_size Number of textures to allocate in this array.
		 *  \param format     Data format of the new texture.
		 *  \param levels     Number of mipmap levels for the new texture. Setting
		 *                    this to `0` enables all levels.
		 */
		Texture2DArray(GraphicContext &context, const Size &size, int array_size, TextureFormat texture_format = tf_rgba8, int levels = 1);

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

		/// Returns the number of textures in the array.
		int get_array_size() const;

		/// Get the texture wrap mode for the s coordinate.
		TextureWrapMode get_wrap_mode_s() const;

		/// Get the texture wrap mode for the t coordinate.
		TextureWrapMode get_wrap_mode_t() const;

		/** Upload image to this texture array.
		 *  \param context     Graphic context to use for the request.
		 *  \param array_index Index in the array to upload the image into.
		 *  \param image       Image to upload.
		 *  \param level       Mipmap level-of-detail number.
		 */
		void set_image(
			GraphicContext &context,
			int array_index,
			const PixelBuffer &image,
			int level = 0);

		/** Upload image to sub-texture.
		 *  \param context     Graphic context to use for the request.
		 *  \param array_index Index in the array to upload the image into.
		 *  \param x           The horizontal point in the selected texture to
		 *                     write the new sub-texture image onto.
		 *  \param y           The vertical point in the selected texture to write
		 *                     the sub-texture image onto.
		 *  \param image       Image to upload.
		 *  \param level       Mipmap level-of-detail number.
		 */
		void set_subimage(
			GraphicContext &context,
			int array_index,
			int x,
			int y,
			const PixelBuffer &image,
			const Rect &src_rect,
			int level = 0);

		/** Upload image to sub-texture.
		 *  \param context     Graphic context to use for the request.
		 *  \param array_index Index in the array to upload the image into.
		 *  \param point       Point in the selected texture to write the new
		 *                     sub-texture image onto.
		 *  \param image       Image to upload.
		 *  \param level       Mipmap level-of-detail number.
		 */

		void set_subimage(
			GraphicContext &context,
			int array_index,
			const Point &point,
			const PixelBuffer &image,
			const Rect &src_rect,
			int level = 0);

		void set_wrap_mode(
			TextureWrapMode wrap_s,
			TextureWrapMode wrap_t);

		/// Creates a 2D texture view
		Texture2D create_2d_view(int array_index, TextureFormat texture_format, int min_level, int num_levels);

		/// Sets the display pixel ratio for this texture.
		void set_pixel_ratio(float ratio);
	};

	/// \}
}
