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
	class TransferBufferProvider;
	class TransferBuffer_Impl;

	/// \brief Transfer Buffer
	class TransferBuffer
	{
	public:
		/// \brief Constructs a null instance.
		TransferBuffer();

		/// \brief Constructs a transfer buffer
		///
		/// \param gc = Graphic Context
		/// \param size = value
		/// \param usage = Buffer Usage
		TransferBuffer(GraphicContext &gc, int size, BufferUsage usage = usage_dynamic_copy);

		/// \brief Constructs a transfer buffer
		///
		/// \param gc = Graphic Context
		/// \param data = void
		/// \param size = value
		/// \param usage = Buffer Usage
		TransferBuffer(GraphicContext &gc, const void *data, int size, BufferUsage usage = usage_dynamic_copy);

		virtual ~TransferBuffer();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Retrieves a pointer to the mapped buffer.
		void *get_data();

		/// \brief Get Provider
		///
		/// \return provider
		TransferBufferProvider *get_provider() const;

		/// \brief Handle comparison operator.
		bool operator==(const TransferBuffer &other) const;

		/// \brief Maps buffer into system memory.
		void lock(GraphicContext &gc, BufferAccess access);

		/// \brief Unmaps buffer.
		void unlock();

		/// \brief Uploads data to transfer buffer.
		void upload_data(GraphicContext &gc, int offset, const void *data, int size);

	private:
		std::shared_ptr<TransferBuffer_Impl> impl;
	};

	/// \}
}
