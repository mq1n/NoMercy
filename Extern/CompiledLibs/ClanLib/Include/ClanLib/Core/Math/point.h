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
**    Kenneth Gangstoe
**    Harry Storbacka
**    Mark Page
*/

#pragma once

#include "vec2.h"
#include "angle.h"
#include "origin.h"
#include "size.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class Pointf;
	class Pointd;

	/// \brief 2D (x,y) point structure.
	///
	/// These point templates are defined for: int (Point), float (Pointf), double (Pointd)
	template<typename Type>
	class Pointx : public Vec2<Type>
	{
	public:
		Pointx() : Vec2<Type>() {}
		Pointx(Type x, Type y) : Vec2<Type>(x, y) {}
		Pointx(const Pointx<Type> &p) : Vec2<Type>(p.x, p.y) {}
		Pointx(const Vec2<Type> &p) : Vec2<Type>(p.x, p.y) {}
	};

	/// \brief 2D (x,y) point structure - Integer
	class Point : public Pointx<int>
	{
	public:
		Point() : Pointx<int>() {}
		Point(int x, int y) : Pointx<int>(x, y) {}
		Point(const Pointx<int> &p) : Pointx<int>(p.x, p.y) {}
		Point(const Vec2<int> &p) : Pointx<int>(p.x, p.y) {}
	};

	/// \brief 2D (x,y) point structure - Float
	class Pointf : public Pointx<float>
	{
	public:
		Pointf() : Pointx<float>() {}
		Pointf(float x, float y) : Pointx<float>(x, y) {}
		Pointf(const Pointx<float> &p) : Pointx<float>(p.x, p.y) {}
		Pointf(const Vec2<float> &p) : Pointx<float>(p.x, p.y) {}
	};

	/// \brief 2D (x,y) point structure - Double
	class Pointd : public Pointx<double>
	{
	public:
		Pointd() : Pointx<double>() {}
		Pointd(double x, double y) : Pointx<double>(x, y) {}
		Pointd(const Pointx<double> &p) : Pointx<double>(p.x, p.y) {}
		Pointd(const Vec2<double> &p) : Pointx<double>(p.x, p.y) {}
	};

	/// \}
}
