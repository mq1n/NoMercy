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
**    Mark Page
*/

#pragma once

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	/// \brief Array Buffer usage enum
	enum BufferUsage
	{
		usage_stream_draw,
		usage_stream_read,
		usage_stream_copy,
		usage_static_draw,
		usage_static_read,
		usage_static_copy,
		usage_dynamic_draw,
		usage_dynamic_read,
		usage_dynamic_copy
	};

	/// \brief Array Buffer access enum
	enum BufferAccess
	{
		access_read_only,
		access_write_only,
		access_read_write,
		access_write_discard // TBD; requires usage usage_stream_draw on object
	};

	/// \}
}
