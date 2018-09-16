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
**    Harry Storbacka
**    Mark Page
**    Kenneth Gangstoe
**
*/

#pragma once

#include <memory>
#include "../../Core/Math/vec3.h"
#include "../2D/color.h"
#include "vertex_array_vector.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class GraphicContext;
	class Colorf;
	class Colorf;
	class GraphicContext;
	class PrimitivesArray_Impl;
	class PrimitivesArrayProvider;

	#define cl_offsetof(s,m) (&(((s *)0)->m))

	/// \brief Primitives array description.
	enum VertexAttributeDataType
	{
		type_unsigned_byte,
		type_unsigned_short,
		type_unsigned_int,
		type_byte,
		type_short,
		type_int,
		type_float
	};

	/// \brief Primitives array description.
	class PrimitivesArray
	{
	public:
		/// \brief Construct a null instance
		PrimitivesArray();

		/// \brief Constructs a PrimitivesArray
		///
		/// \param gc = Graphic Context
		PrimitivesArray(GraphicContext &gc);

		~PrimitivesArray();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the target provider for the primitives array.
		PrimitivesArrayProvider *get_provider() const;

		/// \brief Set attributes
		///
		/// \param index = value
		/// \param buffer = Vertex Array Buffer
		/// \param size = value
		/// \param type = Vertex Attribute Data Type
		/// \param offset = void
		/// \param stride = value
		/// \param normalize = bool
		void set_attributes(int index, VertexArrayBuffer &buffer, int size, VertexAttributeDataType type, size_t offset = 0, int stride = 0, bool normalize = false);

		void set_attributes(int index, VertexArrayVector<unsigned char> &buffer, int size, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, size, type_unsigned_byte, 0, 0, normalize);
		}

		void set_attributes(int index, VertexArrayVector<char> &buffer, int size, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, size, type_byte, 0, 0, normalize);
		}

		void set_attributes(int index, VertexArrayVector<unsigned short> &buffer, int size, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, size, type_unsigned_short, 0, 0, normalize);
		}

		void set_attributes(int index, VertexArrayVector<short> &buffer, int size, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, size, type_short, 0, 0, normalize);
		}

		void set_attributes(int index, VertexArrayVector<unsigned int> &buffer, int size, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, size, type_unsigned_int, 0, 0, normalize);
		}

		void set_attributes(int index, VertexArrayVector<int> &buffer, int size, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, size, type_int, 0, 0, normalize);
		}

		void set_attributes(int index, VertexArrayVector<float> &buffer, int size, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, size, type_float, 0, 0, normalize);
		}

		void set_attributes(int index, VertexArrayVector<unsigned char> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_unsigned_byte, 0, sizeof(unsigned char), normalize);
		}

		void set_attributes(int index, VertexArrayVector<signed char> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_byte, 0, sizeof(signed char), normalize);
		}

		void set_attributes(int index, VertexArrayVector<unsigned short> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_unsigned_short, 0, sizeof(unsigned short), normalize);
		}

		void set_attributes(int index, VertexArrayVector<signed short> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_short, 0, sizeof(signed short), normalize);
		}

		void set_attributes(int index, VertexArrayVector<unsigned int> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_unsigned_int, 0, sizeof(unsigned int), normalize);
		}

		void set_attributes(int index, VertexArrayVector<signed int> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_int, 0, sizeof(signed int), normalize);
		}

		void set_attributes(int index, VertexArrayVector<float> &buffer)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_float, 0, sizeof(float), false);
		}

		void set_attributes(int index, VertexArrayVector<Vec2ub> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_unsigned_byte, 0, sizeof(Vec2ub), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec2b> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_byte, 0, sizeof(Vec2b), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec2us> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_unsigned_short, 0, sizeof(Vec2us), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec2s> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_short, 0, sizeof(Vec2s), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec2ui> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_unsigned_int, 0, sizeof(Vec2ui), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec2i> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_int, 0, sizeof(Vec2i), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec2f> &buffer)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_float, 0, sizeof(Vec2f), false);
		}

		void set_attributes(int index, VertexArrayVector<Vec3ub> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_unsigned_byte, 0, sizeof(Vec3ub), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec3b> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_byte, 0, sizeof(Vec3b), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec3us> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_unsigned_short, 0, sizeof(Vec3us), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec3s> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_short, 0, sizeof(Vec3s), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec3ui> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_unsigned_int, 0, sizeof(Vec3ui), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec3i> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_int, 0, sizeof(Vec3i), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec3f> &buffer)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_float, 0, sizeof(Vec3f), false);
		}

		void set_attributes(int index, VertexArrayVector<Vec4ub> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_unsigned_byte, 0, sizeof(Vec4ub), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec4b> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_byte, 0, sizeof(Vec4b), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec4us> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_unsigned_short, 0, sizeof(Vec4us), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec4s> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_short, 0, sizeof(Vec4s), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec4ui> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_unsigned_int, 0, sizeof(Vec4ui), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec4i> &buffer, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_int, 0, sizeof(Vec4i), normalize);
		}

		void set_attributes(int index, VertexArrayVector<Vec4f> &buffer)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_float, 0, sizeof(Vec4f), false);
		}

		void set_attributes(int index, VertexArrayVector<Colorf> &buffer)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_float, 0, sizeof(Colorf), false);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, unsigned char *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_unsigned_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, signed char *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, unsigned short *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_unsigned_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, signed short *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, unsigned int *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_unsigned_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, signed int *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, float *member_offset)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 1, type_float, (size_t)member_offset, sizeof(Type), false);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec2ub *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_unsigned_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec2b *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec2us *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_unsigned_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec2s *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec2ui *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_unsigned_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec2i *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec2f *member_offset)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 2, type_float, (size_t)member_offset, sizeof(Type), false);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec3ub *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_unsigned_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec3b *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec3us *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_unsigned_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec3s *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec3ui *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_unsigned_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec3i *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec3f *member_offset)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 3, type_float, (size_t)member_offset, sizeof(Type), false);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec4ub *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_unsigned_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec4b *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_byte, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec4us *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_unsigned_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec4s *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_short, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec4ui *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_unsigned_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec4i *member_offset, bool normalize = false)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_int, (size_t)member_offset, sizeof(Type), normalize);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Vec4f *member_offset)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_float, (size_t)member_offset, sizeof(Type), false);
		}

		template<typename Type>
		void set_attributes(int index, VertexArrayVector<Type> &buffer, Colorf *member_offset)
		{
			set_attributes(index, (VertexArrayBuffer&)buffer, 4, type_float, (size_t)member_offset, sizeof(Type), false);
		}

	private:
		std::shared_ptr<PrimitivesArray_Impl> impl;
	};

	/// \}
}
