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

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class DataBuffer;
	class Base64Encoder_Impl;

	/// \brief Base64 encoder class.
	class Base64Encoder
	{
	public:
		/// \brief Constructs a base64 encoder.
		Base64Encoder();

		/// \brief Returns the data in encoded form.
		DataBuffer &get_result();

		/// \brief Resets the encoder.
		void reset();

		/// \brief Feeds the encoder with more data.
		void feed(const void *data, int size, bool append_result = true);

		/// \brief Ends the base64 encoding.
		void finalize(bool append_result = true);

		/// \brief Base64 encodes data and returns it as an 8 bit string.
		static std::string encode(const void *data, int size);

		/// \brief Encode
		///
		/// \param data = String Ref8
		///
		/// \return String8
		static std::string encode(const std::string &data);

		/// \brief Encode
		///
		/// \param data = Data Buffer
		///
		/// \return String8
		static std::string encode(const DataBuffer &data);

	private:
		std::shared_ptr<Base64Encoder_Impl> impl;
	};

	/// \}
}
