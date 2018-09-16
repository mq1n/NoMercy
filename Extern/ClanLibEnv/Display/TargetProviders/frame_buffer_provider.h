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

#include "../Render/frame_buffer.h"
#include <vector>

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class RenderBuffer;
	class Texture2D;

	/// \brief Interface for implementing a FrameBuffer target.
	class FrameBufferProvider
	{
	public:
		virtual ~FrameBufferProvider() { return; }

		virtual Size get_size() const = 0;
		virtual FrameBufferBindTarget get_bind_target() const = 0;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param render_buffer = Render Buffer
		virtual void attach_color(int attachment_index, const RenderBuffer &render_buffer) = 0;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param level = value
		virtual void attach_color(int attachment_index, const Texture1D &texture, int level) = 0;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param array_index = value
		/// \param level = value
		virtual void attach_color(int attachment_index, const Texture1DArray &texture, int array_index, int level) = 0;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param level = value
		virtual void attach_color(int attachment_index, const Texture2D &texture, int level) = 0;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param array_index = value
		/// \param level = value
		virtual void attach_color(int attachment_index, const Texture2DArray &texture, int array_index, int level) = 0;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param depth = value
		/// \param level = value
		virtual void attach_color(int attachment_index, const Texture3D &texture, int depth, int level) = 0;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param subtype = Texture Subtype
		/// \param level = value
		/// \param zoffset = value
		virtual void attach_color(int attachment_index, const TextureCube &texture, TextureSubtype subtype, int level) = 0;

		/// \brief Detach color buffer
		///
		/// \param color_buffer = value
		/// \param texture = Texture
		/// \param level = value
		/// \param zoffset = value
		virtual void detach_color(int attachment_index) = 0;

		virtual void attach_stencil(const RenderBuffer &render_buffer) = 0;
		virtual void attach_stencil(const Texture2D &texture, int level) = 0;
		virtual void attach_stencil(const TextureCube &texture, TextureSubtype subtype, int level) = 0;
		virtual void detach_stencil() = 0;

		virtual void attach_depth(const RenderBuffer &render_buffer) = 0;
		virtual void attach_depth(const Texture2D &texture, int level) = 0;
		virtual void attach_depth(const TextureCube &texture, TextureSubtype subtype, int level) = 0;
		virtual void detach_depth() = 0;

		virtual void attach_depth_stencil(const RenderBuffer &render_buffer) = 0;
		virtual void attach_depth_stencil(const Texture2D &texture, int level) = 0;
		virtual void attach_depth_stencil(const TextureCube &texture, TextureSubtype subtype, int level) = 0;
		virtual void detach_depth_stencil() = 0;

		virtual void set_bind_target(FrameBufferBindTarget target) = 0;
	};

	/// \}
}
