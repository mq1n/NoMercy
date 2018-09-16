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

namespace clan
{
	/// \brief Class for handling HTML urls.
	class HTMLUrl
	{
	public:
		HTMLUrl();
		HTMLUrl(const std::string &url, const HTMLUrl &base = HTMLUrl());
		std::string to_string() const;

		std::string scheme;

		// http scheme:
		std::string host;
		std::string port;
		std::string path;
		std::string query;

		// data scheme:
		std::string content_type;
		std::string encoding;
		std::string data;

	private:
		std::string read_scheme(std::string &input, std::string::size_type &pos);
	};
}
