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
#include "vec3.h"

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

	/// \brief 4D vector
	///
	/// These vector templates are defined for:\n
	/// char (Vec4c), unsigned char (Vec4uc), short (Vec4s),\n
	/// unsigned short (Vec4us), int (Vec4i), unsigned int (Vec4ui), float (Vec4f), double (Vec4d)
	template<typename Type>
	class Vec4
	{
	public:
		typedef Type datatype;

		union { Type x; Type s; Type r; };
		union { Type y; Type t; Type g; };
		union { Type z; Type u; Type b; };
		union { Type w; Type v; Type a; };

		Vec4() : x(0), y(0), z(0), w(0) { }
		explicit Vec4(const Type &scalar) : x(scalar), y(scalar), z(scalar), w(scalar) { }
		explicit Vec4(const Vec2<Type> &copy, const Type &p3, const Type &p4) { x = copy.x; y = copy.y; z = p3; w = p4; }
		explicit Vec4(const Vec2<Type> &copy, const Vec2<Type> &copy34) { x = copy.x; y = copy.y; z = copy34.x; w = copy34.y; }
		explicit Vec4(const Vec3<Type> &copy, const Type &p4) { x = copy.x; y = copy.y; z = copy.z; w = p4; }
		explicit Vec4(const Type &p1, const Type &p2, const Type &p3, const Type &p4) : x(p1), y(p2), z(p3), w(p4) { }
		explicit Vec4(const Type &p1, const Type &p2, const Vec2<Type> &copy34) : x(p1), y(p2), z(copy34.x), w(copy34.y) { }
		explicit Vec4(const Type *array_xyzw) : x(array_xyzw[0]), y(array_xyzw[1]), z(array_xyzw[2]), w(array_xyzw[3]) { }

		/// \brief Normalizes a vector (not taking into account the w ordinate)
		///
		/// Operates in the native datatype
		/// \param vector = The vector to use
		/// \return the normalized vector
		static Vec4<Type> normalize3(const Vec4<Type> &vector);

		/// \brief Normalizes a vector (taking into account the w ordinate)
		///
		/// Operates in the native datatype
		/// \param vector = The vector to use
		/// \return the normalized vector
		static Vec4<Type> normalize4(const Vec4<Type> &vector);

		/// \brief Dot products between two vectors (not taking into account the w ordinate).
		///
		/// Operates in the native datatype
		///
		/// \param vector1 First vector used for the dot product.
		/// \param vector2 Second vector used for the dot product.
		/// \return The resulting dot product of the two vectors.
		static Type dot3(const Vec4<Type>& vector1, const Vec4<Type>& vector2) { return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z; }

		/// \brief Dot products between two vectors (taking into account the w ordinate).
		///
		/// Operates in the native datatype
		///
		/// \param vector1 First vector used for the dot product.
		/// \param vector2 Second vector used for the dot product.
		/// \return The resulting dot product of the two vectors.
		static Type dot4(const Vec4<Type>& vector1, const Vec4<Type>& vector2) { return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z + vector1.w*vector2.w; }

		/// \brief Calculate the cross product between two vectors (not taking into account the w ordinate).
		///
		/// \vector1 = The first vector
		/// \vector2 = The second vector
		/// \return = Cross product
		static Vec4<Type> cross3(const Vec4<Type>& vector1, const Vec4<Type>& vector2);

		/// \brief Rotate a vector around an axis. Same as glRotate[f|d](angle, a);
		///
		/// Not taking into account the w ordinate\n
		///  The w ordinate is not modified
		///
		/// \param vector = The vector to use
		/// \param angle Angle to rotate
		/// \param axis Rotation axis.
		/// \return The rotated vector
		static Vec4<Type> rotate3(const Vec4<Type>& vector, const Angle &angle, const Vec4<Type>& axis);

		/// \brief Rounds all components on a vector.
		///
		/// Includes the w ordinate\n
		/// Uses Asymmetric Arithmetic Rounding
		/// \param vector = The vector to use
		/// \return The rounded vector
		static Vec4<Type> round(const Vec4<Type>& vector);

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param first = Value A
		/// \param second = Value B
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		static bool is_equal(const Vec4<Type> &first, const Vec4<Type> &second, Type epsilon)
		{
			Type diff_x = second.x - first.x; Type diff_y = second.y - first.y; Type diff_z = second.z - first.z; Type diff_w = second.w - first.w;
			return (diff_x >= -epsilon && diff_x <= epsilon && diff_y >= -epsilon && diff_y <= epsilon && diff_z >= -epsilon && diff_z <= epsilon && diff_w >= -epsilon && diff_w <= epsilon);
		}

		void set_xy(const Vec2<Type> &new_v) { x = new_v.x; y = new_v.y; }
		void set_zw(const Vec2<Type> &new_v) { z = new_v.x; w = new_v.y; }

		/// \brief Returns the length (magnitude) of this vector (not taking into account the w ordinate).
		///
		/// Operates in the native datatype
		///
		/// \return the length of the vector */
		Type length3() const;

		/// \brief Returns the length (magnitude) of this vector (taking into account the w ordinate).
		///
		/// Operates in the native datatype
		///
		/// \return the length of the vector */
		Type length4() const;

		/// \brief Normalizes this vector (not taking into account the w ordinate)
		///
		/// Operates in the native datatype
		/// \return reference to this object
		Vec4<Type> &normalize3();

		/// \brief Normalizes this vector (taking into account the w ordinate)
		///
		/// Operates in the native datatype
		/// \return reference to this object
		Vec4<Type> &normalize4();

		/// \brief Dot products this vector with an other vector (not taking into account the w ordinate).
		///
		/// Operates in the native datatype
		///
		/// \param vector Second vector used for the dot product.
		/// \return The resulting dot product of the two vectors.
		Type dot3(const Vec4<Type>& vector) const { return x*vector.x + y*vector.y + z*vector.z; }

		/// \brief Dot products this vector with an other vector (taking into account the w ordinate).
		///
		/// Operates in the native datatype
		///
		/// \param vector Second vector used for the dot product.
		/// \return The resulting dot product of the two vectors.
		Type dot4(const Vec4<Type>& vector) const { return x*vector.x + y*vector.y + z*vector.z + w*vector.w; }

		/// \brief Calculate the angle between this vector and an other vector (not taking into account the w ordinate).
		///
		/// \param vector = Second vector used to calculate angle.
		///
		/// \return The angle between the two vectors.
		Angle angle3(const Vec4<Type>& vector) const;

		/// \brief Calculate the distance between this vector and an other vector (not taking into account the w ordinate).
		///
		/// \param vector = Second vector used to calculate distance.
		///
		/// \return The distance between the two vectors.
		Type distance3(const Vec4<Type>& vector) const;

		/// \brief Calculate the distance between this vector and an other vector (taking into account the w ordinate).
		///
		/// \param vector = Second vector used to calculate distance.
		///
		/// \return The distance between the two vectors.
		Type distance4(const Vec4<Type>& vector) const;

		/// \brief Calculate the cross product between this vector and an other vector (not taking into account the w ordinate).
		///
		/// Operates in the native datatype
		///
		/// \param vector Second vector used to perform the calculation.
		/// \return reference to this object
		Vec4<Type> &cross3(const Vec4<Type>& vector);

		/// \brief Rotate this vector around an axis. Same as glRotate[f|d](angle, a);
		///
		/// Not taking into account the w ordinate\n
		///  The w ordinate is not modified
		///
		/// \param angle Angle to rotate
		/// \param axis Rotation axis.
		/// \return reference to this object
		Vec4<Type> &rotate3(const Angle &angle, const Vec4<Type>& axis);

		/// \brief Rounds all components on this vector.
		///
		/// Includes the w ordinate\n
		/// Uses Asymmetric Arithmetic Rounding
		/// \return reference to this object
		Vec4<Type> &round();

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param other = Other value
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		bool is_equal(const Vec4<Type> &other, Type epsilon) const { return Vec4<Type>::is_equal(*this, other, epsilon); }

		/// \brief += operator.
		void operator += (const Vec4<Type>& vector) { x += vector.x; y += vector.y; z += vector.z; w += vector.w; }

		/// \brief += operator.
		void operator += (Type value) { x += value; y += value; z += value; w += value; }

		/// \brief -= operator.
		void operator -= (const Vec4<Type>& vector) { x -= vector.x; y -= vector.y; z -= vector.z; w -= vector.w; }

		/// \brief -= operator.
		void operator -= (Type value) { x -= value; y -= value; z -= value; w -= value; }

		/// \brief - operator.
		Vec4<Type> operator - () const { return Vec4<Type>(-x, -y, -z, -w); }

		/// \brief *= operator.
		void operator *= (const Vec4<Type>& vector) { x *= vector.x; y *= vector.y; z *= vector.z; w *= vector.w; }

		/// \brief *= operator.
		void operator *= (Type value) { x *= value; y *= value; z *= value; w *= value; }

		/// \brief /= operator.
		void operator /= (const Vec4<Type>& vector) { x /= vector.x; y /= vector.y; z /= vector.z; w /= vector.w; }

		/// \brief /= operator.
		void operator /= (Type value) { x /= value; y /= value; z /= value; w /= value; }

		/// \brief = operator.
		Vec4<Type> &operator = (const Vec4<Type>& vector) { x = vector.x; y = vector.y; z = vector.z; w = vector.w; return *this; }

		/// \brief == operator.
		bool operator == (const Vec4<Type>& vector) const { return ((x == vector.x) && (y == vector.y) && (z == vector.z) && (w == vector.w)); }

		/// \brief != operator.
		bool operator != (const Vec4<Type>& vector) const { return ((x != vector.x) || (y != vector.y) || (z != vector.z) || (w != vector.w)); }

		/// \brief < operator.
		bool operator < (const Vec4<Type>& vector) const { return w < vector.w || (w == vector.w && (z < vector.z || (z == vector.z && (y < vector.y || (y == vector.y && x < vector.x))))); }
	};

	/// \brief + operator.
	template<typename Type>
	Vec4<Type> operator + (const Vec4<Type>& v1, const Vec4<Type>& v2) { return Vec4<Type>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }

	/// \brief + operator.
	template<typename Type>
	Vec4<Type> operator + (Type s, const Vec4<Type>& v) { return Vec4<Type>(s + v.x, s + v.y, s + v.z, s + v.w); }

	/// \brief + operator.
	template<typename Type>
	Vec4<Type> operator + (const Vec4<Type>& v, Type s) { return Vec4<Type>(v.x + s, v.y + s, v.z + s, v.w + s); }

	/// \brief - operator.
	template<typename Type>
	Vec4<Type> operator - (const Vec4<Type>& v1, const Vec4<Type>& v2) { return Vec4<Type>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w); }

	/// \brief - operator.
	template<typename Type>
	Vec4<Type> operator - (Type s, const Vec4<Type>& v) { return Vec4<Type>(s - v.x, s - v.y, s - v.z, s - v.w); }

	/// \brief - operator.
	template<typename Type>
	Vec4<Type> operator - (const Vec4<Type>& v, Type s) { return Vec4<Type>(v.x - s, v.y - s, v.z - s, v.w - s); }

	/// \brief * operator.
	template<typename Type>
	Vec4<Type> operator * (const Vec4<Type>& v1, const Vec4<Type>& v2) { return Vec4<Type>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w); }

	/// \brief * operator.
	template<typename Type>
	Vec4<Type> operator * (Type s, const Vec4<Type>& v) { return Vec4<Type>(s * v.x, s * v.y, s * v.z, s * v.w); }

	/// \brief * operator.
	template<typename Type>
	Vec4<Type> operator * (const Vec4<Type>& v, Type s) { return Vec4<Type>(v.x * s, v.y * s, v.z * s, v.w * s); }

	/// \brief / operator.
	template<typename Type>
	Vec4<Type> operator / (const Vec4<Type>& v1, const Vec4<Type>& v2) { return Vec4<Type>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w); }

	/// \brief / operator.
	template<typename Type>
	Vec4<Type> operator / (Type s, const Vec4<Type>& v) { return Vec4<Type>(s / v.x, s / v.y, s / v.z, s / v.w); }

	/// \brief / operator.
	template<typename Type>
	Vec4<Type> operator / (const Vec4<Type>& v, Type s) { return Vec4<Type>(v.x / s, v.y / s, v.z / s, v.w / s); }

	template<typename Type>
	Vec4<Type> operator * (const Vec4<Type>& v, const Mat4<Type>& matrix)
	{
		return Vec4<Type>(
			matrix[0 * 4 + 0] * v.x + matrix[0 * 4 + 1] * v.y + matrix[0 * 4 + 2] * v.z + matrix[0 * 4 + 3] * v.w,
			matrix[1 * 4 + 0] * v.x + matrix[1 * 4 + 1] * v.y + matrix[1 * 4 + 2] * v.z + matrix[1 * 4 + 3] * v.w,
			matrix[2 * 4 + 0] * v.x + matrix[2 * 4 + 1] * v.y + matrix[2 * 4 + 2] * v.z + matrix[2 * 4 + 3] * v.w,
			matrix[3 * 4 + 0] * v.x + matrix[3 * 4 + 1] * v.y + matrix[3 * 4 + 2] * v.z + matrix[3 * 4 + 3] * v.w);
	}

	template<typename Type>
	Vec4<Type> operator * (const Mat4<Type>& matrix, const Vec4<Type>& v)
	{
		return Vec4<Type>(
			matrix[0 * 4 + 0] * v.x + matrix[1 * 4 + 0] * v.y + matrix[2 * 4 + 0] * v.z + matrix[3 * 4 + 0] * v.w,
			matrix[0 * 4 + 1] * v.x + matrix[1 * 4 + 1] * v.y + matrix[2 * 4 + 1] * v.z + matrix[3 * 4 + 1] * v.w,
			matrix[0 * 4 + 2] * v.x + matrix[1 * 4 + 2] * v.y + matrix[2 * 4 + 2] * v.z + matrix[3 * 4 + 2] * v.w,
			matrix[0 * 4 + 3] * v.x + matrix[1 * 4 + 3] * v.y + matrix[2 * 4 + 3] * v.z + matrix[3 * 4 + 3] * v.w);
	}

	template<typename Type>
	inline Type Vec4<Type>::length3() const { return (Type)floor(sqrt(float(x*x + y*y + z*z)) + 0.5f); }

	template<>
	inline double Vec4<double>::length3() const { return sqrt(x*x + y*y + z*z); }

	template<>
	inline float Vec4<float>::length3() const { return sqrt(x*x + y*y + z*z); }

	template<typename Type>
	inline Type Vec4<Type>::length4() const { return (Type)floor(sqrt(float(x*x + y*y + z*z + w*w)) + 0.5f); }

	template<>
	inline double Vec4<double>::length4() const { return sqrt(x*x + y*y + z*z + w*w); }

	template<>
	inline float Vec4<float>::length4() const { return sqrt(x*x + y*y + z*z + w*w); }

	typedef Vec4<unsigned char> Vec4ub;
	typedef Vec4<char> Vec4b;
	typedef Vec4<unsigned short> Vec4us;
	typedef Vec4<short> Vec4s;
	typedef Vec4<unsigned int> Vec4ui;
	typedef Vec4<int> Vec4i;
	typedef Vec4<float> Vec4f;
	typedef Vec4<double> Vec4d;

	/// \}
}
