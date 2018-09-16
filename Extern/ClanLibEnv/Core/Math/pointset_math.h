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
**    Emanuel Griesen
**    Harry Storbacka
*/

#pragma once

#include <vector>
#include "point.h"
#include "circle.h"
#include "rect.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	/// \brief Math operations related to point sets.
	class PointSetMath
	{
	public:
		/// \brief Find minimum spanning circle for the set of points
		///
		/// \param points the points to calulate on
		static Circlef minimum_enclosing_disc(
			const std::vector<Pointf> &points);

		/// \brief Return the convex hull of the given set of points
		///
		/// \param points the set of points
		static std::vector<Pointf> convex_hull_from_polygon(std::vector<Pointf> &points);

		static Rect bounding_box(const std::vector<Pointf> &points);

		static void calculate_minimum_enclosing_disc(
			Circlef &smalldisc,
			const std::vector<Pointf> &points,
			int start,
			int end);

		static void minimum_disc_with_1point(
			Circlef &smalldisc,
			const std::vector<Pointf> &points,
			int start,
			unsigned int i);

		static void minimum_disc_with_2points(
			Circlef &smalldisc,
			const std::vector<Pointf> &points,
			int start,
			unsigned int i,
			unsigned int j);

		static void minimum_disc_with_3points(
			Circlef &smalldisc,
			const std::vector<Pointf> &points,
			unsigned int i,
			unsigned int j,
			unsigned int k);

	};

	/// \}
}
