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

#include "mat2.h"
#include "mat4.h"
#include "vec3.h"
#include "../System/cl_platform.h"
#include "angle.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	template<typename Type>
	class Mat2;

	template<typename Type>
	class Mat3;

	template<typename Type>
	class Mat4;

	class Angle;

	/// \brief 3D matrix
	///
	/// These matrix templates are defined for: int (Mat3i), float (Mat3f), double (Mat3d)
	template<typename Type>
	class Mat3
	{
	public:
		/// \brief Constructs a 3x3 matrix (zero'ed)
		Mat3()
		{
			for (auto & elem : matrix)
				elem = 0;
		}

		/// \brief Constructs a 3x3 matrix (copied)
		Mat3(const Mat3<Type> &copy)
		{
			for (int i = 0; i < 9; i++)
				matrix[i] = copy.matrix[i];
		}

		/// \brief Constructs a 3x3 matrix (copied from a 2d matrix)
		explicit Mat3(const Mat2<Type> &copy);

		/// \brief Constructs a 3x3 matrix (copied from a 4d matrix)
		explicit Mat3(const Mat4<Type> &copy);

		/// \brief Constructs a 3x3 matrix (copied from 9 floats)
		explicit Mat3(const float *init_matrix)
		{
			for (int i = 0; i < 9; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 3x3 matrix (copied from specified values)
		explicit Mat3(Type m00, Type m01, Type m02, Type m10, Type m11, Type m12, Type m20, Type m21, Type m22)
		{
			matrix[0 * 3 + 0] = m00; matrix[0 * 3 + 1] = m01; matrix[0 * 3 + 2] = m02;
			matrix[1 * 3 + 0] = m10; matrix[1 * 3 + 1] = m11; matrix[1 * 3 + 2] = m12;
			matrix[2 * 3 + 0] = m20; matrix[2 * 3 + 1] = m21; matrix[2 * 3 + 2] = m22;
		}

		/// \brief Constructs a 3x3 matrix (copied from 9 doubles)
		explicit Mat3(const double *init_matrix)
		{
			for (int i = 0; i < 9; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 3x3 matrix (copied from 9, 64 bit integers)
		explicit Mat3(const int64_t *init_matrix)
		{
			for (int i = 0; i < 9; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 3x3 matrix (copied from 9, 32 bit integers)
		explicit Mat3(const int32_t *init_matrix)
		{
			for (int i = 0; i < 9; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 3x3 matrix (copied from 9, 16 bit integers)
		explicit Mat3(const int16_t *init_matrix)
		{
			for (int i = 0; i < 9; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 3x3 matrix (copied from 9, 8 bit integers)
		explicit Mat3(const int8_t *init_matrix)
		{
			for (int i = 0; i < 9; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		static Mat3<Type> null();

		static Mat3<Type> identity();

		/// \brief Create a 3d rotation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param angle = Angle to rotate by
		/// \param x = Amount to rotate in the X axis
		/// \param y = Amount to rotate in the Y axis
		/// \param z = Amount to rotate in the Z axis
		/// \param normalize = true = Normalize x,y,z before creating rotation matrix
		/// \return The matrix (in column-major format)
		static Mat3<Type> rotate(const Angle &angle, Type x, Type y, Type z, bool normalize = true);

		/// \brief Create a 3d rotation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param angle = Angle to rotate by
		/// \param rotation = Amount to rotate in the XYZ axes
		/// \param normalize = true = Normalize x,y,z before creating rotation matrix
		/// \return The matrix (in column-major format)
		static Mat3<Type> rotate(const Angle &angle, Vec3<Type> rotation, bool normalize = true)
		{
			return rotate(angle, rotation.x, rotation.y, rotation.z, normalize);
		}

		/// \brief Create a 3d rotation matrix using euler angles
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		///
		/// \return The matrix (in column-major format)
		static Mat3<Type> rotate(const Angle &angle_x, const Angle &angle_y, const Angle &angle_z, EulerOrder order);

		/// \brief Create a 2d rotation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param angle = Angle to rotate by
		/// \return The matrix (in column-major format)
		static Mat3<Type> rotate(const Angle &angle);

		/// \brief Create a 2d scale matrix
		///
		/// \param x = Scale X
		/// \param y = Scale Y
		/// \return The matrix
		static Mat3<Type> scale(Type x, Type y);

		/// \brief Create a 2d scale matrix
		///
		/// \param xy = Scale XY
		/// \return The matrix
		static Mat3<Type> scale(const Vec3<Type> &xy)
		{
			return scale(xy.x, xy.y);
		}

		/// \brief Create a 2d translation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param x = Translate X
		/// \param y = Translate Y
		/// \return The matrix (in column-major format)
		static Mat3<Type> translate(Type x, Type y);

		/// \brief Create a 2d translation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param xy = Translate XY
		/// \return The matrix (in column-major format)
		static Mat3<Type> translate(const Vec2<Type> &xy)
		{
			return translate(xy.x, xy.y);
		}

		/// \brief Multiply 2 matrices
		///
		/// This multiplies the matrix as follows: result = matrix1 * matrix2\n
		/// Matrix is multiplied in the Column-Major matrix format (opengl native)
		///
		/// \param matrix_1 = First Matrix to multiply
		/// \param matrix_2 = Second Matrix to multiply
		/// \return The matrix
		static Mat3<Type> multiply(const Mat3<Type> &matrix_1, const Mat3<Type> &matrix_2);

		/// \brief Add 2 matrices
		///
		/// This adds the matrix as follows: result = matrix1 + matrix2
		///
		/// \param matrix_1 = First Matrix to add
		/// \param matrix_2 = Second Matrix to add
		/// \return The matrix
		static Mat3<Type> add(const Mat3<Type> &matrix_1, const Mat3<Type> &matrix_2);

		/// \brief Subtract 2 matrices
		///
		/// This subtracts the matrix as follows: result = matrix1 - matrix2
		///
		/// \param matrix_1 = First Matrix to subtract
		/// \param matrix_2 = Second Matrix to subtract
		/// \return The matrix
		static Mat3<Type> subtract(const Mat3<Type> &matrix_1, const Mat3<Type> &matrix_2);

		/// \brief Calculate the adjoint (or known as Adjugate or Conjugate Transpose) of a matrix
		///
		/// \param matrix = The matrix to use
		/// \return The adjoint matrix
		static Mat3<Type> adjoint(const Mat3<Type> &matrix);

		/// \brief Calculate the matrix inverse of a matrix
		///
		/// Creates a zero matrix if the determinent == 0
		/// \param matrix = The matrix to use
		/// \return The inversed matrix
		static Mat3<Type> inverse(const Mat3<Type> &matrix);

		/// \brief Calculate the transpose of a matrix 
		///
		/// \param matrix = The matrix to use
		/// \return The transposed matrix
		static Mat3<Type> transpose(const Mat3<Type> &matrix);

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param first = Value A
		/// \param second = Value B
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		static bool is_equal(const Mat3<Type> &first, const Mat3<Type> &second, Type epsilon)
		{
			for (int i = 0; i < 9; i++)
			{
				Type diff = second.matrix[i] - first.matrix[i];
				if (diff < -epsilon || diff > epsilon) return false;
			}
			return true;
		}

		/// The matrix (in column-major format)
		Type matrix[9];

		/// \brief Calculate the matrix determinant
		double det() const;

		/// \brief Creates the adjoint (or known as adjugate) of the matrix
		///
		/// \return reference to this object
		Mat3<Type> &adjoint();

		/// \brief Create the matrix inverse. (Returns a zero matrix if the determinent = 0)
		///
		/// \return reference to this object
		Mat3<Type> &inverse();

		/// \brief Calculate the transpose of this matrix 
		///
		/// \return reference to this object
		Mat3<Type> &transpose();

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param other = Other value
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		bool is_equal(const Mat3<Type> &other, Type epsilon) const { return Mat3<Type>::is_equal(*this, other, epsilon); }

		/// \brief Operator that returns the matrix as a array.
		operator Type const*() const { return matrix; }

		/// \brief Operator that returns the matrix as a array.
		operator Type *() { return matrix; }

		/// \brief Operator that returns the matrix cell at the given index.
		Type &operator[](int i) { return matrix[i]; }

		/// \brief Operator that returns the matrix cell at the given index.
		const Type &operator[](int i) const { return matrix[i]; }

		/// \brief Operator that returns the matrix cell at the given index.
		Type &operator[](unsigned int i) { return matrix[i]; }

		/// \brief Operator that returns the matrix cell at the given index.
		const Type &operator[](unsigned int i) const { return matrix[i]; }

		/// \brief Copy assignment operator.
		Mat3<Type> &operator =(const Mat3<Type> &copy) { memcpy(matrix, copy.matrix, sizeof(matrix)); return *this; }

		/// \brief Copy assignment operator.
		Mat3<Type> &operator =(const Mat4<Type> &copy);

		/// \brief Copy assignment operator.
		Mat3<Type> &operator =(const Mat2<Type> &copy);

		/// \brief Multiplication operator.
		Mat3<Type> operator *(const Mat3<Type> &mult) const;

		/// \brief Addition operator.
		Mat3<Type> operator +(const Mat3<Type> &add_matrix) const;

		/// \brief Subtraction operator.
		Mat3<Type> operator -(const Mat3<Type> &sub_matrix) const;

		/// \brief Multiplication operator.
		Vec2<Type> operator *(const Vec2<Type> &mult) const;

		/// \brief Equality operator.
		bool operator==(const Mat3<Type> &other) const
		{
			for (int i = 0; i < 9; i++)
				if (matrix[i] != other.matrix[i]) return false;
			return true;
		}

		/// \brief Not-equal operator.
		bool operator!=(const Mat3<Type> &other) { return !((*this) == other); }
	};

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::multiply(const Mat3<Type> &matrix_1, const Mat3<Type> &matrix_2) { return matrix_1 * matrix_2; }

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::add(const Mat3<Type> &matrix_1, const Mat3<Type> &matrix_2) { return matrix_1 + matrix_2; }

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::subtract(const Mat3<Type> &matrix_1, const Mat3<Type> &matrix_2) { return matrix_1 - matrix_2; }

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::adjoint(const Mat3<Type> &matrix) { Mat3<Type> dest(matrix); dest.adjoint(); return dest; }

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::inverse(const Mat3<Type> &matrix) { Mat3<Type> dest(matrix); dest.inverse(); return dest; }

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::transpose(const Mat3<Type> &matrix) { Mat3<Type> dest(matrix); dest.transpose(); return dest; }

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::null() { Mat3<Type> m; memset(m.matrix, 0, sizeof(m.matrix)); return m; }

	template<typename Type>
	inline Mat3<Type> Mat3<Type>::identity() { Mat3<Type> m = null(); m.matrix[0] = 1; m.matrix[4] = 1; m.matrix[8] = 1; return m; }

	typedef Mat3<int> Mat3i;
	typedef Mat3<float> Mat3f;
	typedef Mat3<double> Mat3d;

	/// \}
}
