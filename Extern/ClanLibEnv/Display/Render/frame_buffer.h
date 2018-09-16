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

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class Size;
	class Rect;
	class Texture1D;
	class Texture1DArray;
	class Texture2D;
	class Texture2DArray;
	class Texture3D;
	class TextureCube;
	class RenderBuffer;
	class GraphicContext;
	class FrameBufferProvider;
	class FrameBuffer_Impl;

	/// \brief Texture Subtype
	enum TextureSubtype
	{
		subtype_cube_map_positive_x,
		subtype_cube_map_negative_x,
		subtype_cube_map_positive_y,
		subtype_cube_map_negative_y,
		subtype_cube_map_positive_z,
		subtype_cube_map_negative_z
	};

	/// \brief Framebuffer bind target.
	enum FrameBufferBindTarget
	{
		framebuffer_draw,
		framebuffer_read
	};

	/// \brief Frame-buffer object class.
	class FrameBuffer
	{
	public:
		/// \brief Constructs a null instance.
		FrameBuffer();

		/// \brief Constructs a FrameBuffer
		///
		/// By default, the bind target is set to framebuffer_draw. See set_bind_target()
		///
		/// \param context = Graphic Context
		FrameBuffer(GraphicContext &context);

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Get Provider
		///
		/// \return provider
		FrameBufferProvider *get_provider() const;

		/// \brief Get the minumum size of all the frame buffer attachments
		///
		/// \return size
		Size get_size() const;

		/// \brief Get the bind target of the framebuffer
		///
		/// \return The bind target
		FrameBufferBindTarget get_bind_target() const;

		/// \brief Equality operator
		bool operator==(const FrameBuffer &other) const;

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param render_buffer = Render Buffer
		void attach_color(int attachment_index, const RenderBuffer &render_buffer);

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param level = value
		void attach_color(int attachment_index, const Texture1D &texture, int level = 0);

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param array_index = value
		/// \param level = value
		void attach_color(int attachment_index, const Texture1DArray &texture, int array_index = -1, int level = 0);

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param level = value
		void attach_color(int attachment_index, const Texture2D &texture, int level = 0);

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param array_index = value
		/// \param level = value
		void attach_color(int attachment_index, const Texture2DArray &texture, int array_index = -1, int level = 0);

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param depth = value
		/// \param level = value
		void attach_color(int attachment_index, const Texture3D &texture, int depth, int level = 0);

		/// \brief Attach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param subtype = Texture Subtype
		/// \param level = value
		/// \param zoffset = value
		void attach_color(int attachment_index, const TextureCube &texture, TextureSubtype subtype, int level = 0);

		/// \brief Detach color buffer
		///
		/// \param attachment_index = value
		/// \param texture = Texture
		/// \param level = value
		/// \param zoffset = value
		void detach_color(int attachment_index);

		void attach_stencil(const RenderBuffer &render_buffer);
		void attach_stencil(const Texture2D &texture, int level = 0);
		void attach_stencil(const TextureCube &texture, TextureSubtype subtype, int level = 0);
		void detach_stencil();

		void attach_depth(const RenderBuffer &render_buffer);
		void attach_depth(const Texture2D &texture, int level = 0);
		void attach_depth(const TextureCube &texture, TextureSubtype subtype, int level = 0);
		void detach_depth();

		void attach_depth_stencil(const RenderBuffer &render_buffer);
		void attach_depth_stencil(const Texture2D &texture, int level = 0);
		void attach_depth_stencil(const TextureCube &texture, TextureSubtype subtype, int level = 0);
		void detach_depth_stencil();

		/// \brief Set the bind target of the framebuffer to either drawn to or read from.
		///
		/// Detach existing textures and renderbuffers before setting a new bind target
		///
		/// \param target = Target
		void set_bind_target(FrameBufferBindTarget target);

		/** Retrieves the pixel ratio of this texture.
		*  \return The display pixel ratio set for this texture.
		*          A zero value implies that no pixel ratio has been set
		*/
		float get_pixel_ratio() const;

	private:
		std::shared_ptr<FrameBuffer_Impl> impl;
	};

	/// \}
}
