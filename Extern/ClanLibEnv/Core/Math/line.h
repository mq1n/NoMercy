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
**    Mark Page
*/

#pragma once

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	template<typename Type>
	class Line2x;

	template<typename Type>
	class Line3x;

	template<typename Type>
	class Rectx;

	template<typename Type>
	class Vec2;

	class Angle;

	/// \brief 3D line
	///
	/// These line templates are defined for: int (Line3), float (Line3f), double (Line3d)
	template<typename Type>
	class Line3x
	{
	public:
		Vec3<Type> p;
		Vec3<Type> q;

		Line3x() : p(), q() {}
		Line3x(const Line3x<Type> &copy) : p(copy.p), q(copy.q) {}
		Line3x(const Vec3<Type> &point_p, const Vec3<Type> &point_q) : p(point_p), q(point_q) {}

		/// \brief Return the intersection of this and other line
		///
		/// \param second = The second line to use
		/// \param intersect = On Return: true if the lines intersect, false if the lines are parallel
		/// \param range = Rounding error delta, to use to judge whether of not the lines intersect
		/// \return The point
		Vec3<Type> get_intersection(const Line3x<Type> &second, bool &intersect, Type range = (Type) 0.5) const;

		/// \brief = operator.
		Line3x<Type> &operator = (const Line3x<Type>& copy) { p = copy.p; q = copy.q; return *this; }

		/// \brief == operator.
		bool operator == (const Line3x<Type>& line) const { return ((p == line.p) && (q == line.q)); }

		/// \brief != operator.
		bool operator != (const Line3x<Type>& line) const { return ((p != line.p) || (q != line.q)); }
	};

	/// \brief 2D line
	///
	/// These line templates are defined for: int (Line2i), float (Line2f), double (Line2d)
	template<typename Type>
	class Line2x
	{
	public:
		/// \brief First point on the line
		Vec2<Type> p;

		// \brief Another point on the line
		Vec2<Type> q;

		Line2x() : p(), q() { }
		Line2x(const Line2x<Type> &copy) : p(copy.p), q(copy.q) {}
		Line2x(const Vec2<Type> &point_p, const Vec2<Type> &point_q) : p(point_p), q(point_q) {}
		Line2x(const Vec2<Type> &point_p, Type gradient) : p(point_p), q(static_cast<Type> (1), gradient) {}

		/// \brief Return the intersection of this and other line
		///
		/// \param second = The second line to use
		/// \param intersect = On Return: true if the lines intersect, false if the lines are parallel
		/// \return The point
		Vec2<Type> get_intersection(const Line2x<Type> &second, bool &intersect) const;

		/// \brief Return [<0, 0, >0] if the Point P is right, on or left of the line trough A,B
		///
		/// \param point = The point
		/// \return Value representing - left (>0), centre (=0), or right (<0)
		Type point_right_of_line(Vec2<Type> point) const { return (q.x - p.x) * (point.y - p.y) - (point.x - p.x) * (q.y - p.y); }

		/// \brief = operator.
		Line2x<Type> &operator = (const Line2x<Type>& copy) { p = copy.p; q = copy.q; return *this; }

		/// \brief == operator.
		bool operator == (const Line2x<Type>& line) const { return ((p == line.p) && (q == line.q)); }

		/// \brief != operator.
		bool operator != (const Line2x<Type>& line) const { return ((p != line.p) || (q != line.q)); }
	};

	/// \brief 2D line - Integer
	class Line2 : public Line2x<int>
	{
	public:
		Line2() : Line2x<int>() { }
		Line2(const Line2x<int> &copy) : Line2x<int>(copy) { }
		Line2(const Vec2<int> &point_p, const Vec2<int> &point_q) : Line2x<int>(point_p, point_q) { }
		Line2(const Vec2<int> &point_p, int gradient) : Line2x<int>(point_p, gradient) { }
	};

	/// \brief 2D line - Float
	class Line2f : public Line2x<float>
	{
	public:
		Line2f() : Line2x<float>() { }
		Line2f(const Line2x<float> &copy) : Line2x<float>(copy) { }
		Line2f(const Vec2<float> &point_p, const Vec2<float> &point_q) : Line2x<float>(point_p, point_q) { }
		Line2f(const Vec2<float> &point_p, float gradient) : Line2x<float>(point_p, gradient) { }
	};

	/// \brief 2D line - Double
	class Line2d : public Line2x<double>
	{
	public:
		Line2d() : Line2x<double>() { }
		Line2d(const Line2x<double> &copy) : Line2x<double>(copy) { }
		Line2d(const Vec2<double> &point_p, const Vec2<double> &point_q) : Line2x<double>(point_p, point_q) { }
		Line2d(const Vec2<double> &point_p, double gradient) : Line2x<double>(point_p, gradient) { }
	};

	/// \brief 3D line - Integer
	class Line3 : public Line3x<int>
	{
	public:
		Line3() : Line3x<int>() { }
		Line3(const Line3x<int> &copy) : Line3x<int>(copy) { }
		Line3(const Vec3<int> &point_p, const Vec3<int> &point_q) : Line3x<int>(point_p, point_q) { }
	};

	/// \brief 3D line - Float
	class Line3f : public Line3x<float>
	{
	public:
		Line3f() : Line3x<float>() { }
		Line3f(const Line3x<float> &copy) : Line3x<float>(copy) { }
		Line3f(const Vec3<float> &point_p, const Vec3<float> &point_q) : Line3x<float>(point_p, point_q) { }
	};

	/// \brief 3D line - Double
	class Line3d : public Line3x<double>
	{
	public:
		Line3d() : Line3x<double>() { }
		Line3d(const Line3x<double> &copy) : Line3x<double>(copy) { }
		Line3d(const Vec3<double> &podouble_p, const Vec3<double> &podouble_q) : Line3x<double>(podouble_p, podouble_q) { }
	};

	/// \}
}
