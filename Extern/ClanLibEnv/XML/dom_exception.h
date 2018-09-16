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

#include "../Core/System/exception.h"

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	/// \brief DOM Exception class.
	class DomException : public Exception
	{
	public:
		/// \brief Constructs a DomException
		///
		/// \param code = short
		DomException(unsigned short code);

		DomException(
			const std::string &message,
			unsigned short code);

		enum ExceptionCodes
		{
			INDEX_SIZE_ERR = 1,
			DOMSTRING_SIZE_ERR = 2,
			HIERARCHY_REQUEST_ERR = 3,
			WRONG_DOCUMENT_ERR = 4,
			INVALID_CHARACTER_ERR = 5,
			NO_DATA_ALLOWED_ERR = 6,
			NO_MODIFICATION_ALLOWED_ERR = 7,
			NOT_FOUND_ERR = 8,
			NOT_SUPPORTED_ERR = 9,
			INUSE_ATTRIBUTE_ERR = 10,
			INVALID_STATE_ERR = 11,
			SYNTAX_ERR = 12,
			INVALID_MODIFICATION_ERR = 13,
			NAMESPACE_ERR = 14,
			INVALID_ACCESS_ERR = 15
		};

		unsigned short code;
	};

	/// \}
}
