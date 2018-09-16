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
**    Harry Storbacka
**    Magnus Norddahl
*/

#pragma once

#include <memory>
#include <vector>

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class BezierCurve_Impl;
	class Pointf;
	class Angle;

	/// \brief Bezier curve generator.
	class BezierCurve
	{
	public:
		/// \brief Constructs a bezier curve generator.
		BezierCurve();

		virtual ~BezierCurve();

		/// \brief Returns the control points of the bezier.
		std::vector<Pointf> get_control_points() const;

		/// \brief Adds a control point to bezier.
		void add_control_point(float x, float y);

		void add_control_point(const Pointf &);

		/// \brief Generates points on the bezier curve.
		std::vector<Pointf> generate_curve_points(const Angle &split_angle);

		/// \brief Get a point on the bezier curve.
		Pointf get_point_relative(float pos_0_to_1) const;

	private:
		std::shared_ptr<BezierCurve_Impl> impl;
	};

	/// \}
}
