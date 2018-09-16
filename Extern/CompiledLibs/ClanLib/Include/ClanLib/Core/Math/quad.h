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

#include "rect.h"
#include "size.h"
#include "point.h"
#include "origin.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	/// \brief 2D quad structure.
	///
	/// These quads templates are defined for: int (Quad), float (Quadf), double (Quadd)
	///
	template<typename Type>
	class Quadx
	{
	public:
		/// \brief Constructs a quad.
		Quadx() : p(), q(), r(), s() {}

		/// \brief Constructs a quad.
		///
		/// \param new_p = coord for first point of quad.
		/// \param new_q = coord for second point of quad.
		/// \param new_r = coord for third point of quad.
		/// \param new_s = coord for forth point of quad.
		Quadx(const Vec2<Type> &new_p, const Vec2<Type> &new_q, const Vec2<Type> &new_r, const Vec2<Type> &new_s)
			: p(new_p), q(new_q), r(new_r), s(new_s) {}

		/// \brief Constructs a quad.
		///
		/// \param rect Rectangle used to initialize the quad. 
		/// <p>The corners of the rect are mapped to the quad member variables as follows:
		///   <ul>
		///     <li>Top-left: p</li>
		///     <li>Top-right: q</li>
		///     <li>Bottom-right: r</li>
		///     <li>Bottom-left: s</li>
		///   </ul>
		/// </p>
		Quadx(const Rectx<Type> &rect) : p(rect.left, rect.top), q(rect.right, rect.top),
			r(rect.right, rect.bottom), s(rect.left, rect.bottom) {}

		/// \brief Constructs a quad.
		///
		/// \param quad = Quad used to initialize this quad.
		Quadx(const Quadx<Type> &quad) : p(quad.p), q(quad.q), r(quad.r), s(quad.s) {}

		/// \brief Quad += Quad operator.
		Quadx<Type> &operator+=(const Quadx<Type> &quad)
		{
			p += quad.p; q += quad.q;  r += quad.r; s += quad.s; return *this;
		}

		/// \brief Quad -= Quad operator.
		Quadx<Type> &operator-=(const Quadx<Type> &quad)
		{
			p -= quad.p; q -= quad.q;  r -= quad.r; s -= quad.s; return *this;
		}

		/// \brief Quad += Point operator.
		Quadx<Type> &operator+=(const Vec2<Type> &point)
		{
			p += point; q += point; r += point; s += point; return *this;
		}

		/// \brief Quad -= Point operator.
		Quadx<Type> &operator-=(const Vec2<Type> &point)
		{
			p -= point; q -= point; r -= point; s -= point; return *this;
		}

		/// \brief Quad + Quad operator.
		Quadx<Type> operator+(const Quadx<Type> &quad) const
		{
			return Quadx(p + quad.p, q + quad.q, r + quad.r, s + quad.s);
		}

		/// \brief Quad - Quad operator.
		Quadx<Type> operator-(const Quadx<Type> &quad) const
		{
			return Quadx(p - quad.p, q - quad.q, r - quad.r, s - quad.s);
		}

		/// \brief Quad + Point operator.
		Quadx<Type> operator+(const Vec2<Type>  &point) const
		{
			return Quadx(p + point, q + point, r + point, s + point);
		}

		/// \brief Quad - Point operator.
		Quadx<Type> operator-(const Vec2<Type>  &point) const
		{
			return Quadx(p - point, q - point, r - point, s - point);
		}

		/// \brief Quad == Quad operator.
		bool operator==(const Quadx<Type> &quad) const
		{
			return (p == quad.p && q == quad.q && r == quad.r && s == quad.s);
		}

		/// \brief Quad != Quad operator.
		bool operator!=(const Quadx<Type> &quad) const
		{
			return (p != quad.p || q != quad.q || r != quad.r || s != quad.s);
		}

		/// \brief First Point
		Vec2<Type> p;

		/// \brief Second Point
		Vec2<Type> q;

		/// \brief Third Point
		Vec2<Type> r;

		/// \brief Fourth Point
		Vec2<Type> s;

		/// \brief Returns the width of the quad.
		Type get_width() const;

		/// \brief Returns the height of the quad.
		Type get_height() const;

		/// \brief Returns the size of the rectangle.
		Sizex<Type> get_size() const { return Sizex<Type>(get_width(), get_height()); }

		/// \brief Returns the bounding box of the quad as a Rect
		Rect get_bounds() const;

		/// \brief Rotates the Quad
		///
		/// \param hotspot Point to rotate around.
		/// \param angle Angle to rotate in degrees.
		///
		/// \return reference to this object
		Quadx<Type> &rotate(const Vec2<Type>  &hotspot, const Angle &angle);

		/// \brief Scale the Quad
		///
		/// \param sx = Scale value in x-axis
		/// \param sy = Scale value in y-axis
		///
		/// \return reference to this object
		Quadx<Type> &scale(float sx, float sy);

		/// \brief Scale the Quad
		///
		/// \param sx = Scale value in x-axis
		/// \param sy = Scale value in y-axis
		/// \param hotspot = Point to scale around.
		///
		/// \return reference to this object
		Quadx<Type> &scale(const Vec2<Type>  &hotspot, float sx, float sy);

		/// \brief Returns the center point of the quad
		Vec2<Type> center() const;

		/// \brief Applies an origin and offset pair to this rectangle
		///
		/// \param origin The new origin to adjust to from default upper-left position
		/// \param x, y Offsets applied negatively to each corner of the rectangle
		///
		/// \return reference to this object
		Quadx<Type> &apply_alignment(Origin origin, Type x, Type y);

		/// \brief Check if a point is inside or outside the quad
		bool is_inside(const Vec2<Type> &point) const;
	};

	/// \brief 2D quad structure - Integer
	class Quad : public Quadx<int>
	{
	public:
		Quad() : Quadx<int>() {}
		Quad(const Vec2<int> &new_p, const Vec2<int> &new_q, const Vec2<int> &new_r, const Vec2<int> &new_s) : Quadx<int>(new_p, new_q, new_r, new_s) {}
		Quad(const Rect &rect) : Quadx<int>(rect) {}
		Quad(const Quadx<int> &quad) : Quadx<int>(quad) {}
	};

	/// \brief 2D quad structure - Float
	class Quadf : public Quadx<float>
	{
	public:
		Quadf() : Quadx<float>() {}
		Quadf(const Vec2<float> &new_p, const Vec2<float> &new_q, const Vec2<float> &new_r, const Vec2<float> &new_s) : Quadx<float>(new_p, new_q, new_r, new_s) {}
		Quadf(const Rectf &rect) : Quadx<float>(rect) {}
		Quadf(const Quadx<float> &quad) : Quadx<float>(quad) {}
	};

	/// \brief 2D quad structure - Double
	class Quadd : public Quadx<double>
	{
	public:
		Quadd() : Quadx<double>() {}
		Quadd(const Vec2<double> &new_p, const Vec2<double> &new_q, const Vec2<double> &new_r, const Vec2<double> &new_s) : Quadx<double>(new_p, new_q, new_r, new_s) {}
		Quadd(const Rectd &rect) : Quadx<double>(rect) {}
		Quadd(const Quadx<double> &quad) : Quadx<double>(quad) {}
	};

	/// \}
}
