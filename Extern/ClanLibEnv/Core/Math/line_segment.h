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
	class LineSegment2x;

	template<typename Type>
	class LineSegment3x;

	template<typename Type>
	class Vec2;

	template<typename Type>
	class Vec3;

	template<typename Type>
	class Rectx;

	class Angle;

	/// \brief 3D line segment
	///
	/// A line segment has a start point and an end point\n
	/// These line templates are defined for: int (LineSegment3), float (LineSegment3f), double (LineSegment3d)
	template<typename Type>
	class LineSegment3x
	{
	public:
		/// \brief Start point on the line
		Vec3<Type> p;

		// \brief End point on the line
		Vec3<Type> q;

		LineSegment3x() : p(), q() {}
		LineSegment3x(const LineSegment3x<Type> &copy) : p(copy.p), q(copy.q) {}
		LineSegment3x(const Vec3<Type> &point_p, const Vec3<Type> &point_q) : p(point_p), q(point_q) {}

		/// \brief Get the midpoint of this line
		///
		/// \return The midpoint
		Vec3<Type> get_midpoint() const { return Vec3<Type>((q.x + p.x) / ((Type)2), (q.y + p.y) / ((Type)2), (q.z + p.z) / ((Type)2)); };

		/// \brief Calculate the distance from a line segment to a point
		///
		/// \param point = The point
		/// \param dest_intercept = On Return: The point on the line closest to the point
		/// \return The Distance
		Type point_distance(const Vec3<Type> &point, Vec3<Type> &dest_intercept) const;

		/// \brief = operator.
		LineSegment3x<Type> &operator = (const LineSegment3x<Type>& copy) { p = copy.p; q = copy.q; return *this; }

		/// \brief == operator.
		bool operator == (const LineSegment3x<Type>& line) const { return ((p == line.p) && (q == line.q)); }

		/// \brief != operator.
		bool operator != (const LineSegment3x<Type>& line) const { return ((p != line.p) || (q != line.q)); }
	};

	/// \brief 2D line segment
	///
	/// A line segment has a start point and an end point\n
	/// These line templates are defined for: int (LineSegment2i), float (LineSegment2f), double (LineSegment2d)
	template<typename Type>
	class LineSegment2x
	{
	public:
		/// \brief Start point on the line
		Vec2<Type> p;

		// \brief End point on the line
		Vec2<Type> q;

		LineSegment2x() : p(), q() {}
		LineSegment2x(const LineSegment2x<Type> &copy) : p(copy.p), q(copy.q) {}
		LineSegment2x(const Vec2<Type> &point_p, const Vec2<Type> &point_q) : p(point_p), q(point_q) {}

		/// \brief Get the midpoint of this line
		///
		/// \return The midpoint
		Vec2<Type> get_midpoint() const { return Vec2<Type>((q.x + p.x) / ((Type)2), (q.y + p.y) / ((Type)2)); };

		/// \brief Return the distance from a point to a line.
		///
		/// \param point = The point.
		Type point_distance(const Vec2<Type> &point);

		/// \brief Return true if two line segments are collinear. (All points are on the same line.)
		///
		/// \param second = The second line to check with
		/// \return true = They are collinear
		bool collinear(const LineSegment2x<Type> &second) const;

		/// \brief Return true if two line segments intersect.
		///
		/// \param second = Second line.
		/// \param collinear_intersect = true if a collision is reported when all points are on the same line.
		/// \return true = Intersects
		bool intersects(const LineSegment2x<Type> &second, bool collinear_intersect) const;

		/// \brief Return the intersection point of two lines.
		///
		/// \param second = Second line.
		/// \param intersect = On Return: The intercept. If the lines are parallel, this contains this line's first point
		/// \return true if the lines intersect, false if the lines are parallel
		Vec2<Type> get_intersection(const LineSegment2x<Type> &second, bool &intersect) const;

		/// \brief Return [<0, 0, >0] if the Point P is right, on or left of the line trough A,B
		///
		/// \param point = The point
		/// \return Value representing - left (>0), centre (=0), or right (<0)
		Type point_right_of_line(const Vec2<Type> &point) const { return (q.x - p.x) * (point.y - p.y) - (point.x - p.x) * (q.y - p.y); }

		/// \brief Return the normal vector of the line from point A to point B.
		///
		/// When using Vec2i, the vector is an 8 bit fraction (multiplied by 256)
		///
		/// \return The normal vector
		Vec2<Type> normal() const;

		/// \brief Clip this line to a rectangle
		///
		/// If clipping was not successful, this object is undefined
		///
		/// \param rect = Rectangle to clip to
		/// \param clipped = On Return: true if the line could be clipped, false if line exists outside the rectangle
		/// \return reference to this object
		LineSegment2x<Type> &clip(const Rectx<Type> &rect, bool &clipped);

		/// \brief = operator.
		LineSegment2x<Type> &operator = (const LineSegment2x<Type>& copy) { p = copy.p; q = copy.q; return *this; }

		/// \brief == operator.
		bool operator == (const LineSegment2x<Type>& line) const { return ((p == line.p) && (q == line.q)); }

		/// \brief != operator.
		bool operator != (const LineSegment2x<Type>& line) const { return ((p != line.p) || (q != line.q)); }
	};

	/// \brief 2D line segment - Integer
	///
	/// A line segment has a start point and an end point\n
	class LineSegment2 : public LineSegment2x<int>
	{
	public:
		LineSegment2() : LineSegment2x<int>() {}
		LineSegment2(const LineSegment2x<int> &copy) : LineSegment2x<int>(copy) {}
		LineSegment2(const Vec2<int> &point_p, const Vec2<int> &point_q) : LineSegment2x<int>(point_p, point_q) {}
	};

	/// \brief 2D line segment - Float
	///
	/// A line segment has a start point and an end point\n
	class LineSegment2f : public LineSegment2x<float>
	{
	public:
		LineSegment2f() : LineSegment2x<float>() {}
		LineSegment2f(const LineSegment2x<float> &copy) : LineSegment2x<float>(copy) {}
		LineSegment2f(const Vec2<float> &point_p, const Vec2<float> &point_q) : LineSegment2x<float>(point_p, point_q) {}
	};

	/// \brief 2D line segment - Double
	///
	/// A line segment has a start point and an end point\n
	class LineSegment2d : public LineSegment2x<double>
	{
	public:
		LineSegment2d() : LineSegment2x<double>() {}
		LineSegment2d(const LineSegment2x<double> &copy) : LineSegment2x<double>(copy) {}
		LineSegment2d(const Vec2<double> &point_p, const Vec2<double> &point_q) : LineSegment2x<double>(point_p, point_q) {}
	};

	/// \brief 3D line segment - Integer
	///
	/// A line segment has a start point and an end point\n
	class LineSegment3 : public LineSegment3x<int>
	{
	public:
		LineSegment3() : LineSegment3x<int>() {}
		LineSegment3(const LineSegment3x<int> &copy) : LineSegment3x<int>(copy) {}
		LineSegment3(const Vec3<int> &point_p, const Vec3<int> &point_q) : LineSegment3x<int>(point_p, point_q) {}
	};

	/// \brief 3D line segment - Float
	///
	/// A line segment has a start point and an end point\n
	class LineSegment3f : public LineSegment3x<float>
	{
	public:
		LineSegment3f() : LineSegment3x<float>() {}
		LineSegment3f(const LineSegment3x<float> &copy) : LineSegment3x<float>(copy) {}
		LineSegment3f(const Vec3<float> &point_p, const Vec3<float> &point_q) : LineSegment3x<float>(point_p, point_q) {}
	};

	/// \brief 3D line segment - Double
	///
	/// A line segment has a start point and an end point\n
	class LineSegment3d : public LineSegment3x<double>
	{
	public:
		LineSegment3d() : LineSegment3x<double>() {}
		LineSegment3d(const LineSegment3x<double> &copy) : LineSegment3x<double>(copy) {}
		LineSegment3d(const Vec3<double> &point_p, const Vec3<double> &point_q) : LineSegment3x<double>(point_p, point_q) {}
	};

	/// \}
}
