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

	/// \brief 1D texture array object class.
	class Texture1DArray : public Texture
	{
	public:
		/// \brief Constructs a null instance.
		Texture1DArray();

		/// \brief Constructs a texture from an implementation
		///
		/// \param impl = The implementation
		Texture1DArray(const std::shared_ptr<Texture_Impl> &impl) : Texture(impl) { }

		/// \brief Constructs a Texture
		///
		/// \param context = Graphic Context
		/// \param size = value
		/// \param height = value
		/// \param internal_format = Texture Format
		/// \param levels = Mipmap levels for the texture. 0 = all levels
		Texture1DArray(GraphicContext &context, int size, int array_size, TextureFormat texture_format = tf_rgba8, int levels = 1);

		/// \brief Get the texture width.
		int get_size() const;

		/// \brief Get the texture array size
		int get_array_size() const;

		/// \brief Get the texture wrap mode for the s coordinate.
		TextureWrapMode get_wrap_mode_s() const;

		/// \brief Upload image to texture.
		///
		/// \param context Graphic context to use for the request
		/// \param array_index Index in the array
		/// \param image Image to upload.
		/// \param level Mipmap level-of-detail number.
		void set_image(GraphicContext &context, int array_index, PixelBuffer &image, int level = 0);

		/// \brief Upload image to sub texture.
		///
		/// \param context Graphic context to use for the request
		/// \param array_index Index in the array
		/// \param image Image to upload.
		/// \param level Mipmap level-of-detail number.
		void set_subimage(
			GraphicContext &context,
			int array_index,
			int x,
			const PixelBuffer &image,
			const int src_x,
			const int src_width,
			int level = 0);

		void set_wrap_mode(TextureWrapMode wrap_s);
	};

	/// \}
}
