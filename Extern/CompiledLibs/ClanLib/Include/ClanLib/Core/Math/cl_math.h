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
*/

#pragma once

#include <cmath>
#include "../System/cl_platform.h"
#include "vec4.h"
#include <memory>

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	template<typename T, typename ...Args>
	std::unique_ptr<T> make_unique(Args&& ...args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}

	#undef pow2
	#undef min
	#undef max

	template<typename T>
	inline T pow2(T value)
	{
		return value*value;
	}

	template<typename A, typename B> inline A min(A a, B b) { return a < b ? a : b; }
	template<typename A, typename B> inline A max(A a, B b) { return a > b ? a : b; }

	template<typename Type>
	inline Vec2<Type> min(Vec2<Type> a, Vec2<Type> b)
	{
		return Vec2<Type>(min(a.x, b.x), min(a.y, b.y));
	}

	template<typename Type>
	inline Vec3<Type> min(Vec3<Type> a, Vec3<Type> b)
	{
		return Vec3<Type>(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
	}

	template<typename Type>
	inline Vec4<Type> min(Vec4<Type> a, Vec4<Type> b)
	{
		return Vec4<Type>(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
	}

	template<typename Type>
	inline Vec2<Type> max(Vec2<Type> a, Vec2<Type> b)
	{
		return Vec2<Type>(max(a.x, b.x), max(a.y, b.y));
	}

	template<typename Type>
	inline Vec3<Type> max(Vec3<Type> a, Vec3<Type> b)
	{
		return Vec3<Type>(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
	}

	template<typename Type>
	inline Vec4<Type> max(Vec4<Type> a, Vec4<Type> b)
	{
		return Vec4<Type>(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
	}

	template<typename A, typename B, typename C>
	inline C clamp(A val, B minval, C maxval)
	{
		return max((A)minval, min((A)maxval, val));
	}

	template<typename A, typename B, typename C>
	inline A mix(A a, B b, C mix)
	{
		return a * (C(1) - mix) + b * mix;
	}

	inline int sign(int x)
	{
		if (x < 0)
			return -1;
		else if (x > 0)
			return 1;
		else
			return 0;
	}

	inline float sign(float x)
	{
		if (x < 0.0f)
			return -1.0f;
		else if (x > 0.0f)
			return 1.0f;
		else
			return 0.0f;
	}

	inline double sign(double x)
	{
		if (x < 0.0)
			return -1.0;
		else if (x > 0.0)
			return 1.0;
		else
			return 0.0;
	}

	template<typename Type>
	inline Vec2<Type> sign(const Vec2<Type> &x)
	{
		return Vec2<Type>(sign(x.x), sign(x.y));
	}

	template<typename Type>
	inline Vec3<Type> sign(const Vec3<Type> &x)
	{
		return Vec3<Type>(sign(x.x), sign(x.y), sign(x.z));
	}

	template<typename Type>
	inline Vec4<Type> sign(const Vec4<Type> &x)
	{
		return Vec4<Type>(sign(x.x), sign(x.y), sign(x.z), sign(x.w));
	}

	template<typename A, typename B, typename C> inline C smoothstep(A edge0, B edge1, C x)
	{
		C t = clamp((x - edge0) / (edge1 - edge0), C(0), C(1));
		return t * t * (C(3) - C(2) * t);
	}

	inline int step(int edge, int x)
	{
		return x < edge ? 0 : 1;
	}

	inline long long step(long long edge, long long x)
	{
		return x < edge ? 0 : 1;
	}

	inline float step(float edge, float x)
	{
		return x < edge ? 0.0f : 1.0f;
	}

	inline double step(double edge, double x)
	{
		return x < edge ? 0.0 : 1.0;
	}

	template<typename Type>
	inline Vec2<Type> step(const Vec2<Type> &edge, const Vec2<Type> &x)
	{
		return Vec2<Type>(step(edge.x, x.x), step(edge.y, x.y));
	}

	template<typename Type>
	inline Vec3<Type> step(const Vec3<Type> &edge, const Vec3<Type> &x)
	{
		return Vec3<Type>(step(edge.x, x.x), step(edge.y, x.y), step(edge.z, x.z));
	}

	template<typename Type>
	inline Vec4<Type> step(const Vec4<Type> &edge, const Vec4<Type> &x)
	{
		return Vec4<Type>(step(edge.x, x.x), step(edge.y, x.y), step(edge.z, x.z), step(edge.w, x.w));
	}

	/// \}
}
