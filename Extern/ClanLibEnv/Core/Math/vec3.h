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
#include "vec2.h"
#include "vec4.h"

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

	/// \brief 3D vector
	///
	/// These vector templates are defined for:\n
	/// char (Vec3c), unsigned char (Vec3uc), short (Vec3s),\n
	/// unsigned short (Vec3us), int (Vec3i), unsigned int (Vec3ui), float (Vecdf), double (Vec3d)
	template<typename Type>
	class Vec3
	{
	public:
		typedef Type datatype;

		union { Type x; Type s; Type r; };
		union { Type y; Type t; Type g; };
		union { Type z; Type u; Type b; };

		Vec3() : x(0), y(0), z(0) { }
		explicit Vec3(const Type &scalar) : x(scalar), y(scalar), z(scalar) { }
		explicit Vec3(const Vec2<Type> &copy, const Type &p3) { x = copy.x; y = copy.y; z = p3; }
		explicit Vec3(const Vec4<Type> &copy) { x = copy.x; y = copy.y; z = copy.z; }

		Vec3(const Vec3<double> &copy);
		Vec3(const Vec3<float> &copy);
		Vec3(const Vec3<int> &copy);

		explicit Vec3(const Type &p1, const Type &p2, const Type &p3) : x(p1), y(p2), z(p3) { }
		explicit Vec3(const Type *array_xyz) : x(array_xyz[0]), y(array_xyz[1]), z(array_xyz[2]) { }

		/// \brief Normalizes a vector
		///
		/// \param vector = Vector to use
		///
		/// Operates in the native datatype
		static Vec3<Type> normalize(const Vec3<Type>& vector);

		/// \brief Dot products between two vectors.
		///
		/// \return The dot product
		static Type dot(const Vec3<Type>& vector1, const Vec3<Type>& vector2) { return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z; }

		/// \brief Calculate the cross product between two vectors.
		///
		/// \param vector1 = The first vector
		/// \param vector2 = The second vector
		/// \return The cross product
		static Vec3<Type> cross(const Vec3<Type>& vector1, const Vec3<Type>& vector2);

		/// \brief Rotate a vector around an axis. Same as glRotate[f|d](angle, a);
		///
		/// \param vector = Vector to use
		/// \param angle = Angle to rotate.
		/// \param axis = Rotation axis.
		/// \return The rotated vector
		static Vec3<Type> rotate(const Vec3<Type>& vector, const Angle &angle, const Vec3<Type>& axis);

		/// \brief Rounds all components on a vector
		///
		/// Uses Asymmetric Arithmetic Rounding
		/// \param vector = Vector to use
		/// \return The rounded vector
		static Vec3<Type> round(const Vec3<Type>& vector);

		/// \brief Calculate the reflection direction for an incident vector
		///
		/// Normal vector should be normalized
		static Vec3<Type> reflect(const Vec3<Type>& incident, const Vec3<Type>& normal);

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param first = Value A
		/// \param second = Value B
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		static bool is_equal(const Vec3<Type> &first, const Vec3<Type> &second, Type epsilon)
		{
			Type diff_x = second.x - first.x; Type diff_y = second.y - first.y; Type diff_z = second.z - first.z;
			return (diff_x >= -epsilon && diff_x <= epsilon && diff_y >= -epsilon && diff_y <= epsilon && diff_z >= -epsilon && diff_z <= epsilon);
		}

		/// \brief Returns the length (magnitude) of this vector.
		///
		/// Operates in the native datatype
		/// \return the length of the vector
		Type length() const;

		/// \brief Normalizes this vector
		///
		/// Operates in the native datatype
		/// \return reference to this object
		Vec3<Type> &normalize();

		/// \brief Dot products this vector with an other vector.
		///
		/// Operates in the native datatype
		///
		/// \param vector Second vector used for the dot product.
		/// \return The resulting dot product of the two vectors.
		Type dot(const Vec3<Type>& vector) const { return x*vector.x + y*vector.y + z*vector.z; }

		/// \brief Calculate the angle between this vector and an other vector.
		///
		/// \param vector Second vector used to calculate angle.
		///
		/// \return The angle between the two vectors.
		Angle angle(const Vec3<Type>& vector) const;

		/// \brief Calculate the angle between this vector and an other vector, where the vectors are unit vectors
		///
		/// \param vector Second vector used to calculate angle.
		///
		/// \return The angle between the two vectors.
		Angle angle_normed(const Vec3<Type>& vector) const;

		/// \brief Calculate the distance between this vector and an other vector.
		///
		/// \param vector = Second vector used to calculate distance.
		///
		/// \return The distance between the two vectors.
		Type distance(const Vec3<Type>& vector) const;

		/// \brief Calculate the cross product between this vector and an other vector.
		///
		/// Operates in the native datatype
		/// \param vector = Second vector used to perform the calculation.
		/// \return reference to this object
		Vec3<Type> &cross(const Vec3<Type>& vector);

		/// \brief Rotate this vector around an axis. Same as glRotate[f|d](angle, a);
		///
		/// \param angle Angle to rotate.
		/// \param axis Rotation axis.
		/// \return reference to this object
		Vec3<Type> &rotate(const Angle &angle, const Vec3<Type>& axis);

		/// \brief Rounds all components on this vector
		///
		//// Uses Asymmetric Arithmetic Rounding
		/// \return reference to this object
		Vec3<Type> &round();

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param other = Other value
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		bool is_equal(const Vec3<Type> &other, Type epsilon) const { return Vec3<Type>::is_equal(*this, other, epsilon); }

		/// \brief += operator.
		void operator += (const Vec3<Type>& vector) { x += vector.x; y += vector.y; z += vector.z; }

		/// \brief += operator.
		void operator += (Type value) { x += value; y += value; z += value; }

		/// \brief -= operator.
		void operator -= (const Vec3<Type>& vector) { x -= vector.x; y -= vector.y; z -= vector.z; }

		/// \brief -= operator.
		void operator -= (Type value) { x -= value; y -= value; z -= value; }

		/// \brief - operator.
		Vec3<Type> operator - () const { return Vec3<Type>(-x, -y, -z); }

		/// \brief *= operator.
		void operator *= (const Vec3<Type>& vector) { x *= vector.x; y *= vector.y; z *= vector.z; }

		/// \brief *= operator.
		void operator *= (Type value) { x *= value; y *= value; z *= value; }

		/// \brief /= operator.
		void operator /= (const Vec3<Type>& vector) { x /= vector.x; y /= vector.y; z /= vector.z; }

		/// \brief /= operator.
		void operator /= (Type value) { x /= value; y /= value; z /= value; }

		/// \brief = operator.
		Vec3<Type> &operator = (const Vec3<Type>& vector) { x = vector.x; y = vector.y; z = vector.z; return *this; }

		/// \brief == operator.
		bool operator == (const Vec3<Type>& vector) const { return ((x == vector.x) && (y == vector.y) && (z == vector.z)); }

		/// \brief != operator.
		bool operator != (const Vec3<Type>& vector) const { return ((x != vector.x) || (y != vector.y) || (z != vector.z)); }

		/// \brief < operator.
		bool operator < (const Vec3<Type>& vector) const { return z < vector.z || (z == vector.z && (y < vector.y || (y == vector.y && x < vector.x))); }
	};

	/// \brief + operator.
	template<typename Type>
	Vec3<Type> operator + (const Vec3<Type>& v1, const Vec3<Type>& v2) { return Vec3<Type>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }

	/// \brief + operator.
	template<typename Type>
	Vec3<Type> operator + (Type s, const Vec3<Type>& v) { return Vec3<Type>(s + v.x, s + v.y, s + v.z); }

	/// \brief + operator.
	template<typename Type>
	Vec3<Type> operator + (const Vec3<Type>& v, Type s) { return Vec3<Type>(v.x + s, v.y + s, v.z + s); }

	/// \brief - operator.
	template<typename Type>
	Vec3<Type> operator - (const Vec3<Type>& v1, const Vec3<Type>& v2) { return Vec3<Type>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }

	/// \brief - operator.
	template<typename Type>
	Vec3<Type> operator - (Type s, const Vec3<Type>& v) { return Vec3<Type>(s - v.x, s - v.y, s - v.z); }

	/// \brief - operator.
	template<typename Type>
	Vec3<Type> operator - (const Vec3<Type>& v, Type s) { return Vec3<Type>(v.x - s, v.y - s, v.z - s); }

	/// \brief * operator.
	template<typename Type>
	Vec3<Type> operator * (const Vec3<Type>& v1, const Vec3<Type>& v2) { return Vec3<Type>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }

	/// \brief * operator.
	template<typename Type>
	Vec3<Type> operator * (Type s, const Vec3<Type>& v) { return Vec3<Type>(s * v.x, s * v.y, s * v.z); }

	/// \brief * operator.
	template<typename Type>
	Vec3<Type> operator * (const Vec3<Type>& v, Type s) { return Vec3<Type>(v.x * s, v.y * s, v.z * s); }

	/// \brief / operator.
	template<typename Type>
	Vec3<Type> operator / (const Vec3<Type>& v1, const Vec3<Type>& v2) { return Vec3<Type>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }

	/// \brief / operator.
	template<typename Type>
	Vec3<Type> operator / (Type s, const Vec3<Type>& v) { return Vec3<Type>(s / v.x, s / v.y, s / v.z); }

	/// \brief / operator.
	template<typename Type>
	Vec3<Type> operator / (const Vec3<Type>& v, Type s) { return Vec3<Type>(v.x / s, v.y / s, v.z / s); }

	/// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
	/// Note: "vec = vector * matrix"  is different to "vec = matrix * vector"
	template<typename Type>
	Vec3<Type> operator * (const Vec3<Type>& v, const Mat3<Type>& matrix)
	{
		return Vec3<Type>(
			matrix[0 * 3 + 0] * v.x + matrix[0 * 3 + 1] * v.y + matrix[0 * 3 + 2] * v.z,
			matrix[1 * 3 + 0] * v.x + matrix[1 * 3 + 1] * v.y + matrix[1 * 3 + 2] * v.z,
			matrix[2 * 3 + 0] * v.x + matrix[2 * 3 + 1] * v.y + matrix[2 * 3 + 2] * v.z);
	}

	/// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
	/// Note: "vec = vector * matrix"  is different to "vec = matrix * vector"
	template<typename Type>
	Vec3<Type> operator * (const Mat3<Type>& matrix, const Vec3<Type>& v)
	{
		return Vec3<Type>(
			matrix[0 * 3 + 0] * v.x + matrix[1 * 3 + 0] * v.y + matrix[2 * 3 + 0] * v.z,
			matrix[0 * 3 + 1] * v.x + matrix[1 * 3 + 1] * v.y + matrix[2 * 3 + 1] * v.z,
			matrix[0 * 3 + 2] * v.x + matrix[1 * 3 + 2] * v.y + matrix[2 * 3 + 2] * v.z);
	}

	template<>
	inline Vec3<unsigned char>::Vec3(const Vec3<float> &copy) { x = (unsigned char)floor(copy.x + 0.5f); y = (unsigned char)floor(copy.y + 0.5f); z = (unsigned char)floor(copy.z + 0.5f); }

	template<>
	inline Vec3<unsigned char>::Vec3(const Vec3<double> &copy) { x = (unsigned char)floor(copy.x + 0.5); y = (unsigned char)floor(copy.y + 0.5); z = (unsigned char)floor(copy.z + 0.5); }

	template<>
	inline Vec3<unsigned char>::Vec3(const Vec3<int> &copy) { x = (unsigned char)copy.x; y = (unsigned char)copy.y; z = (unsigned char)copy.z; }

	template<>
	inline Vec3<char>::Vec3(const Vec3<float> &copy) { x = (char)floor(copy.x + 0.5f); y = (char)floor(copy.y + 0.5f); z = (char)floor(copy.z + 0.5f); }

	template<>
	inline Vec3<char>::Vec3(const Vec3<double> &copy) { x = (char)floor(copy.x + 0.5); y = (char)floor(copy.y + 0.5); z = (char)floor(copy.z + 0.5); }

	template<>
	inline Vec3<char>::Vec3(const Vec3<int> &copy) { x = (char)copy.x; y = (char)copy.y; z = (char)copy.z; }

	template<>
	inline Vec3<unsigned short>::Vec3(const Vec3<float> &copy) { x = (unsigned short)floor(copy.x + 0.5f); y = (unsigned short)floor(copy.y + 0.5f); z = (unsigned short)floor(copy.z + 0.5f); }

	template<>
	inline Vec3<unsigned short>::Vec3(const Vec3<double> &copy) { x = (unsigned short)floor(copy.x + 0.5); y = (unsigned short)floor(copy.y + 0.5); z = (unsigned short)floor(copy.z + 0.5); }

	template<>
	inline Vec3<unsigned short>::Vec3(const Vec3<int> &copy) { x = (unsigned short)copy.x; y = (unsigned short)copy.y; z = (unsigned short)copy.z; }

	template<>
	inline Vec3<short>::Vec3(const Vec3<float> &copy) { x = (short)floor(copy.x + 0.5f); y = (short)floor(copy.y + 0.5f); z = (short)floor(copy.z + 0.5f); }

	template<>
	inline Vec3<short>::Vec3(const Vec3<double> &copy) { x = (short)floor(copy.x + 0.5); y = (short)floor(copy.y + 0.5); z = (short)floor(copy.z + 0.5); }

	template<>
	inline Vec3<short>::Vec3(const Vec3<int> &copy) { x = (short)copy.x; y = (short)copy.y; z = (short)copy.z; }

	template<>
	inline Vec3<int>::Vec3(const Vec3<float> &copy) { x = (int)floor(copy.x + 0.5f); y = (int)floor(copy.y + 0.5f); z = (int)floor(copy.z + 0.5f); }

	template<>
	inline Vec3<int>::Vec3(const Vec3<double> &copy) { x = (int)floor(copy.x + 0.5); y = (int)floor(copy.y + 0.5); z = (int)floor(copy.z + 0.5); }

	template<>
	inline Vec3<int>::Vec3(const Vec3<int> &copy) { x = (int)copy.x; y = (int)copy.y; z = (int)copy.z; }

	template<>
	inline Vec3<unsigned int>::Vec3(const Vec3<float> &copy) { x = (unsigned int)floor(copy.x + 0.5f); y = (unsigned int)floor(copy.y + 0.5f); z = (unsigned int)floor(copy.z + 0.5f); }

	template<>
	inline Vec3<unsigned int>::Vec3(const Vec3<double> &copy) { x = (unsigned int)floor(copy.x + 0.5); y = (unsigned int)floor(copy.y + 0.5); z = (unsigned int)floor(copy.z + 0.5); }

	template<>
	inline Vec3<unsigned int>::Vec3(const Vec3<int> &copy) { x = (unsigned int)copy.x; y = (unsigned int)copy.y; z = (unsigned int)copy.z; }

	template<>
	inline Vec3<float>::Vec3(const Vec3<float> &copy) { x = (float)copy.x; y = (float)copy.y; z = (float)copy.z; }

	template<>
	inline Vec3<float>::Vec3(const Vec3<double> &copy) { x = (float)copy.x; y = (float)copy.y; z = (float)copy.z; }

	template<>
	inline Vec3<float>::Vec3(const Vec3<int> &copy) { x = (float)copy.x; y = (float)copy.y; z = (float)copy.z; }

	template<>
	inline Vec3<double>::Vec3(const Vec3<float> &copy) { x = (double)copy.x; y = (double)copy.y; z = (double)copy.z; }

	template<>
	inline Vec3<double>::Vec3(const Vec3<double> &copy) { x = (double)copy.x; y = (double)copy.y; z = (double)copy.z; }

	template<>
	inline Vec3<double>::Vec3(const Vec3<int> &copy) { x = (double)copy.x; y = (double)copy.y; z = (double)copy.z; }

	template<typename Type>
	inline Type Vec3<Type>::length() const { return (Type)floor(sqrt(float(x*x + y*y + z*z)) + 0.5f); }

	template<>
	inline double Vec3<double>::length() const { return sqrt(x*x + y*y + z*z); }

	template<>
	inline float Vec3<float>::length() const { return sqrt(x*x + y*y + z*z); }

	template<typename Type>
	inline Vec3<Type> &Vec3<Type>::normalize() { Type f = length(); if (f != 0) { x /= f; y /= f; z /= f; } return *this; }

	template<typename Type>
	inline Vec3<Type> Vec3<Type>::normalize(const Vec3<Type>& vector) { Vec3<Type> dest(vector); dest.normalize(); return dest; }

	typedef Vec3<unsigned char> Vec3ub;
	typedef Vec3<char> Vec3b;
	typedef Vec3<unsigned short> Vec3us;
	typedef Vec3<short> Vec3s;
	typedef Vec3<unsigned int> Vec3ui;
	typedef Vec3<int> Vec3i;
	typedef Vec3<float> Vec3f;
	typedef Vec3<double> Vec3d;

	/// \}
}
