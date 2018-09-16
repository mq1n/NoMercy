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
**    Mark Page
*/

#pragma once

#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_Font clanDisplay Font
	/// \{

	class FontMetrics_Impl;

	/// \brief Font metrics class.
	///
	/// <img src="../../img/FontMetrics.png">
	class FontMetrics
	{
	public:
		FontMetrics();

		FontMetrics(
			float height,
			float ascent,
			float descent,
			float internal_leading,
			float external_leading,
			float line_height,		// If 0, then line_height is calculated as height + external_leading
			float pixel_ratio
			);

		~FontMetrics();

		/// \brief Returns the height of the font.
		float get_height() const;

		/// \brief Return the distance between lines.
		float get_line_height() const;

		/// \brief Returns the baseline offset from the top of a line
		float get_baseline_offset() const;

		/// \brief Returns the font ascender.
		float get_ascent() const;

		/// \brief Returns the font descender.
		float get_descent() const;

		/// \brief Returns the amount of leading (space) inside the bounds set by the get_height() function.
		float get_internal_leading() const;

		/// \brief Returns the amount of extra leading (space) that to add between rows.
		float get_external_leading() const;

	private:
		std::shared_ptr<FontMetrics_Impl> impl;
	};

	/// \}
}
