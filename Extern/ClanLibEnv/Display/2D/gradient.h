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
**    Kenneth Gangstoe
**    Harry Storbacka
*/

#pragma once

#include "color.h"

namespace clan
{
	/// \addtogroup clanDisplay_2D clanDisplay 2D
	/// \{

	/// \brief Colorgradient description class.
	class Gradient
	{
	public:
		/// \brief Constructs a color gradient.
		Gradient()
		{
		}

		/// \brief Constructs a color gradient.
		///
		/// \param top_left Top left color component.
		/// \param top_right Top right color component.
		/// \param bottom_left Bottom left color component.
		/// \param bottom_right Bottom right color component.
		Gradient(const Colorf &top_left, const Colorf &top_right,
			const Colorf &bottom_left, const Colorf &bottom_right)
			: top_left(top_left), top_right(top_right), bottom_left(bottom_left),
			bottom_right(bottom_right)
		{
		}

		/// \brief Constructs a Gradient
		///
		/// \param first_color = Colorf
		/// \param second_color = Colorf
		Gradient(const Colorf &first_color, const Colorf &second_color)
			: top_left(first_color), top_right(first_color), bottom_left(second_color),
			bottom_right(second_color)
		{
		}

		/// \brief Color of top left corner of gradient.
		Colorf top_left;

		/// \brief Color of top right corner of gradient.
		Colorf top_right;

		/// \brief Color of bottom left corner of gradient.
		Colorf bottom_left;

		/// \brief Color of bottom right corner of gradient.
		Colorf bottom_right;

		/// \brief Compares gradient to another one.
		bool operator==(const Gradient &other) const
		{
			return top_left == other.top_left && top_right == other.top_right &&
				bottom_left == other.bottom_left && bottom_right == bottom_right;
		}

		/// \brief Compares gradient to another one.
		bool operator!=(const Gradient &other) const
		{
			return !(*this == other);
		}
	};

	/// \}
}
