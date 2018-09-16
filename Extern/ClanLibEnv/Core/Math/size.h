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
*/


#pragma once

#include "vec2.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	template<typename Type>
	class Vec2;

	template<typename Type>
	class Vec3;

	template<typename Type>
	class Vec4;

	/// \brief 2D (width,height) size structure.
	///
	/// These line templates are defined for: int (Size), float (Sizef), double (Sized)
	///
	template<typename Type>
	class Sizex
	{
	public:
		/// \brief Constructs a size structure.
		Sizex() : width(0), height(0) { return; }

		/// \brief Constructs a size structure.
		///
		/// \param width Initial width of size structure.
		/// \param height Initial height of size structure.
		Sizex(Type width, Type height)
			: width(width), height(height) { }

		/// \brief Constructs a size structure.
		///
		/// \param s = Size structure to construct this one from.
		Sizex(const Sizex<Type> &s)
			: width(s.width), height(s.height) {}

		/// \brief Size width.
		Type width;

		/// \brief Size height.
		Type height;

		operator Vec2<Type>() const
		{
			return Vec2<Type>(width, height);
		}

		/// \brief Size += Size operator.
		Sizex<Type> &operator+=(const Sizex<Type> &s)
		{
			width += s.width; height += s.height; return *this;
		}

		/// \brief Size -= Size operator.
		Sizex<Type> &operator-=(const Sizex<Type> &s)
		{
			width -= s.width; height -= s.height; return *this;
		}

		/// \brief Size + Size operator.
		Sizex<Type> operator+(const Sizex<Type> &s) const
		{
			return Sizex<Type>(width + s.width, height + s.height);
		}

		/// \brief Size - Size operator.
		Sizex<Type> operator-(const Sizex<Type> &s) const
		{
			return Sizex<Type>(width - s.width, height - s.height);
		}

		/// \brief Size += operator.
		Sizex<Type> &operator+=(const Type &s)
		{
			width += s; height += s; return *this;
		}

		/// \brief Size -= operator.
		Sizex<Type> &operator-=(const Type &s)
		{
			width -= s; height -= s; return *this;
		}

		/// \brief Size *= operator.
		Sizex<Type> &operator*=(const Type &s)
		{
			width *= s; height *= s; return *this;
		}

		/// \brief Size /= operator.
		Sizex<Type> &operator/=(const Type &s)
		{
			width /= s; height /= s; return *this;
		}

		/// \brief Size + operator.
		Sizex<Type> operator+(const Type &s) const
		{
			return Sizex<Type>(width + s, height + s);
		}

		/// \brief Size - operator.
		Sizex<Type> operator-(const Type &s) const
		{
			return Sizex<Type>(width - s, height - s);
		}

		/// \brief Size * operator.
		Sizex<Type> operator*(const Type &s) const
		{
			return Sizex<Type>(width * s, height * s);
		}

		/// \brief Size / operator.
		Sizex<Type> operator/(const Type &s) const
		{
			return Sizex<Type>(width / s, height / s);
		}

		/// \brief Size == Size operator (deep compare).
		bool operator==(const Sizex<Type> &s) const
		{
			return (width == s.width) && (height == s.height);
		}

		/// \brief Size != Size operator (deep compare).
		bool operator!=(const Sizex<Type> &s) const
		{
			return (width != s.width) || (height != s.height);
		}
	};

	/// \brief 2D (width,height) size structure - Integer
	class Size : public Sizex<int>
	{
	public:
		Size() : Sizex<int>() {}
		Size(int width, int height) : Sizex<int>(width, height) {}
		Size(const Sizex<int> &s) : Sizex<int>(s) {}
		Size(const Vec2<int> &s) : Sizex<int>(s.x, s.y) {}

		explicit Size(const Sizex<float> &copy) { width = (int)(copy.width + 0.5f); height = (int)(copy.height + 0.5f); }
		explicit Size(const Sizex<double> &copy) { width = (int)(copy.width + 0.5); height = (int)(copy.height + 0.5); }
	};

	/// \brief 2D (width,height) size structure - Float
	class Sizef : public Sizex<float>
	{
	public:
		Sizef() : Sizex<float>() {}
		Sizef(float width, float height) : Sizex<float>(width, height) {}
		Sizef(const Sizex<float> &s) : Sizex<float>(s) {}
		Sizef(const Vec2<float> &s) : Sizex<float>(s.x, s.y) {}

		Sizef(const Sizex<int> &copy) { width = (float)copy.width; height = (float)copy.height; }
		explicit Sizef(const Sizex<double> &copy) { width = (float)copy.width; height = (float)copy.height; }
	};

	/// \brief 2D (width,height) size structure - Double
	class Sized : public Sizex<double>
	{
	public:
		Sized() : Sizex<double>() {}
		Sized(double width, double height) : Sizex<double>(width, height) {}
		Sized(const Sizex<double> &s) : Sizex<double>(s) {}
		Sized(const Vec2<double> &s) : Sizex<double>(s.x, s.y) {}

		Sized(const Sizex<int> &copy) { width = (double)copy.width; height = (double)copy.height; }
		Sized(const Sizex<float> &copy) { width = (double)copy.width; height = (double)copy.height; }
	};

	/// \}
}
