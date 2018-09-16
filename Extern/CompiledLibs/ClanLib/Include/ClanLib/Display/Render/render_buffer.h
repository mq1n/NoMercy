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

#include <memory>
#include "texture.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class RenderBuffer;
	class GraphicContext;
	class RenderBufferProvider;
	class RenderBuffer_Impl;

	/// \brief Render-buffer object class.
	class RenderBuffer
	{
	public:
		/// \brief Constructs a null instance.
		RenderBuffer();

		/// \brief Constructs a RenderBuffer
		///
		/// \param context = Graphic Context
		/// \param width = value
		/// \param height = value
		/// \param internal_format = Texture Format
		RenderBuffer(GraphicContext &context, int width, int height, TextureFormat texture_format = tf_rgba8, int multisample_samples = 0);

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the render buffer size.
		const Size &get_size() const;

		/// \brief Get Provider
		///
		/// \return provider
		RenderBufferProvider *get_provider() const;

		/// \brief Equality operator
		bool operator==(const RenderBuffer &other) const;

	private:
		std::shared_ptr<RenderBuffer_Impl> impl;
	};

	/// \}
}
