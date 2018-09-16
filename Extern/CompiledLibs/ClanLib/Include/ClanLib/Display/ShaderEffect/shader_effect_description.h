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

#include <memory>
#include "../Render/graphic_context.h"
#include "../Render/rasterizer_state_description.h"
#include "../Render/blend_state_description.h"
#include "../Render/depth_stencil_state_description.h"
#include "../Render/storage_buffer.h"
#include "../Render/uniform_buffer.h"
#include "../Render/texture.h"
#include "../Render/render_buffer.h"
#include "../../Core/Resources/resource.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class ShaderEffectDescription_Impl;

	/// \brief Shader Effect Description
	///
	///    <p>...</p> 
	class ShaderEffectDescription
	{
	public:
		ShaderEffectDescription();

		void set_glsl_version(int version = 330);

		void set_define(std::string name, std::string value = "1");

		void set_vertex_shader(std::string code, ShaderLanguage language = shader_glsl);
		void set_fragment_shader(std::string code, ShaderLanguage language = shader_glsl);
		void set_compute_shader(std::string code, ShaderLanguage language = shader_glsl);

		RasterizerStateDescription &rasterizer();
		BlendStateDescription &blend();
		DepthStencilStateDescription &depth_stencil();

		void set_frag_data(std::string name, RenderBuffer buffer);
		void set_frag_data(std::string name, Texture texture);
		void set_frag_data_to_back_buffer(std::string name);

		void set_depth_data(RenderBuffer buffer);
		void set_depth_data(Texture texture);

		void set_stencil_data(RenderBuffer buffer);
		void set_stencil_data(Texture texture);

		void set_texture(std::string name, Resource<Texture> texture);
		void set_image(std::string name, Resource<Texture> texture);

		void set_storage(std::string name, Resource<StorageBuffer> values);

		void set_uniform_block(std::string name, Resource<UniformBuffer> values);

		void set_attribute(std::string name, VertexArrayBuffer &buffer, int size, VertexAttributeDataType type, size_t offset = 0, int stride = 0, bool normalize = false);

		void set_attribute(std::string name, VertexArrayVector<unsigned char> &buffer, int size, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, size, type_unsigned_byte, 0, 0, normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<char> &buffer, int size, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, size, type_byte, 0, 0, normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<unsigned short> &buffer, int size, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, size, type_unsigned_short, 0, 0, normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<short> &buffer, int size, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, size, type_short, 0, 0, normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<unsigned int> &buffer, int size, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, size, type_unsigned_int, 0, 0, normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<int> &buffer, int size, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, size, type_int, 0, 0, normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<float> &buffer, int size, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, size, type_float, 0, 0, normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<unsigned char> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 1, type_unsigned_byte, 0, sizeof(unsigned char), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<signed char> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 1, type_byte, 0, sizeof(signed char), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<unsigned short> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 1, type_unsigned_short, 0, sizeof(unsigned short), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<signed short> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 1, type_short, 0, sizeof(signed short), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<unsigned int> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 1, type_unsigned_int, 0, sizeof(unsigned int), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<signed int> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 1, type_int, 0, sizeof(signed int), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<float> &buffer)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 1, type_float, 0, sizeof(float), false);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec2ub> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 2, type_unsigned_byte, 0, sizeof(Vec2ub), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec2b> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 2, type_byte, 0, sizeof(Vec2b), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec2us> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 2, type_unsigned_short, 0, sizeof(Vec2us), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec2s> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 2, type_short, 0, sizeof(Vec2s), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec2ui> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 2, type_unsigned_int, 0, sizeof(Vec2ui), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec2i> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 2, type_int, 0, sizeof(Vec2i), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec2f> &buffer)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 2, type_float, 0, sizeof(Vec2f), false);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec3ub> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 3, type_unsigned_byte, 0, sizeof(Vec3ub), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec3b> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 3, type_byte, 0, sizeof(Vec3b), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec3us> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 3, type_unsigned_short, 0, sizeof(Vec3us), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec3s> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 3, type_short, 0, sizeof(Vec3s), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec3ui> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 3, type_unsigned_int, 0, sizeof(Vec3ui), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec3i> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 3, type_int, 0, sizeof(Vec3i), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec3f> &buffer)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 3, type_float, 0, sizeof(Vec3f), false);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec4ub> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_unsigned_byte, 0, sizeof(Vec4ub), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec4b> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_byte, 0, sizeof(Vec4b), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec4us> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_unsigned_short, 0, sizeof(Vec4us), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec4s> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_short, 0, sizeof(Vec4s), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec4ui> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_unsigned_int, 0, sizeof(Vec4ui), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec4i> &buffer, bool normalize = false)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_int, 0, sizeof(Vec4i), normalize);
		}

		void set_attribute(std::string name, VertexArrayVector<Vec4f> &buffer)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_float, 0, sizeof(Vec4f), false);
		}

		void set_attribute(std::string name, VertexArrayVector<Colorf> &buffer)
		{
			set_attribute(name, (VertexArrayBuffer&)buffer, 4, type_float, 0, sizeof(Colorf), false);
		}

		void set_attribute_screen_quad(std::string name);
		void set_attribute_uv_quad(std::string name);

		void set_elements(ElementArrayBuffer &element_array, VertexAttributeDataType indices_type);

		void set_elements(ElementArrayVector<unsigned int> &element_array)
		{
			set_elements(element_array, type_unsigned_int);
		}

		void set_elements(ElementArrayVector<unsigned short> &element_array)
		{
			set_elements(element_array, type_unsigned_short);
		}

		void set_elements(ElementArrayVector<unsigned char> &element_array)
		{
			set_elements(element_array, type_unsigned_byte);
		}

		void set_draw_count(int count);

	private:
		std::shared_ptr<ShaderEffectDescription_Impl> impl;

		friend class ShaderEffect;
	};

	/// \}
}
