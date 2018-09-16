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
**    Thomas Gottschalk Larsen
*/

#pragma once

#include "API/Core/System/exception.h"

namespace clan
{
	class XPathToken;

	class XPathException : public Exception
	{
	public:
		XPathException(const std::string &message) : Exception(message) {};
		XPathException(const std::string &message, const std::string &expression);
		XPathException(const std::string &message, const std::string &expression, const XPathToken &cur_token);
		~XPathException() throw() {}

		std::string get_message() const;

	public:
		std::string expression;
		std::string::size_type error_position;
	};
}
