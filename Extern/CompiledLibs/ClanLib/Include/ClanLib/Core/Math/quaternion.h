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

#include "vec3.h"
#include "angle.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	/// \brief Quaternion
	///
	/// These quaternion templates are defined for: float (Quaternionf), double (Quaterniond)
	template<typename Type>
	class Quaternionx
	{
	public:
		/// \brief The real scalar part
		Type w;

		/// \brief The imaginary vector part
		union { Type i; Type x; };
		union { Type j; Type y; };
		union { Type k; Type z; };

		Quaternionx() : w(1), i(0), j(0), k(0) { }
		explicit Quaternionx(Type real, Type i, Type j, Type k) : w(real), i(i), j(j), k(k) { }
		explicit Quaternionx(Type real, const Vec3<Type> &imag) : w(real), i(imag.x), j(imag.y), k(imag.z) { }
		Quaternionx(const Quaternionx<Type> &copy) : w(copy.w), i(copy.i), j(copy.j), k(copy.k) { }
		explicit Quaternionx(Type euler_x, Type euler_y, Type euler_z, AngleUnit unit, EulerOrder order);
		explicit Quaternionx(const Vec3<Type> &euler, AngleUnit unit, EulerOrder order);
		explicit Quaternionx(const Angle &euler_x, const Angle &euler_y, const Angle &euler_z, EulerOrder order);
		explicit Quaternionx(const Mat4<Type> &rotation_matrix);

		static Quaternionx<Type> axis_angle(const Angle &angle, const Vec3f &axis);
		static Quaternionx<Type> multiply(const Quaternionx<Type> &quaternion_1, const Quaternionx<Type> &quaternion_2);

		/// \brief Calculates the shortest arc quaternion between two vectors
		static Quaternionx<Type> rotation_between(Vec3<Type> v0, Vec3<Type> v1);

		/// \brief Calculates the shortest arc quaternion between two vectors
		static Quaternionx<Type> rotation_between(Vec4<Type> v0, Vec4<Type> v1);

		/// \brief Linear Quaternion Interpolation
		///
		/// \param quaternion_initial = Source quaternion
		/// \param quaternion_final = Destination quaternion
		/// \param lerp_time = Time in the range of 0.0 to 1.0
		static Quaternionx<Type> lerp(const Quaternionx<Type> &quaternion_initial, const Quaternionx<Type> &quaternion_final, Type lerp_time);

		/// \brief Spherical Quaternion Interpolation
		///
		/// \param quaternion_initial = Source quaternion
		/// \param quaternion_final = Destination quaternion
		/// \param slerp_time = Time in the range of 0.0 to 1.0
		static Quaternionx<Type> slerp(const Quaternionx<Type> &quaternion_initial, const Quaternionx<Type> &quaternion_final, Type slerp_time);

		/// \brief Convert the quaternion to a rotation matrix
		///
		/// This function assumes that the quarternion is normalized.
		Mat4<Type> to_matrix() const;

		/// \brief Get the quaternion magnitude
		Type magnitude() const;

		void set(Type euler_x, Type euler_y, Type euler_z, AngleUnit unit, EulerOrder order);
		void set(const Vec3<Type> &euler, AngleUnit unit, EulerOrder order);
		void set(const Angle &euler_x, const Angle &euler_y, const Angle &euler_z, EulerOrder order);

		Quaternionx<Type> &rotate(const Angle &angle, const Vec3f &axis);

		Quaternionx<Type> &rotate(const Angle &euler_x, const Angle &euler_y, const Angle &euler_z, EulerOrder order);

		/// \brief Normalizes this quaternion
		///
		/// \return reference to this object
		Quaternionx<Type> &normalize();

		/// \brief Inverse this quaternion
		///
		/// This is the same as the conjugate of a quaternion
		///
		/// \return reference to this object
		Quaternionx<Type> &inverse();

		/// \brief Rotates vector by this quaternion.
		///
		/// \param v = Vertex to rotate
		///
		/// \return rotated vector
		Vec3<Type> rotate_vector(const Vec3<Type> &v) const;

		Vec4<Type> rotate_vector(const Vec4<Type> &v) const;

		/// \brief Normalizes this quaternion
		///
		/// \return reference to this object
		static Quaternionx<Type> normalize(Quaternionx<Type> q) { return q.normalize(); }

		/// \brief Inverse this quaternion
		///
		/// This is the same as the conjugate of a quaternion
		///
		/// \return reference to this object
		static Quaternionx<Type> inverse(Quaternionx<Type> q) { return q.inverse(); }

		/// \brief Multiplication operator.
		Quaternionx<Type> operator *(const Quaternionx<Type> &mult) const { return Quaternionx<Type>::multiply(*this, mult); }

		Quaternionx<Type> operator *(const Mat4<Type> &matrix) const;

		/// \brief Less operator.
		bool operator<(const Quaternionx<Type> &other) const
		{
			if (x != other.x) return x < other.x;
			else if (y != other.y) return y < other.y;
			else if (z != other.z) return z < other.z;
			else return w < other.w;
		}

		/// \brief Greater operator.
		bool operator>(const Quaternionx<Type> &other) const
		{
			if (x != other.x) return x > other.x;
			else if (y != other.y) return y > other.y;
			else if (z != other.z) return z > other.z;
			else return w > other.w;
		}

		/// \brief Less equal operator.
		bool operator<=(const Quaternionx<Type> &other) const { return *this < other || *this == other; }

		/// \brief Greater equal operator.
		bool operator>=(const Quaternionx<Type> &other) const { return *this > other || *this == other; }

		/// \brief Equal operator.
		bool operator==(const Quaternionx<Type> &other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }

		/// \brief Not equal operator.
		bool operator!=(const Quaternionx<Type> &other) const { return x != other.x || y != other.y || z != other.z || w == other.w; }
	};

	/// \brief Quaternion - Float
	class Quaternionf : public Quaternionx<float>
	{
	public:
		Quaternionf() : Quaternionx<float>() { }
		Quaternionf(const Quaternionx<float> &copy) : Quaternionx<float>(copy) { }
		explicit Quaternionf(const Mat4<float> &rotation_matrix) : Quaternionx<float>(rotation_matrix) { };

		explicit Quaternionf(float real, float i, float j, float k) : Quaternionx<float>(real, i, j, k) { }
		explicit Quaternionf(float real, const Vec3<float> &imag) : Quaternionx<float>(real, imag) { }
		explicit Quaternionf(float euler_x, float euler_y, float euler_z, AngleUnit unit, EulerOrder order) : Quaternionx<float>(euler_x, euler_y, euler_z, unit, order) { }
		explicit Quaternionf(const Vec3<float> &euler, AngleUnit unit, EulerOrder order) : Quaternionx<float>(euler, unit, order) { }
		explicit Quaternionf(const Angle &euler_x, const Angle &euler_y, const Angle &euler_z, EulerOrder order) : Quaternionx<float>(euler_x, euler_y, euler_z, order) { }

	};

	/// \brief Quaternion - Double
	class Quaterniond : public Quaternionx<double>
	{
	public:
		Quaterniond() : Quaternionx<double>() { }
		Quaterniond(const Quaternionx<double> &copy) : Quaternionx<double>(copy) { }
		explicit Quaterniond(const Mat4<double> &rotation_matrix) : Quaternionx<double>(rotation_matrix) { }
		explicit Quaterniond(double real, double i, double j, double k) : Quaternionx<double>(real, i, j, k) { }
		explicit Quaterniond(double real, const Vec3<double> &imag) : Quaternionx<double>(real, imag) { }
		explicit Quaterniond(double euler_x, double euler_y, double euler_z, AngleUnit unit, EulerOrder order) : Quaternionx<double>(euler_x, euler_y, euler_z, unit, order) { }
		explicit Quaterniond(const Vec3<double> &euler, AngleUnit unit, EulerOrder order) : Quaternionx<double>(euler, unit, order) { }
		explicit Quaterniond(const Angle &euler_x, const Angle &euler_y, const Angle &euler_z, EulerOrder order) : Quaternionx<double>(euler_x, euler_y, euler_z, order) { }
	};

	/// \}
}
