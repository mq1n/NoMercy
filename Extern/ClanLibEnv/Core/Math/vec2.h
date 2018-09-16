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
**    Harry Storbacka
*/

#pragma once

#include <cmath>
#include "vec3.h"
#include "vec4.h"
#include "origin.h"

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

	template<typename Type>
	class Mat2;

	template<typename Type>
	class Mat3;

	template<typename Type>
	class Mat4;

	template<typename Type>
	class Sizex;

	template<typename Type>
	class Pointx;

	class Angle;

	/// \brief 2D vector
	///
	/// These vector templates are defined for:\n
	/// char (Vec2c), unsigned char (Vec2uc), short (Vec2s),\n
	/// unsigned short (Vec2us), int (Vec2i), unsigned int (Vec2ui), float (Vec2f), double (Vec2d)
	template<typename Type>
	class Vec2
	{
	public:
		typedef Type datatype;

		union { Type x; Type s; Type r; };
		union { Type y; Type t; Type g; };

		Vec2() : x(0), y(0) { }
		explicit Vec2(const Type &scalar) : x(scalar), y(scalar) { }
		explicit Vec2(const Vec3<Type> &copy) { x = copy.x; y = copy.y; }
		explicit Vec2(const Vec4<Type> &copy) { x = copy.x; y = copy.y; }
		explicit Vec2(const Type &p1, const Type &p2) : x(p1), y(p2) { }
		explicit Vec2(const Type *array_xy) : x(array_xy[0]), y(array_xy[1]) { }

		Vec2(const Vec2<double> &copy);
		Vec2(const Vec2<float> &copy);
		Vec2(const Vec2<int> &copy);

		/// \brief Normalizes a vector
		///
		/// Operates in the native datatype
		///
		/// \param vector = Vector to use
		/// \return normalized vector
		static Vec2<Type> normalize(const Vec2<Type>& vector);

		/// \brief Dot products a vector with an other vector.
		///
		/// Operates in the native datatype
		///
		/// \param vector_1 = First vector used for the dot product.
		/// \param vector_2 = Second vector used for the dot product.
		/// \return The resulting dot product of the two vectors.
		static Type dot(const Vec2<Type>& vector_1, const Vec2<Type>& vector_2) { return vector_1.x*vector_2.x + vector_1.y*vector_2.y; }

		/// \brief Rounds all components on a vector
		///
		/// Uses Asymmetric Arithmetic Rounding
		///
		/// \param vector = Vector to use
		/// \return The rounded vector
		static Vec2<Type> round(const Vec2<Type>& vector);

		/// \brief Rotate a vector around another point.
		///
		/// \param vector = Vector to use
		/// \param hotspot The point around which to rotate.
		/// \param angle = Angle to rotate.
		static Vec2<Type> rotate(const Vec2<Type>& vector, const Vec2<Type>& hotspot, const Angle &angle);

		/// \brief Returns the anchor point for the origin within the dimensions of the size structure.
		///
		/// \param origin = The origin
		/// \param size = The size
		/// \return The point
		static Pointx<Type> calc_origin(Origin origin, const Sizex<Type> &size);

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param first = Value A
		/// \param second = Value B
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		static bool is_equal(const Vec2<Type> &first, const Vec2<Type> &second, Type epsilon)
		{
			Type diff_x = second.x - first.x; Type diff_y = second.y - first.y;
			return (diff_x >= -epsilon && diff_x <= epsilon && diff_y >= -epsilon && diff_y <= epsilon);
		}

		/// \brief Returns the length (magnitude) of this vector.
		///
		/// Operates in the native datatype
		///
		/// \return the length of the vector
		Type length() const;

		/// \brief Normalizes this vector
		///
		/// Operates in the native datatype
		///
		/// \return reference to this object
		Vec2<Type> &normalize();

		/// \brief Dot products this vector with an other vector.
		///
		/// Operates in the native datatype
		///
		/// \param vector Second vector used for the dot product.
		///
		/// \return The resulting dot product of the two vectors.
		Type dot(const Vec2<Type>& vector) const { return x*vector.x + y*vector.y; }

		/// \brief Calculate the angle between this vector and an other vector.
		///
		/// \param vector = Second vector used to calculate angle.
		///
		/// \return The angle between the two vectors
		Angle angle(const Vec2<Type>& vector) const;

		/// \brief Calculate the angle between this vector and an other vector, where the vectors are unit vectors
		///
		/// \param vector = Second vector used to calculate angle.
		///
		/// \return The angle between the two vectors
		Angle angle_normed(const Vec2<Type>& vector) const;

		/// \brief Calculate the angle of the line joining this point and other point
		///
		/// \param point = Second point in the line
		///
		/// \return The angle
		Angle angle_line(const Vec2<Type>& point) const;

		/// \brief Calculate the distance between this vector and an other vector.
		///
		/// \param vector = Second vector used to calculate distance.
		///
		/// \return The distance between the two vectors.
		Type distance(const Vec2<Type>& vector) const;

		/// \brief Rounds all components of this vector
		///
		/// Uses Asymmetric Arithmetic Rounding
		///
		/// \return reference to this object
		Vec2<Type> &round();

		/// \brief Rotate this vector around another point.
		///
		/// \param hotspot The point around which to rotate.
		/// \param angle = Angle to rotate.
		///
		/// \return reference to this object
		Vec2<Type> &rotate(const Vec2<Type>& hotspot, const Angle &angle);

		/// \brief Rounds a value for the datatype
		///
		/// For doubles and floats, this function does not round.
		/// \param value = Value to round
		/// \return The rounded value
		Type round_value(float value) const;

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param other = Other value
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		bool is_equal(const Vec2<Type> &other, Type epsilon) const { return Vec2<Type>::is_equal(*this, other, epsilon); }

		/// \brief += operator.
		void operator += (const Vec2<Type>& vector) { x += vector.x; y += vector.y; }

		/// \brief += operator.
		void operator += (Type value) { x += value; y += value; }

		/// \brief -= operator.
		void operator -= (const Vec2<Type>& vector) { x -= vector.x; y -= vector.y; }

		/// \brief -= operator.
		void operator -= (Type value) { x -= value; y -= value; }

		/// \brief - operator.
		Vec2<Type> operator - () const { return Vec2<Type>(-x, -y); }

		/// \brief *= operator.
		void operator *= (const Vec2<Type>& vector) { x *= vector.x; y *= vector.y; }

		/// \brief *= operator.
		void operator *= (Type value) { x *= value; y *= value; }

		/// \brief /= operator.
		void operator /= (const Vec2<Type>& vector) { x /= vector.x; y /= vector.y; }

		/// \brief /= operator.
		void operator /= (Type value) { x /= value; y /= value; }

		/// \brief = operator.
		Vec2<Type> &operator = (const Vec2<Type>& vector) { x = vector.x; y = vector.y; return *this; }

		/// \brief == operator.
		bool operator == (const Vec2<Type>& vector) const { return ((x == vector.x) && (y == vector.y)); }

		/// \brief != operator.
		bool operator != (const Vec2<Type>& vector) const { return ((x != vector.x) || (y != vector.y)); }

		/// \brief < operator.
		bool operator < (const Vec2<Type>& vector) const { return y < vector.y || (y == vector.y && x < vector.x); }
		/// \}
	};

	/// \brief + operator.
	template<typename Type>
	Vec2<Type> operator + (const Vec2<Type>& v1, const Vec2<Type>& v2) { return Vec2<Type>(v1.x + v2.x, v1.y + v2.y); }

	/// \brief + operator.
	template<typename Type>
	Vec2<Type> operator + (Type s, const Vec2<Type>& v) { return Vec2<Type>(s + v.x, s + v.y); }

	/// \brief + operator.
	template<typename Type>
	Vec2<Type> operator + (const Vec2<Type>& v, Type s) { return Vec2<Type>(v.x + s, v.y + s); }

	/// \brief - operator.
	template<typename Type>
	Vec2<Type> operator - (const Vec2<Type>& v1, const Vec2<Type>& v2) { return Vec2<Type>(v1.x - v2.x, v1.y - v2.y); }

	/// \brief - operator.
	template<typename Type>
	Vec2<Type> operator - (Type s, const Vec2<Type>& v) { return Vec2<Type>(s - v.x, s - v.y); }

	/// \brief - operator.
	template<typename Type>
	Vec2<Type> operator - (const Vec2<Type>& v, Type s) { return Vec2<Type>(v.x - s, v.y - s); }

	/// \brief * operator.
	template<typename Type>
	Vec2<Type> operator * (const Vec2<Type>& v1, const Vec2<Type>& v2) { return Vec2<Type>(v1.x * v2.x, v1.y * v2.y); }

	/// \brief * operator.
	template<typename Type>
	Vec2<Type> operator * (Type s, const Vec2<Type>& v) { return Vec2<Type>(s * v.x, s * v.y); }

	/// \brief * operator.
	template<typename Type>
	Vec2<Type> operator * (const Vec2<Type>& v, Type s) { return Vec2<Type>(v.x * s, v.y * s); }

	/// \brief / operator.
	template<typename Type>
	Vec2<Type> operator / (const Vec2<Type>& v1, const Vec2<Type>& v2) { return Vec2<Type>(v1.x / v2.x, v1.y / v2.y); }

	/// \brief / operator.
	template<typename Type>
	Vec2<Type> operator / (Type s, const Vec2<Type>& v) { return Vec2<Type>(s / v.x, s / v.y); }

	/// \brief / operator.
	template<typename Type>
	Vec2<Type> operator / (const Vec2<Type>& v, Type s) { return Vec2<Type>(v.x / s, v.y / s); }

	template<typename Type>
	Vec2<Type> operator * (const Vec2<Type>& v, const Mat2<Type>& matrix)
	{
		return Vec2<Type>(
			matrix[0 * 2 + 0] * v.x + matrix[0 * 2 + 1] * v.y,
			matrix[1 * 2 + 0] * v.x + matrix[1 * 2 + 1] * v.y);
	}

	template<typename Type>
	Vec2<Type> operator * (const Mat2<Type>& matrix, const Vec2<Type>& v)
	{
		return Vec2<Type>(
			matrix[0 * 2 + 0] * v.x + matrix[1 * 2 + 0] * v.y,
			matrix[0 * 2 + 1] * v.x + matrix[1 * 2 + 1] * v.y);
	}

	//////////////////////////////////////////////////////////////////////////

	template<>
	inline Vec2<unsigned char>::Vec2(const Vec2<float> &copy) { x = (unsigned char)std::floor(copy.x + 0.5f); y = (unsigned char)std::floor(copy.y + 0.5f); }

	template<>
	inline Vec2<unsigned char>::Vec2(const Vec2<double> &copy) { x = (unsigned char)std::floor(copy.x + 0.5); y = (unsigned char)std::floor(copy.y + 0.5); }

	template<>
	inline Vec2<unsigned char>::Vec2(const Vec2<int> &copy) { x = (unsigned char)copy.x; y = (unsigned char)copy.y; }

	template<>
	inline Vec2<char>::Vec2(const Vec2<float> &copy) { x = (char)std::floor(copy.x + 0.5f); y = (char)std::floor(copy.y + 0.5f); }

	template<>
	inline Vec2<char>::Vec2(const Vec2<double> &copy) { x = (char)std::floor(copy.x + 0.5); y = (char)std::floor(copy.y + 0.5); }

	template<>
	inline Vec2<char>::Vec2(const Vec2<int> &copy) { x = (char)copy.x; y = (char)copy.y; }

	template<>
	inline Vec2<unsigned short>::Vec2(const Vec2<float> &copy) { x = (unsigned short)std::floor(copy.x + 0.5f); y = (unsigned short)std::floor(copy.y + 0.5f); }

	template<>
	inline Vec2<unsigned short>::Vec2(const Vec2<double> &copy) { x = (unsigned short)std::floor(copy.x + 0.5); y = (unsigned short)std::floor(copy.y + 0.5); }

	template<>
	inline Vec2<unsigned short>::Vec2(const Vec2<int> &copy) { x = (unsigned short)copy.x; y = (unsigned short)copy.y; }

	template<>
	inline Vec2<short>::Vec2(const Vec2<float> &copy) { x = (short)std::floor(copy.x + 0.5f); y = (short)std::floor(copy.y + 0.5f); }

	template<>
	inline Vec2<short>::Vec2(const Vec2<double> &copy) { x = (short)std::floor(copy.x + 0.5); y = (short)std::floor(copy.y + 0.5); }

	template<>
	inline Vec2<short>::Vec2(const Vec2<int> &copy) { x = (short)copy.x; y = (short)copy.y; }

	template<>
	inline Vec2<int>::Vec2(const Vec2<float> &copy) { x = (int)std::floor(copy.x + 0.5f); y = (int)std::floor(copy.y + 0.5f); }

	template<>
	inline Vec2<int>::Vec2(const Vec2<double> &copy) { x = (int)std::floor(copy.x + 0.5); y = (int)std::floor(copy.y + 0.5); }

	template<>
	inline Vec2<int>::Vec2(const Vec2<int> &copy) { x = (int)copy.x; y = (int)copy.y; }

	template<>
	inline Vec2<unsigned int>::Vec2(const Vec2<float> &copy) { x = (unsigned int)std::floor(copy.x + 0.5f); y = (unsigned int)std::floor(copy.y + 0.5f); }

	template<>
	inline Vec2<unsigned int>::Vec2(const Vec2<double> &copy) { x = (unsigned int)std::floor(copy.x + 0.5); y = (unsigned int)std::floor(copy.y + 0.5); }

	template<>
	inline Vec2<unsigned int>::Vec2(const Vec2<int> &copy) { x = (unsigned int)copy.x; y = (unsigned int)copy.y; }

	template<>
	inline Vec2<float>::Vec2(const Vec2<float> &copy) { x = (float)copy.x; y = (float)copy.y; }

	template<>
	inline Vec2<float>::Vec2(const Vec2<double> &copy) { x = (float)copy.x; y = (float)copy.y; }

	template<>
	inline Vec2<float>::Vec2(const Vec2<int> &copy) { x = (float)copy.x; y = (float)copy.y; }

	template<>
	inline Vec2<double>::Vec2(const Vec2<float> &copy) { x = (double)copy.x; y = (double)copy.y; }

	template<>
	inline Vec2<double>::Vec2(const Vec2<double> &copy) { x = (double)copy.x; y = (double)copy.y; }

	template<>
	inline Vec2<double>::Vec2(const Vec2<int> &copy) { x = (double)copy.x; y = (double)copy.y; }

	template<typename Type>
	inline Type Vec2<Type>::length() const { return (Type)floor(sqrt(float(x*x + y*y)) + 0.5f); }

	template<>
	inline double Vec2<double>::length() const { return sqrt(x*x + y*y); }

	template<>
	inline float Vec2<float>::length() const { return sqrt(x*x + y*y); }

	template<typename Type>
	inline Vec2<Type> &Vec2<Type>::normalize() { Type f = length(); if (f != 0) { x /= f; y /= f; } return *this; }

	template<typename Type>
	inline Vec2<Type> Vec2<Type>::normalize(const Vec2<Type>& vector) { Vec2<Type> dest(vector); dest.normalize(); return dest; }

	//////////////////////////////////////////////////////////////////////////

	typedef Vec2<unsigned char> Vec2ub;
	typedef Vec2<char> Vec2b;
	typedef Vec2<unsigned short> Vec2us;
	typedef Vec2<short> Vec2s;
	typedef Vec2<unsigned int> Vec2ui;
	typedef Vec2<int> Vec2i;
	typedef Vec2<float> Vec2f;
	typedef Vec2<double> Vec2d;

	/// \}
}
