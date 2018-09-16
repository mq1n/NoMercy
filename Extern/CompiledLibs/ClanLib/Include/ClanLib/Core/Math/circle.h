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

#include "point.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	/// \brief Circle
	///
	/// These circle templates are defined for: int (Circle), float (Circlef), double (Circled)
	template<typename Type>
	class Circlex
	{
	public:
		/// \brief Circle center point
		Vec2<Type> position;

		/// \brief Circle radius
		Type radius;

		Circlex(Type x, Type y, Type radius) : position(x, y), radius(radius) { }
		Circlex(const Vec2<Type> &p, Type radius) : position(p), radius(radius) { }
		Circlex() : position(), radius(static_cast<Type> (0))  { }
		Circlex(const Circlex<Type> &copy) : position(copy.position), radius(copy.radius) {}

		bool is_inside(const Vec2<Type> &point) { return radius >= position.distance(point); }

		Circlex<Type> &operator = (const Circlex<Type>& copy) { position = copy.position; radius = copy.radius; return *this; }
		bool operator == (const Circlex<Type>& circle) const { return ((position == circle.position) && (radius == circle.radius)); }
		bool operator != (const Circlex<Type>& circle) const { return ((position != circle.position) || (radius != circle.radius)); }
	};

	/// \brief Circle - Integer
	class Circle : public Circlex<int>
	{
	public:
		Circle(int x, int y, int radius) : Circlex<int>(x, y, radius) { }
		Circle(const Vec2<int> &p, int radius) : Circlex<int>(p, radius) { }
		Circle() : Circlex<int>(){ }
		Circle(const Circlex<int> &copy) : Circlex<int>(copy){ }
	};

	/// \brief Circle - Float
	class Circlef : public Circlex<float>
	{
	public:
		Circlef(float x, float y, float radius) : Circlex<float>(x, y, radius) { }
		Circlef(const Vec2<float> &p, float radius) : Circlex<float>(p, radius) { }
		Circlef() : Circlex<float>(){ }
		Circlef(const Circlex<float> &copy) : Circlex<float>(copy){ }
	};

	/// \brief Circle - Double
	class Circled : public Circlex<double>
	{
	public:
		Circled(double x, double y, double radius) : Circlex<double>(x, y, radius) { }
		Circled(const Vec2<double> &p, double radius) : Circlex<double>(p, radius) { }
		Circled() : Circlex<double>(){ }
		Circled(const Circlex<double> &copy) : Circlex<double>(copy){ }
	};

	/// \}
}
