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
**    Kenneth Gangstoe
*/

#pragma once

#include <memory>
#include "../../Core/IOData/file_system.h"
#include "graphic_context.h"
#include "uniform_vector.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class ShaderObject;
	class ProgramObject_Impl;
	class GraphicContext;
	class GraphicContextProvider;
	class XMLResourceDocument;
	class ProgramObjectProvider;

	/// \brief Program Object
	///
	///    <p>The shader objects that are to be used by programmable stages of
	///    OpenGL are collected together to form a program object. ProgramObject
	///    is ClanLib's C++ interface to OpenGL program objects.</p>
	///    <p>To construct a program object programatically, the procedure is
	///    as follows:</p>
	///    <pre>
	///    ShaderObject vertex_shader(shadertype_vertex, vertex_glsl_sourcecode);
	///    ShaderObject fragment_shader(shadertype_fragment, fragment_glsl_sourcecode);
	///    ProgramObject program;
	///    program.attach(vertex_shader);
	///    program.attach(fragment_shader);
	///    program.link();
	///    </pre>
	///    <p>For more information about program objects, see the OpenGL 2.0
	///    specification at <a href="http://www.opengl.org">www.opengl.org</a>. Documentation
	///    about the OpenGL Shader Language (GLSL) is also available from www.opengl.org.</p>
	class ProgramObject
	{
	public:
		/// \brief Construct a null instance
		ProgramObject();

		/// \brief Constructs a ProgramObject
		///
		/// \param gc = Graphic Context
		ProgramObject(GraphicContext &gc);

		/// \brief Constructs a ProgramObject
		///
		/// \param gc_provider = Graphic Context Provider
		ProgramObject(GraphicContextProvider *gc_provider);

		/// \brief Constructs a ProgramObject
		///
		/// \param provider = Font Provider
		ProgramObject(ProgramObjectProvider *provider);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param resource_id = String Ref
		/// \param resman = Resource Manager
		///
		/// \return Program Object
		static ProgramObject load(GraphicContext &gc, const std::string &resource_id, const XMLResourceDocument &resman);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param vertex_fullname = String Ref
		/// \param fragment_fullname = String Ref
		///
		/// \return Program Object
		static ProgramObject load(GraphicContext &gc, const std::string &vertex_fullname, const std::string &fragment_fullname);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param vertex_fullname = String Ref
		/// \param geometry_fullname = String Ref
		/// \param fragment_fullname = String Ref
		///
		/// \return Program Object
		static ProgramObject load(GraphicContext &gc, const std::string &vertex_fullname, const std::string &geometry_fullname, const std::string &fragment_fullname);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param vertex_filename = String Ref
		/// \param fragment_filename = String Ref
		/// \param directory = Virtual Directory
		///
		/// \return Program Object
		static ProgramObject load(GraphicContext &gc, const std::string &vertex_filename, const std::string &fragment_filename, const FileSystem &fs);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param vertex_filename = String Ref
		/// \param geometry_filename = String Ref
		/// \param fragment_filename = String Ref
		/// \param directory = Virtual Directory
		///
		/// \return Program Object
		static ProgramObject load(GraphicContext &gc, const std::string &vertex_filename, const std::string &geometry_filename, const std::string &fragment_filename, const FileSystem &fs);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param vertex_file = IODevice
		/// \param fragment_file = IODevice
		///
		/// \return Program Object
		static ProgramObject load(GraphicContext &gc, IODevice &vertex_file, IODevice &fragment_file);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param vertex_file = IODevice
		/// \param geometry_file = IODevice
		/// \param fragment_file = IODevice
		///
		/// \return Program Object
		static ProgramObject load(GraphicContext &gc, IODevice &vertex_file, IODevice &geometry_file, IODevice &fragment_file);

		/// \brief Load and link
		///
		/// \param gc = Graphic Context
		/// \param vertex_fullname = String Ref
		/// \param fragment_fullname = String Ref
		///
		/// \return Program Object
		static ProgramObject load_and_link(GraphicContext &gc, const std::string &vertex_fullname, const std::string &fragment_fullname);

		/// \brief Load and link
		///
		/// \param gc = Graphic Context
		/// \param vertex_fullname = String Ref
		/// \param geometry_fullname = String Ref
		/// \param fragment_fullname = String Ref
		///
		/// \return Program Object
		static ProgramObject load_and_link(GraphicContext &gc, const std::string &vertex_fullname, const std::string &geometry_fullname, const std::string &fragment_fullname);

		/// \brief Load and link
		///
		/// \param gc = Graphic Context
		/// \param vertex_filename = String Ref
		/// \param fragment_filename = String Ref
		/// \param directory = Virtual Directory
		///
		/// \return Program Object
		static ProgramObject load_and_link(GraphicContext &gc, const std::string &vertex_filename, const std::string &fragment_filename, const FileSystem &fs);

		/// \brief Load and link
		///
		/// \param gc = Graphic Context
		/// \param vertex_filename = String Ref
		/// \param geometry_filename = String Ref
		/// \param fragment_filename = String Ref
		/// \param directory = Virtual Directory
		///
		/// \return Program Object
		static ProgramObject load_and_link(GraphicContext &gc, const std::string &vertex_filename, const std::string &geometry_filename, const std::string &fragment_filename, const FileSystem &fs);

		/// \brief Load and link
		///
		/// \param gc = Graphic Context
		/// \param vertex_file = IODevice
		/// \param fragment_file = IODevice
		///
		/// \return Program Object
		static ProgramObject load_and_link(GraphicContext &gc, IODevice &vertex_file, IODevice &fragment_file);

		/// \brief Load and link
		///
		/// \param gc = Graphic Context
		/// \param vertex_file = IODevice
		/// \param geometry_file = IODevice
		/// \param fragment_file = IODevice
		///
		/// \return Program Object
		static ProgramObject load_and_link(GraphicContext &gc, IODevice &vertex_file, IODevice &geometry_file, IODevice &fragment_file);

		virtual ~ProgramObject();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Retrieves the provider.
		ProgramObjectProvider *get_provider() const;

		/// \brief Returns the OpenGL program object handle.
		unsigned int get_handle() const;

		/// \brief Returns the shaders attached to the program object.
		std::vector<ShaderObject> get_shaders() const;

		/// \brief Returns the current info log for the program object.
		std::string get_info_log() const;

		/// \brief Returns the location of a named active attribute.
		///
		/// Returns -1 when unknown
		int get_attribute_location(const std::string &name) const;

		/// \brief Returns the location of a named uniform variable.
		///
		/// Returns -1 when unknown
		int get_uniform_location(const std::string &name) const;

		/// \brief Get the uniform block size
		///
		/// An exception is thrown of block_name was not found
		///
		/// Returns -1 when unknown
		int get_uniform_buffer_size(const std::string &block_name) const;

		/// \brief Get the uniform block size
		int get_uniform_buffer_size(int block_index) const;

		/// \brief Get the uniform block index
		///
		/// Returns -1 when the block index was not found
		int get_uniform_buffer_index(const std::string &block_name) const;

		/// \brief Get the storage buffer index
		///
		/// Returns -1 when the block index was not found
		int get_storage_buffer_index(const std::string &name) const;

		/// \brief Equality operator
		bool operator==(const ProgramObject &other) const;

		/// \brief Add shader to program object.
		void attach(const ShaderObject &obj);

		/// \brief Remove shader from program object.
		void detach(const ShaderObject &obj);

		/// \brief Bind attribute to specific location.
		/** <p>This function must be called before linking.</p>*/
		void bind_attribute_location(int index, const std::string &name);

		/// \brief Bind shader out variable a specific color buffer location.
		/** <p>This function must be called before linking.</p>*/
		void bind_frag_data_location(int color_number, const std::string &name);

		/// \brief Link program.
		/** <p>If the linking fails, get_info_log() will return the link log.</p>*/
		bool link();

		/// \brief Validate program.
		/** <p>If the validation fails, get_info_log() will return the validation log.</p>*/
		bool validate();

		void set_uniform1i(int location, int value_a);
		void set_uniform2i(int location, int value_a, int value_b);
		void set_uniform3i(int location, int value_a, int value_b, int value_c);
		void set_uniform4i(int location, int value_a, int value_b, int value_c, int value_d);
		void set_uniformiv(int location, int size, int count, const int *data);
		void set_uniform2i(int location, const Vec2i &vec) { set_uniform2i(location, vec.x, vec.y); }
		void set_uniform3i(int location, const Vec3i &vec) { set_uniform3i(location, vec.x, vec.y, vec.z); }
		void set_uniform4i(int location, const Vec4i &vec) { set_uniform4i(location, vec.x, vec.y, vec.z, vec.w); }
		void set_uniform2s(int location, const Vec2s &vec) { set_uniform2i(location, vec.x, vec.y); }
		void set_uniform3s(int location, const Vec3s &vec) { set_uniform3i(location, vec.x, vec.y, vec.z); }
		void set_uniform4s(int location, const Vec4s &vec) { set_uniform4i(location, vec.x, vec.y, vec.z, vec.w); }
		void set_uniform2b(int location, const Vec2b &vec) { set_uniform2i(location, vec.x, vec.y); }
		void set_uniform3b(int location, const Vec3b &vec) { set_uniform3i(location, vec.x, vec.y, vec.z); }
		void set_uniform4b(int location, const Vec4b &vec) { set_uniform4i(location, vec.x, vec.y, vec.z, vec.w); }
		void set_uniformiv(int location, int count, const Vec2i *data) { set_uniformiv(location, 2, count, &data->x); }
		void set_uniformiv(int location, int count, const Vec3i *data) { set_uniformiv(location, 3, count, &data->x); }
		void set_uniformiv(int location, int count, const Vec4i *data) { set_uniformiv(location, 4, count, &data->x); }
		void set_uniform1f(int location, float value_a);
		void set_uniform2f(int location, float value_a, float value_b);
		void set_uniform3f(int location, float value_a, float value_b, float value_c);
		void set_uniform4f(int location, float value_a, float value_b, float value_c, float value_d);
		void set_uniformfv(int location, int size, int count, const float *data);
		void set_uniform2f(int location, const Vec2f &vec) { set_uniform2f(location, vec.x, vec.y); }
		void set_uniform3f(int location, const Vec3f &vec) { set_uniform3f(location, vec.x, vec.y, vec.z); }
		void set_uniform4f(int location, const Vec4f &vec) { set_uniform4f(location, vec.x, vec.y, vec.z, vec.w); }
		void set_uniformfv(int location, int count, const Vec2f *data) { set_uniformfv(location, 2, count, &data->x); }
		void set_uniformfv(int location, int count, const Vec3f *data) { set_uniformfv(location, 3, count, &data->x); }
		void set_uniformfv(int location, int count, const Vec4f *data) { set_uniformfv(location, 4, count, &data->x); }
		void set_uniform_matrix(int location, int size, int count, bool transpose, const float *data);
		void set_uniform_matrix(int location, const Mat2f &matrix) { set_uniform_matrix(location, 2, 1, false, matrix.matrix); }
		void set_uniform_matrix(int location, const Mat3f &matrix) { set_uniform_matrix(location, 3, 1, false, matrix.matrix); }
		void set_uniform_matrix(int location, const Mat4f &matrix) { set_uniform_matrix(location, 4, 1, false, matrix.matrix); }
		void set_uniform_matrix(int location, int count, const Mat2f *matrix) { set_uniform_matrix(location, 2, count, false, matrix->matrix); }
		void set_uniform_matrix(int location, int count, const Mat3f *matrix) { set_uniform_matrix(location, 3, count, false, matrix->matrix); }
		void set_uniform_matrix(int location, int count, const Mat4f *matrix) { set_uniform_matrix(location, 4, count, false, matrix->matrix); }

		void set_uniform1i(const std::string &name, int value_a) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform1i(loc, value_a); }
		void set_uniform2i(const std::string &name, int value_a, int value_b) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform2i(loc, value_a, value_b); }
		void set_uniform3i(const std::string &name, int value_a, int value_b, int value_c) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform3i(loc, value_a, value_b, value_c); }
		void set_uniform4i(const std::string &name, int value_a, int value_b, int value_c, int value_d) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform4i(loc, value_a, value_b, value_c, value_d); }
		void set_uniformiv(const std::string &name, int size, int count, const int *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformiv(loc, size, count, data); }
		void set_uniform2i(const std::string &name, const Vec2i &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform2i(loc, vec); }
		void set_uniform3i(const std::string &name, const Vec3i &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform3i(loc, vec); }
		void set_uniform4i(const std::string &name, const Vec4i &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform4i(loc, vec); }
		void set_uniform2s(const std::string &name, const Vec2s &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform2s(loc, vec); }
		void set_uniform3s(const std::string &name, const Vec3s &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform3s(loc, vec); }
		void set_uniform4s(const std::string &name, const Vec4s &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform4s(loc, vec); }
		void set_uniform2b(const std::string &name, const Vec2b &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform2b(loc, vec); }
		void set_uniform3b(const std::string &name, const Vec3b &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform3b(loc, vec); }
		void set_uniform4b(const std::string &name, const Vec4b &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform4b(loc, vec); }
		void set_uniformiv(const std::string &name, int count, const Vec2i *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformiv(loc, count, data); }
		void set_uniformiv(const std::string &name, int count, const Vec3i *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformiv(loc, count, data); }
		void set_uniformiv(const std::string &name, int count, const Vec4i *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformiv(loc, count, data); }
		void set_uniform1f(const std::string &name, float value_a) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform1f(loc, value_a); }
		void set_uniform2f(const std::string &name, float value_a, float value_b) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform2f(loc, value_a, value_b); }
		void set_uniform3f(const std::string &name, float value_a, float value_b, float value_c) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform3f(loc, value_a, value_b, value_c); }
		void set_uniform4f(const std::string &name, float value_a, float value_b, float value_c, float value_d) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform4f(loc, value_a, value_b, value_c, value_d); }
		void set_uniformfv(const std::string &name, int size, int count, const float *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformfv(loc, size, count, data); }
		void set_uniform2f(const std::string &name, const Vec2f &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform2f(loc, vec); }
		void set_uniform3f(const std::string &name, const Vec3f &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform3f(loc, vec); }
		void set_uniform4f(const std::string &name, const Vec4f &vec) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform4f(loc, vec); }
		void set_uniformfv(const std::string &name, int count, const Vec2f *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformfv(loc, count, data); }
		void set_uniformfv(const std::string &name, int count, const Vec3f *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformfv(loc, count, data); }
		void set_uniformfv(const std::string &name, int count, const Vec4f *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniformfv(loc, count, data); }
		void set_uniform_matrix(const std::string &name, int size, int count, bool transpose, const float *data) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform_matrix(loc, size, count, transpose, data); }
		void set_uniform_matrix(const std::string &name, const Mat2f &matrix) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform_matrix(loc, matrix); }
		void set_uniform_matrix(const std::string &name, const Mat3f &matrix) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform_matrix(loc, matrix); }
		void set_uniform_matrix(const std::string &name, const Mat4f &matrix) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform_matrix(loc, matrix); }
		void set_uniform_matrix(const std::string &name, int count, const Mat2f *matrix) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform_matrix(loc, count, matrix); }
		void set_uniform_matrix(const std::string &name, int count, const Mat3f *matrix) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform_matrix(loc, count, matrix); }
		void set_uniform_matrix(const std::string &name, int count, const Mat4f *matrix) { int loc = get_uniform_location(name); if (loc >= 0) set_uniform_matrix(loc, count, matrix); }

		/// \brief Sets the UniformBuffer
		void set_uniform_buffer_index(const std::string &block_name, int bind_index);

		/// \brief Sets the UniformBuffer
		void set_uniform_buffer_index(int block_index, int bind_index);

		/// \brief Sets the UniformBuffer
		void set_storage_buffer_index(const std::string &block_name, int bind_index);

		/// \brief Sets the UniformBuffer
		void set_storage_buffer_index(int block_index, int bind_index);

	private:
		std::shared_ptr<ProgramObject_Impl> impl;
	};

	/// \}
}
