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
	/// \addtogroup clanDisplay_2D clanDisplay 2D
	/// \{

	class Color;
	class Colorf;

	/// \brief Color HSV description class.
	///
	/// These templates are defined for: int (ColorHSVi), float (ColorHSVf), double (ColorHSVd)
	template<typename Type, typename ColorType>
	class ColorHSVx
	{
	public:
		/// \brief Constructs a color.
		///
		ColorHSVx() : h((Type)0), s((Type)0), v((Type)0), a((Type)0) {}
		ColorHSVx(Type h, Type s, Type v, Type a) : h(h), s(s), v(v), a(a) {}
		ColorHSVx(const ColorHSVx<Type, ColorType> &copy) { h = copy.h; s = copy.s; v = copy.v; a = copy.a; }
		ColorHSVx(const Color &color);
		ColorHSVx(const Colorf &color);

		Type h, s, v, a;

		operator ColorType();

		/// \brief = operator.
		ColorHSVx<Type, ColorType> &operator = (const ColorHSVx<Type, ColorType>& copy) { h = copy.h; s = copy.s; v = copy.v; a = copy.a; return *this; }

		/// \brief == operator.
		bool operator == (const ColorHSVx<Type, ColorType>& color) const { return ((h == color.h) && (s == color.s) && (v == color.v) && (a == color.a)); }

		/// \brief != operator.
		bool operator != (const ColorHSVx<Type, ColorType>& color) const { return ((h != color.h) || (s != color.s) || (v != color.v) || (a != color.a)); }
	};

	typedef ColorHSVx<int, Color> ColorHSVi;
	typedef ColorHSVx<float, Colorf> ColorHSVf;
	typedef ColorHSVx<double, Colorf> ColorHSVd;

	/// \}
}
