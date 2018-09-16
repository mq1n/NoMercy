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
#include <vector>

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class GraphicContextProvider;
	class XMLResourceDocument;
	class ShaderObject_Impl;
	class ShaderObjectProvider;

	/// \brief Shader Type
	enum ShaderType
	{
		shadertype_vertex,
		shadertype_geometry,
		shadertype_fragment,
		shadertype_tess_evaluation,
		shadertype_tess_control,
		shadertype_compute,
		shadertype_num_types
	};

	/// \brief Shader Object
	///
	///    <p>The source code that makes up a program that gets executed by one of
	///    the programmable stages is encapsulated in one or more shader
	///    objects. Shader objects are attached to a program objects to form a
	///    programmable setup. ShaderObject is ClanLib's C++ interface to OpenGL
	///    shader objects.</p> 
	class ShaderObject
	{
	public:
		/// \brief Constructs a null instance
		ShaderObject();

		/// \brief Constructs an OpenGL shader.
		///
		/// \param type Shader type. Can be shadertype_vertex or shadertype_fragment.
		/// \param source Shader source code, in OpenGL Shader Language (GLSL).
		/// \param gc Graphics context in which to create the shader object
		ShaderObject(GraphicContext &gc, ShaderType type, const std::string &source);

		/// \brief Constructs a ShaderObject
		ShaderObject(GraphicContext &gc, ShaderType type, const void *source, int source_size);

		/// \brief Constructs a ShaderObject
		///
		/// \param gc = Graphic Context
		/// \param type = Shader Type
		/// \param sources = sources
		ShaderObject(GraphicContext &gc, ShaderType type, const std::vector<std::string> &sources);

		/// \brief Constructs a ShaderObject
		///
		/// \param gc_provider = Graphic Context Provider
		/// \param type = Shader Type
		/// \param source = String Ref
		ShaderObject(GraphicContextProvider *gc_provider, ShaderType type, const std::string &source);

		/// \brief Constructs a ShaderObject
		///
		/// \param gc_provider = Graphic Context Provider
		/// \param type = Shader Type
		/// \param sources = sources
		ShaderObject(GraphicContextProvider *gc_provider, ShaderType type, const std::vector<std::string> &sources);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param resource_id = String Ref
		/// \param resources = Resource Manager
		///
		/// \return Shader Object
		static ShaderObject load(GraphicContext &gc, const std::string &resource_id, const XMLResourceDocument &resources);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param type = Shader Type
		/// \param filename = String Ref
		/// \param directory = Virtual Directory
		///
		/// \return Shader Object
		static ShaderObject load(GraphicContext &gc, ShaderType type, const std::string &filename, const FileSystem &fs);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param type = Shader Type
		/// \param fullname = String Ref
		///
		/// \return Shader Object
		static ShaderObject load(GraphicContext &gc, ShaderType type, const std::string &fullname);

		/// \brief Load
		///
		/// \param gc = Graphic Context
		/// \param type = Shader Type
		/// \param file = IODevice
		///
		/// \return Shader Object
		static ShaderObject load(GraphicContext &gc, ShaderType type, IODevice &file);

		/// \brief Load and compile
		///
		/// \param gc = Graphic Context
		/// \param type = Shader Type
		/// \param filename = String Ref
		/// \param directory = Virtual Directory
		///
		/// \return Shader Object
		static ShaderObject load_and_compile(GraphicContext &gc, ShaderType type, const std::string &filename, const FileSystem &fs);

		/// \brief Load and compile
		///
		/// \param gc = Graphic Context
		/// \param type = Shader Type
		/// \param filename = String Ref
		///
		/// \return Shader Object
		static ShaderObject load_and_compile(GraphicContext &gc, ShaderType type, const std::string &filename);

		/// \brief Load and compile
		///
		/// \param gc = Graphic Context
		/// \param type = Shader Type
		/// \param file = IODevice
		///
		/// \return Shader Object
		static ShaderObject load_and_compile(GraphicContext &gc, ShaderType type, IODevice &file);

		virtual ~ShaderObject();

		/// \brief Returns the OpenGL shader handle.
		unsigned int get_handle() const;

		/// \brief Gets the shader type.
		ShaderType get_shader_type() const;

		/// \brief Get shader object's info log.
		std::string get_info_log() const;

		/// \brief Get shader source code.
		std::string get_shader_source() const;

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Get Provider
		///
		/// \return provider
		ShaderObjectProvider *get_provider() const;

		/// \brief Handle comparison operator.
		bool operator==(const ShaderObject &other) const;

		/// \brief Compile program.
		/** <p>If the compiling fails, get_info_log() will return the compile log.</p>*/
		bool compile();

	private:
		std::shared_ptr<ShaderObject_Impl> impl;
	};

	/// \}
}
