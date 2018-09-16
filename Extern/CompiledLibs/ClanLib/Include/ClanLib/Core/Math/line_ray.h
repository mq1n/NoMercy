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
	class LineRay2x;

	template<typename Type>
	class LineRay3x;

	template<typename Type>
	class Vec2;

	template<typename Type>
	class Vec3;

	class Angle;

	/// \brief 3D line ray
	///
	/// A ray has a start point and a direction\n
	/// These line templates are defined for: int (LineRay3), float (LineRay3f), double (LineRay3d)
	template<typename Type>
	class LineRay3x
	{
	public:
		/// \brief Start point on the line ray
		Vec3<Type> p;

		// \brief Direction of the ray
		Vec3<Type> v;

		LineRay3x() : p(), v() {}
		LineRay3x(const LineRay3x<Type> &copy) : p(copy.p), v(copy.v) {}
		LineRay3x(const Vec3<Type> &point_p, const Vec3<Type> &direction_v) : p(point_p), v(direction_v) {}

		/// \brief = operator.
		LineRay3x<Type> &operator = (const LineRay3x<Type>& copy) { p = copy.p; v = copy.v; return *this; }

		/// \brief == operator.
		bool operator == (const LineRay3x<Type>& line) const { return ((p == line.p) && (v == line.v)); }

		/// \brief != operator.
		bool operator != (const LineRay3x<Type>& line) const { return ((p != line.p) || (v != line.v)); }
	};

	/// \brief 2D line ray
	///
	/// A ray has a start point and a direction\n
	/// These line templates are defined for: int (LineRay2i), float (LineRay2f), double (LineRay2d)
	template<typename Type>
	class LineRay2x
	{
	public:
		/// \brief Start point on the line ray
		Vec2<Type> p;

		// \brief Direction of the ray
		Vec2<Type> v;

		LineRay2x() : p(), v() {}
		LineRay2x(const LineRay2x<Type> &copy) : p(copy.p), v(copy.v) {}
		LineRay2x(const Vec2<Type> &point_p, const Vec2<Type> &direction_v) : p(point_p), v(direction_v) {}

		/// \brief = operator.
		LineRay2x<Type> &operator = (const LineRay2x<Type>& copy) { p = copy.p; v = copy.v; return *this; }

		/// \brief == operator.
		bool operator == (const LineRay2x<Type>& line) const { return ((p == line.p) && (v == line.v)); }

		/// \brief != operator.
		bool operator != (const LineRay2x<Type>& line) const { return ((p != line.p) || (v != line.v)); }
	};

	/// \brief 2D line ray - Integer
	///
	/// A ray has a start point and a direction\n
	class LineRay2 : public LineRay2x<int>
	{
	public:
		LineRay2() : LineRay2x<int>() {}
		LineRay2(const LineRay2x<int> &copy) : LineRay2x<int>(copy) {}
		LineRay2(const Vec2<int> &point_p, const Vec2<int> &direction_v) : LineRay2x<int>(point_p, direction_v) {}
	};

	/// \brief 2D line ray - Float
	///
	/// A ray has a start point and a direction\n
	class LineRay2f : public LineRay2x<float>
	{
	public:
		LineRay2f() : LineRay2x<float>() {}
		LineRay2f(const LineRay2x<float> &copy) : LineRay2x<float>(copy) {}
		LineRay2f(const Vec2<float> &point_p, const Vec2<float> &direction_v) : LineRay2x<float>(point_p, direction_v) {}
	};

	/// \brief 2D line ray - Double
	///
	/// A ray has a start point and a direction\n
	class LineRay2d : public LineRay2x<double>
	{
	public:
		LineRay2d() : LineRay2x<double>() {}
		LineRay2d(const LineRay2x<double> &copy) : LineRay2x<double>(copy) {}
		LineRay2d(const Vec2<double> &point_p, const Vec2<double> &direction_v) : LineRay2x<double>(point_p, direction_v) {}
	};

	/// \brief 3D line ray - Integer
	///
	/// A ray has a start point and a direction\n
	class LineRay3 : public LineRay3x<int>
	{
	public:
		LineRay3() : LineRay3x<int>() {}
		LineRay3(const LineRay3x<int> &copy) : LineRay3x<int>(copy) {}
		LineRay3(const Vec3<int> &point_p, const Vec3<int> &direction_v) : LineRay3x<int>(point_p, direction_v) {}
	};

	/// \brief 3D line ray - Float
	///
	/// A ray has a start point and a direction\n
	class LineRay3f : public LineRay3x<float>
	{
	public:
		LineRay3f() : LineRay3x<float>() {}
		LineRay3f(const LineRay3x<float> &copy) : LineRay3x<float>(copy) {}
		LineRay3f(const Vec3<float> &point_p, const Vec3<float> &direction_v) : LineRay3x<float>(point_p, direction_v) {}
	};

	/// \brief 3D line ray - Double
	///
	/// A ray has a start point and a direction\n
	class LineRay3d : public LineRay3x<double>
	{
	public:
		LineRay3d() : LineRay3x<double>() {}
		LineRay3d(const LineRay3x<double> &copy) : LineRay3x<double>(copy) {}
		LineRay3d(const Vec3<double> &point_p, const Vec3<double> &direction_v) : LineRay3x<double>(point_p, direction_v) {}
	};

	/// \}
}
