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

#include "../System/cl_platform.h"
#include "mat2.h"
#include "mat3.h"
#include "vec3.h"
#include "angle.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	enum Handedness
	{
		handed_left,
		handed_right
	};

	enum ClipZRange
	{
		clip_negative_positive_w, // OpenGL, -wclip <= zclip <= wclip
		clip_zero_positive_w      // Direct3D, 0 <= zclip <= wclip
	};

	template<typename Type>
	class Mat2;

	template<typename Type>
	class Mat3;

	template<typename Type>
	class Mat4;

	template<typename Type>
	class Vec3;

	template<typename Type>
	class Quaternionx;

	class Angle;

	/// \brief 4D matrix
	///
	/// These matrix templates are defined for: int (Mat4i), float (Mat4f), double (Mat4d)
	template<typename Type>
	class Mat4
	{
	public:
		/// \brief Constructs a 4x4 matrix (zero'ed)
		Mat4()
		{
			for (auto & elem : matrix)
				elem = 0;
		}

		/// \brief Constructs a 4x4 matrix (copied)
		Mat4(const Mat4<Type> &copy)
		{
			for (int i = 0; i < 16; i++)
				matrix[i] = copy.matrix[i];
		}

		/// \brief Constructs a 4x4 matrix (copied from a 2d matrix)
		explicit Mat4(const Mat2<Type> &copy);

		/// \brief Constructs a 4x4 matrix (copied from a 3d matrix)
		explicit Mat4(const Mat3<Type> &copy);

		/// \brief Constructs a 4x4 matrix (copied from a array of floats)
		explicit Mat4(const float *init_matrix)
		{
			for (int i = 0; i < 16; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 4x4 matrix (copied from a array of doubles)
		explicit Mat4(const double *init_matrix)
		{
			for (int i = 0; i < 16; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 4x4 matrix (copied from a array of 64 bit integers)
		explicit Mat4(const int64_t *init_matrix)
		{
			for (int i = 0; i < 16; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 4x4 matrix (copied from a array of 32 bit integers)
		explicit Mat4(const int32_t *init_matrix)
		{
			for (int i = 0; i < 16; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 4x4 matrix (copied from a array of 16 bit integers)
		explicit Mat4(const int16_t *init_matrix)
		{
			for (int i = 0; i < 16; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Constructs a 4x4 matrix (copied from a array of 8 bit integers)
		explicit Mat4(const int8_t *init_matrix)
		{
			for (int i = 0; i < 16; i++)
				matrix[i] = (Type)init_matrix[i];
		}

		/// \brief Create a zero matrix
		///
		/// \return The matrix
		static Mat4<Type> null();

		/// \brief Create the identity matrix
		/// \return The matrix
		static Mat4<Type> identity();

		/// \brief Create a frustum matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \return The matrix (in column-major format)
		static Mat4<Type> frustum(Type left, Type right, Type bottom, Type top, Type z_near, Type z_far, Handedness handedness, ClipZRange clip_z);

		/// \brief Create a perspective matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \return The matrix (in column-major format)
		static Mat4<Type> perspective(
			Type field_of_view_y_degrees,
			Type aspect,
			Type z_near,
			Type z_far,
			Handedness handedness,
			ClipZRange clip_z);

		/// \brief Create a ortho matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \return The matrix (in column-major format)
		static Mat4<Type> ortho(Type left, Type right, Type bottom, Type top, Type z_near, Type z_far, Handedness handedness, ClipZRange clip_z);

		/// \brief Create a ortho_2d matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \return The matrix (in column-major format)
		static Mat4<Type> ortho_2d(Type left, Type right, Type bottom, Type top, Handedness handedness, ClipZRange clip_z);

		/// \brief Create a rotation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param angle = Angle to rotate by
		/// \param x = Amount to rotate in the X axis
		/// \param y = Amount to rotate in the Y axis
		/// \param z = Amount to rotate in the Z axis
		/// \param normalize = true = Normalize x,y,z before creating rotation matrix
		/// \return The matrix (in column-major format)
		static Mat4<Type> rotate(const Angle &angle, Type x, Type y, Type z, bool normalize = true);

		/// \brief Create a rotation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param angle = Angle to rotate by
		/// \param rotate = Amount to rotate in the XYZ axes
		/// \param normalize = true = Normalize x,y,z before creating rotation matrix
		/// \return The matrix (in column-major format)
		static Mat4<Type> rotate(const Angle &angle, Vec3<Type> rotation, bool normalize = true)
		{
			return rotate(angle, rotation.x, rotation.y, rotation.z, normalize);
		}

		/// \brief Create a rotation matrix using euler angles
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		///
		/// \return The matrix (in column-major format)
		static Mat4<Type> rotate(const Angle &angle_x, const Angle &angle_y, const Angle &angle_z, EulerOrder order);

		/// \brief Create a scale matrix
		///
		/// \param x = Scale X
		/// \param y = Scale Y
		/// \param z = Scale Z
		/// \return The matrix
		static Mat4<Type> scale(Type x, Type y, Type z);

		/// \brief Create a scale matrix
		///
		/// \param xyz = Scale XYZ
		/// \return The matrix
		static Mat4<Type> scale(const Vec3<Type> &xyz)
		{
			return scale(xyz.x, xyz.y, xyz.z);
		}

		/// \brief Create a translation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param x = Translate X
		/// \param y = Translate Y
		/// \param z = Translate Z
		/// \return The matrix (in column-major format)
		static Mat4<Type> translate(Type x, Type y, Type z);

		/// \brief Create a translation matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param xyz = Translate XYZ
		/// \return The matrix (in column-major format)
		static Mat4<Type> translate(const Vec3<Type> &xyz)
		{
			return translate(xyz.x, xyz.y, xyz.z);
		}

		/// \brief Create the "look at" matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param eye_x = Eye position X
		/// \param eye_y = Eye position Y
		/// \param eye_z = Eye position Z
		/// \param center_x = Center X
		/// \param center_y = Center Y
		/// \param center_z = Center Z
		/// \param up_x = Camera up direction X
		/// \param up_y = Camera up direction Y
		/// \param up_z = Camera up direction Z
		/// \return The matrix (in column-major format)
		static Mat4<Type> look_at(
			Type eye_x, Type eye_y, Type eye_z,
			Type center_x, Type center_y, Type center_z,
			Type up_x, Type up_y, Type up_z);

		/// \brief Create the "look at" matrix
		///
		/// Matrix is created in the Column-Major matrix format (opengl native)
		/// \param eye = Eye position
		/// \param center = Center position
		/// \param up = Camera up direction
		/// \return The matrix (in column-major format)
		static Mat4<Type> look_at(
			Vec3<Type> eye,
			Vec3<Type> center,
			Vec3<Type> up)
		{
			return look_at(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
		}

		/// \brief Multiply 2 matrices
		///
		/// This multiplies the matrix as follows: result = matrix1 * matrix2\n
		/// Matrix is multiplied in the Column-Major matrix format (opengl native)
		///
		/// \param matrix_1 = First Matrix to multiply
		/// \param matrix_2 = Second Matrix to multiply
		/// \return The matrix
		static Mat4<Type> multiply(const Mat4<Type> &matrix_1, const Mat4<Type> &matrix_2);

		/// \brief Add 2 matrices
		///
		/// This adds the matrix as follows: result = matrix1 + matrix2
		///
		/// \param matrix_1 = First Matrix to add
		/// \param matrix_2 = Second Matrix to add
		/// \return The matrix
		static Mat4<Type> add(const Mat4<Type> &matrix_1, const Mat4<Type> &matrix_2);

		/// \brief Subtract 2 matrices
		///
		/// This subtracts the matrix as follows: result = matrix1 - matrix2
		///
		/// \param matrix_1 = First Matrix to subtract
		/// \param matrix_2 = Second Matrix to subtract
		/// \return The matrix
		static Mat4<Type> subtract(const Mat4<Type> &matrix_1, const Mat4<Type> &matrix_2);

		/// \brief Calculate the adjoint (or known as Adjugate or Conjugate Transpose) of a matrix
		///
		/// \param matrix = The matrix to use
		/// \return The adjoint matrix
		static Mat4<Type> adjoint(const Mat4<Type> &matrix);

		/// \brief Calculate the matrix inverse of a matrix
		///
		/// Creates a zero matrix if the determinent == 0
		/// \param matrix = The matrix to use
		/// \return The inversed matrix
		static Mat4<Type> inverse(const Mat4<Type> &matrix);

		/// \brief Calculate the transpose of a matrix 
		///
		/// \param matrix = The matrix to use
		/// \return The transposed matrix
		static Mat4<Type> transpose(const Mat4<Type> &matrix);

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param first = Value A
		/// \param second = Value B
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		static bool is_equal(const Mat4<Type> &first, const Mat4<Type> &second, Type epsilon)
		{
			for (int i = 0; i < 16; i++)
			{
				Type diff = second.matrix[i] - first.matrix[i];
				if (diff < -epsilon || diff > epsilon) return false;
			}
			return true;
		}

		/// The matrix (in column-major format)
		Type matrix[16];

		/// \brief Returns the translation coordinates for this matrix (in column-major format)
		Vec3<Type> get_translate() const { return Vec3<Type>(matrix[12], matrix[13], matrix[14]); }

		/// \brief Extract the euler angles (in radians) from a matrix (in column-major format)
		///
		/// \return The x,y,z angles (in radians)
		Vec3<Type> get_euler(EulerOrder order) const;

		/// \brief Get a transformed point from the matrix (in column-major format)
		///
		/// \return The transformed point
		Vec3<Type> get_transformed_point(const Vec3<Type> &vector) const;

		/// \brief Scale this matrix
		///
		/// This is faster than using: multiply(Mat4<Type>::scale(x,y,z) )
		///
		/// \param x = Scale X
		/// \param y = Scale Y
		/// \param z = Scale Z
		///
		/// \return reference to this object
		Mat4<Type> &scale_self(Type x, Type y, Type z);

		/// \brief Scale this matrix
		///
		/// This is faster than using: multiply(Mat4<Type>::scale(x,y,z) )
		///
		/// \param scale = Scale XYZ
		///
		/// \return reference to this object
		Mat4<Type> &scale_self(const Vec3<Type> &scale) { return scale_self(scale.x, scale.y, scale.z); }

		/// \brief Translate this matrix
		///
		/// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
		/// This is faster than using: multiply(Mat4<Type>::translate(x,y,z) )
		///
		/// \param x = Translate X
		/// \param y = Translate Y
		/// \param z = Translate Z
		///
		/// \return reference to this object
		Mat4<Type> &translate_self(Type x, Type y, Type z);

		/// \brief Translate this matrix
		///
		/// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
		/// This is faster than using: multiply(Mat4<Type>::translate(x,y,z) )
		///
		/// \param translation = Translate XYZ
		///
		/// \return reference to this object
		Mat4<Type> &translate_self(const Vec3<Type> &translation) { return translate_self(translation.x, translation.y, translation.z); }

		/// \brief Set this matrix translation values
		///
		/// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
		/// This does not translate the matrix, see translate_self() if this is desired
		///
		/// \param x = Translate X
		/// \param y = Translate Y
		/// \param z = Translate Z
		///
		/// \return reference to this object
		Mat4<Type> &set_translate(Type x, Type y, Type z) { matrix[3 * 4 + 0] = x; matrix[3 * 4 + 1] = y; matrix[3 * 4 + 2] = z; return *this; }

		/// \brief Set this matrix translation values
		///
		/// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
		/// This does not translate the matrix, see translate_self() if this is desired
		///
		/// \param translation = Translate XYZ
		///
		/// \return reference to this object
		Mat4<Type> &set_translate(const Vec3<Type> &translation) { matrix[3 * 4 + 0] = translation.x; matrix[3 * 4 + 1] = translation.y; matrix[3 * 4 + 2] = translation.z; return *this; }

		/// \brief Calculate the matrix determinant of this matrix
		///
		/// \return The determinant
		double det() const;

		/// \brief Calculate the adjoint (or known as adjugate) of this matrix
		///
		/// \return reference to this object
		Mat4<Type> &adjoint();

		/// \brief Calculate the matrix inverse of this matrix
		///
		/// Creates a zero matrix if the determinent == 0
		/// \return reference to this object
		Mat4<Type> &inverse();

		/// \brief Calculate the transpose of this matrix 
		///
		/// \return reference to this object
		Mat4<Type> &transpose();

		/// \brief Decompose matrix into position, orientation/rotation and scale
		void decompose(Vec3<Type> &out_position, Quaternionx<Type> &out_orientation, Vec3<Type> &out_scale) const;

		/// \brief Returns true if equal within the bounds of an epsilon
		///
		/// \param other = Other value
		/// \param epsilon = The epsilon (eg FLT_EPSILON/2, DBL_EPSILON/2)
		bool is_equal(const Mat4<Type> &other, Type epsilon) const { return Mat4<Type>::is_equal(*this, other, epsilon); }

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
		Mat4<Type> &operator =(const Mat4<Type> &copy) { memcpy(matrix, copy.matrix, sizeof(matrix)); return *this; }

		/// \brief Copy assignment operator.
		Mat4<Type> &operator =(const Mat3<Type> &copy);

		/// \brief Copy assignment operator.
		Mat4<Type> &operator =(const Mat2<Type> &copy);

		/// \brief Multiplication operator.
		Mat4<Type> operator *(const Mat4<Type> &mult) const;

		/// \brief Addition operator.
		Mat4<Type> operator +(const Mat4<Type> &add_matrix) const;

		/// \brief Subtraction operator.
		Mat4<Type> operator -(const Mat4<Type> &sub_matrix) const;

		/// \brief Equality operator.
		bool operator==(const Mat4<Type> &other) const
		{
			for (int i = 0; i < 16; i++)
				if (matrix[i] != other.matrix[i]) return false;
			return true;
		}

		/// \brief Not-equal operator.
		bool operator!=(const Mat4<Type> &other) { return !((*this) == other); }
	};

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::null() { Mat4<Type> m; memset(m.matrix, 0, sizeof(m.matrix));	return m; }

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::identity() { Mat4<Type> m = null(); m.matrix[0] = 1; m.matrix[5] = 1; m.matrix[10] = 1; m.matrix[15] = 1; return m; }

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::multiply(const Mat4<Type> &matrix_1, const Mat4<Type> &matrix_2) { return matrix_1 * matrix_2; }

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::add(const Mat4<Type> &matrix_1, const Mat4<Type> &matrix_2) { return matrix_1 + matrix_2; }

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::subtract(const Mat4<Type> &matrix_1, const Mat4<Type> &matrix_2) { return matrix_1 - matrix_2; }

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::adjoint(const Mat4<Type> &matrix) { Mat4<Type> dest(matrix); dest.adjoint(); return dest; }

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::inverse(const Mat4<Type> &matrix) { Mat4<Type> dest(matrix); dest.inverse(); return dest; }

	template<typename Type>
	inline Mat4<Type> Mat4<Type>::transpose(const Mat4<Type> &matrix) { Mat4<Type> dest(matrix); dest.transpose(); return dest; }

	typedef Mat4<int> Mat4i;
	typedef Mat4<float> Mat4f;
	typedef Mat4<double> Mat4d;

	/// \}
}
