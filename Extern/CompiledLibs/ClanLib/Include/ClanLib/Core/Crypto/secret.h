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
**    Mark Page
*/

#pragma once

#include <memory>

namespace clan
{
	/// \addtogroup clanCore_Crypto clanCore Crypto
	/// \{

	class DataBuffer;
	class Secret_Impl;

	/// \brief Key class
	///
	/// This class zero's the key from memory when the class is destroyed
	class Secret
	{
	public:
		/// \brief Constructs a key (unset)
		Secret();

		/// \brief Constructs a key to a specified length
		///
		/// The data is not initialised
		///
		/// \param new_key_length = Length of the key
		Secret(unsigned int new_key_length);

		/// \brief Get the key size
		///
		/// \return The key size
		unsigned int get_size() const;

		/// \brief Get the key data
		///
		/// \return The key
		unsigned char *get_data() const;

	private:
		std::shared_ptr<Secret_Impl> impl;
	};

	/// \}
}
