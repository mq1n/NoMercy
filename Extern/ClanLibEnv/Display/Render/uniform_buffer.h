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
#include "../Image/buffer_usage.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class GraphicContext;
	class TransferBuffer;
	class UniformBufferProvider;
	class UniformBuffer_Impl;
	class ProgramObject;

	/// \brief Uniform Buffer
	class UniformBuffer
	{
	public:
		/// \brief Constructs a null instance.
		UniformBuffer();

		/// \brief Constructs a ProgramUniformBlock
		///
		/// \param gc = Graphic Context
		/// \param size = size - use ProgramObject::get_uniform_buffer_size() to calcuate this
		/// \param usage = Buffer Usage
		UniformBuffer(GraphicContext &gc, int block_size, BufferUsage usage = usage_dynamic_draw);

		/// \brief Constructs a VertexArrayBuffer
		///
		/// \param gc = Graphic Context
		/// \param data = void
		/// \param size = value
		/// \param usage = Buffer Usage
		UniformBuffer(GraphicContext &gc, const void *data, int size, BufferUsage usage = usage_dynamic_draw);

		/// \brief Constructs a ProgramUniformBlock - convenience function
		///
		/// \param gc = Graphic Context
		/// \param num_blocks = Number of blocks to allocate
		/// \param usage = Buffer Usage
		UniformBuffer(GraphicContext &gc, ProgramObject &program, const std::string &name, int num_blocks = 1, BufferUsage usage = usage_dynamic_draw);

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Get Provider
		///
		/// \return provider
		UniformBufferProvider *get_provider() const;

		/// \brief Handle comparison operator.
		bool operator==(const UniformBuffer &other) const;

		/// \brief Uploads data to uniforms buffer.
		///
		/// The size specified must match the size of the buffer and is only included to help guard against buffer overruns.
		void upload_data(GraphicContext &gc, const void *data, int size);

		/// \brief Copies data from transfer buffer
		void copy_from(GraphicContext &gc, TransferBuffer &buffer, int dest_pos = 0, int src_pos = 0, int size = -1);

		/// \brief Copies data to transfer buffer
		void copy_to(GraphicContext &gc, TransferBuffer &buffer, int dest_pos = 0, int src_pos = 0, int size = -1);

	private:
		std::shared_ptr<UniformBuffer_Impl> impl;
	};

	/// \}
}
