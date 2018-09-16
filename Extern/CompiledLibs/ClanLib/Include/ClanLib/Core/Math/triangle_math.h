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
**    Mark Page
*/

#pragma once

#include "vec3.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	/// \brief Triangles
	///
	/// These triangle templates are defined for: int (Triangle), float (Trianglef), double (Triangled)
	template<typename Type>
	class Trianglex
	{
	public:
		/// \brief First triangle point
		Vec2<Type> p;

		// \brief Second triangle point
		Vec2<Type> q;

		// \brief Third triangle point
		Vec2<Type> r;

		Trianglex() : p(), q(), r() {}
		Trianglex(const Trianglex<Type> &copy) : p(copy.p), q(copy.q), r(copy.r) {}
		Trianglex(const Vec2<Type> &point_p, const Vec2<Type> &point_q, const Vec2<Type> &point_r) : p(point_p), q(point_q), r(point_r) {}

		/// \brief Return true if the point is inside the triangle.
		///
		/// \param point = point to test.
		/// \return true if the point is inside the triangle
		bool point_inside(const Vec2<Type> &point) const;

		Trianglex<Type> &operator = (const Trianglex<Type>& copy) { p = copy.p; q = copy.q; r = copy.r; return *this; }
		bool operator == (const Trianglex<Type>& triangle) const { return ((p == triangle.p) && (q == triangle.q) && (r == triangle.r)); }
		bool operator != (const Trianglex<Type>& triangle) const { return ((p != triangle.p) || (q != triangle.q) || (r != triangle.r)); }
	};

	/// \brief Triangles - Integer
	class Triangle : public Trianglex<int>
	{
	public:
		Triangle() { }
		Triangle(const Trianglex<int> &copy) : Trianglex<int>(copy) {}
		Triangle(const Vec2<int> &point_p, const Vec2<int> &point_q, const Vec2<int> &point_r) : Trianglex<int>(point_p, point_q, point_r) {}
	};

	/// \brief Triangles - Float
	class Trianglef : public Trianglex<float>
	{
	public:
		Trianglef() { }
		Trianglef(const Trianglex<float> &copy) : Trianglex<float>(copy) {}
		Trianglef(const Vec2<float> &point_p, const Vec2<float> &point_q, const Vec2<float> &point_r) : Trianglex<float>(point_p, point_q, point_r) {}
	};

	/// \brief Triangles - Double
	class Triangled : public Trianglex<double>
	{
	public:
		Triangled() { }
		Triangled(const Trianglex<double> &copy) : Trianglex<double>(copy) {}
		Triangled(const Vec2<double> &point_p, const Vec2<double> &point_q, const Vec2<double> &point_r) : Trianglex<double>(point_p, point_q, point_r) {}
	};

	/// \}
}
