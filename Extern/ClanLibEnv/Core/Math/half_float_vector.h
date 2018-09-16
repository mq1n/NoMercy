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

#include "half_float.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "point.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class Vec2hf;
	class Vec3hf;
	class Vec4hf;

	/// \brief 2D half-float vector
	class Vec2hf
	{
	public:
		HalfFloat x;
		HalfFloat y;

		Vec2hf() : x(), y() { }

		explicit Vec2hf(const HalfFloat &scalar) : x(scalar), y(scalar) { }
		explicit Vec2hf(const Vec3hf &copy);
		explicit Vec2hf(const Vec4hf &copy);
		explicit Vec2hf(const HalfFloat &p1, const HalfFloat &p2) : x(p1), y(p2) { }
		explicit Vec2hf(const HalfFloat *array_xy) : x(array_xy[0]), y(array_xy[1]) { }

		explicit Vec2hf(float scalar) : x(scalar), y(scalar) { }
		explicit Vec2hf(const Vec3f &copy) : x(copy.x), y(copy.y) { }
		explicit Vec2hf(const Vec4f &copy) : x(copy.x), y(copy.y) { }
		explicit Vec2hf(float p1, float p2) : x(p1), y(p2) { }
		explicit Vec2hf(float *array_xy) : x(array_xy[0]), y(array_xy[1]) { }
		explicit Vec2hf(const Pointx<int> &point) : x(static_cast<float> (point.x)), y(static_cast<float> (point.y)) { }
		explicit Vec2hf(const Pointx<float> &point) : x(point.x), y(point.y) { }
		explicit Vec2hf(const Pointx<double> &point) : x(static_cast<float> (point.x)), y(static_cast<float> (point.y)) { }

		Vec2hf(const Vec2hf &copy) : x(copy.x), y(copy.y)        { }
		Vec2hf(const Vec2d &copy) : x((float)copy.x), y((float)copy.y) { }
		Vec2hf(const Vec2f &copy) : x(copy.x), y(copy.y)        { }
		Vec2hf(const Vec2i &copy) : x((float)copy.x), y((float)copy.y) { }

	public:
		operator Vec2f() const { return to_float(); }
		Vec2f to_float() const { return Vec2f((float)x, (float)y); }
	};

	/// \brief 3D half-float vector
	class Vec3hf
	{
	public:
		HalfFloat x;
		HalfFloat y;
		HalfFloat z;

		Vec3hf() : x(), y(), z() { }
		explicit Vec3hf(const HalfFloat &scalar) : x(scalar), y(scalar), z(scalar) { }
		explicit Vec3hf(const Vec2hf &copy, const HalfFloat &p3) : x(copy.x), y(copy.y), z(p3) { }
		explicit Vec3hf(const Vec4hf &copy);
		explicit Vec3hf(const HalfFloat &p1, const HalfFloat &p2, const HalfFloat &p3) : x(p1), y(p2), z(p3) { }
		explicit Vec3hf(const HalfFloat *array_xyz) : x(array_xyz[0]), y(array_xyz[1]), z(array_xyz[2]) { }

		explicit Vec3hf(float scalar) : x(scalar), y(scalar), z(scalar) 	{ }
		explicit Vec3hf(const Vec2f &copy, float p3) : x(copy.x), y(copy.y), z(p3)     	{ }
		explicit Vec3hf(const Vec4f &copy) : x(copy.x), y(copy.y), z(copy.z) 	{ }
		explicit Vec3hf(float p1, float p2, float p3) : x(p1), y(p2), z(p3)     	{ }
		explicit Vec3hf(const float *array_xyz) : x(array_xyz[0]), y(array_xyz[1]), z(array_xyz[2]){ }

		Vec3hf(const Vec3hf &copy) : x(copy.x), y(copy.y), z(copy.z)        { }
		Vec3hf(const Vec3d &copy) : x((float)copy.x), y((float)copy.y), z((float)copy.z) { }
		Vec3hf(const Vec3f &copy) : x(copy.x), y(copy.y), z(copy.z)        { }
		Vec3hf(const Vec3i &copy) : x((float)copy.x), y((float)copy.y), z((float)copy.z) { }

	public:
		operator Vec3f() const { return to_float(); }
		Vec3f to_float() const { return Vec3f((float)x, (float)y, (float)z); }
	};

	/// \brief 4D half-float vector
	class Vec4hf
	{
	public:
		HalfFloat x;
		HalfFloat y;
		HalfFloat z;
		HalfFloat w;

		Vec4hf() : x(), y(), z(), w() { }
		explicit Vec4hf(const Vec4f &copy) : x(copy.x), y(copy.y), z(copy.z), w(copy.w) { }

		explicit Vec4hf(const HalfFloat &scalar) : x(scalar), y(scalar), z(scalar), w(scalar) { }
		explicit Vec4hf(const Vec2hf &copy, const HalfFloat &p3, const HalfFloat &p4) : x(copy.x), y(copy.y), z(p3), w(p4) { }
		explicit Vec4hf(const Vec2hf &copy, const Vec2hf &copy34) : x(copy.x), y(copy.y), z(copy34.x), w(copy34.y) { }
		explicit Vec4hf(const Vec3hf &copy, const HalfFloat &p4) : x(copy.x), y(copy.y), z(copy.z), w(p4) { }
		explicit Vec4hf(const HalfFloat &p1, const HalfFloat &p2, const HalfFloat &p3, const HalfFloat &p4) : x(p1), y(p2), z(p3), w(p4) { }
		explicit Vec4hf(const HalfFloat &p1, const HalfFloat &p2, const Vec2hf &copy34) : x(p1), y(p2), z(copy34.x), w(copy34.y) { }
		explicit Vec4hf(const HalfFloat *array_xyzw) : x(array_xyzw[0]), y(array_xyzw[1]), z(array_xyzw[2]), w(array_xyzw[3]) { }

		explicit Vec4hf(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar)   { }
		explicit Vec4hf(const Vec2f &copy, float p3, float p4) : x(copy.x), y(copy.y), z(p3), w(p4)       { }
		explicit Vec4hf(const Vec2f &copy, const Vec2f &copy34) : x(copy.x), y(copy.y), z(copy34.x), w(copy34.y) { }
		explicit Vec4hf(const Vec3f &copy, float p4) : x(copy.x), y(copy.y), z(copy.z), w(p4)      { }
		explicit Vec4hf(float p1, float p2, float p3, float p4) : x(p1), y(p2), z(p3), w(p4)       { }
		explicit Vec4hf(float p1, float p2, const Vec2f &copy34) : x(p1), y(p2), z(copy34.x), w(copy34.y) { }
		explicit Vec4hf(const float *array_xyzw) : x(array_xyzw[0]), y(array_xyzw[1]), z(array_xyzw[2]), w(array_xyzw[3]) { }

	public:
		operator Vec4f() const { return to_float(); }
		Vec4f to_float() const { return Vec4f((float)x, (float)y, (float)z, (float)w); }
	};

	inline Vec2hf::Vec2hf(const Vec3hf &copy) : x(copy.x), y(copy.y) {}
	inline Vec2hf::Vec2hf(const Vec4hf &copy) : x(copy.x), y(copy.y) {}

	inline Vec3hf::Vec3hf(const Vec4hf &copy) : x(copy.x), y(copy.y), z(copy.z) {}

	/// \}
}
