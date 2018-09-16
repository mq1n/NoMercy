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

#include "../../Core/Math/rect.h"

namespace clan
{
	/// \addtogroup clanDisplay_Font clanDisplay Font
	/// \{

	/// \brief Glyph metrics class.
	class GlyphMetrics
	{
	public:
		GlyphMetrics() { }
		GlyphMetrics(const Pointf &bbox_offset, const Sizef &bbox_size, const Sizef &advance) : bbox_offset(bbox_offset), bbox_size(bbox_size), advance(advance) { }

		/// \brief Bounding box offset relative to the write cursor position
		Pointf bbox_offset;

		/// \brief Bounding box size
		/// The bounding box is the smallest rectangle that completely encloses the glyph
		Sizef bbox_size;

		/// \brief Distance the write cursor is moved
		Sizef advance;
	};

	/// \}
}
