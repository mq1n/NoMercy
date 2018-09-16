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
*/

#pragma once

#include <memory>
#include "../../Core/IOData/file_system.h"
#include "../../Core/Resources/resource.h"
#include "graphic_context.h"
#include "../Image/image_import_description.h"
#include "../Image/texture_format.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class Color;
	class Point;
	class PixelBuffer;
	class PixelBufferSet;
	class PixelFormat;
	class TextureProvider;
	class DataBuffer;
	class Texture1D;
	class Texture1DArray;
	class Texture2D;
	class Texture2DArray;
	class Texture3D;
	class TextureCube;
	class TextureCubeArray;
	class Texture_Impl;
	class SharedGCData_Impl;
	class ResourceManager;
	class XMLResourceDocument;

	/// \brief Texture coordinate wrapping modes.
	enum TextureWrapMode
	{
		wrap_clamp_to_edge,
		wrap_repeat,
		wrap_mirrored_repeat
	};

	/// \brief Texture filters.
	enum TextureFilter
	{
		filter_nearest,
		filter_linear,
		filter_nearest_mipmap_nearest,
		filter_nearest_mipmap_linear,
		filter_linear_mipmap_nearest,
		filter_linear_mipmap_linear
	};

	/// \brief Texture compare modes.
	enum TextureCompareMode
	{
		comparemode_none,
		comparemode_compare_r_to_texture
	};

	/// \brief Texture dimensions.
	enum TextureDimensions
	{
		texture_1d,
		texture_1d_array,
		texture_2d,
		texture_2d_array,
		texture_3d,
		texture_cube,
		texture_cube_array
	};

	/// \brief Texture object class.
	class Texture
	{
	public:
		/// \brief Constructs a null instance.
		Texture();

		/// \brief Constructs a texture as described in a pixelbuffer set
		Texture(GraphicContext &gc, PixelBufferSet pixelbuffer_set);

		/// \brief Constructs a texture from an implementation
		///
		/// \param impl = The implementation
		Texture(const std::shared_ptr<Texture_Impl> &impl);

		/// \brief Constructs a texture from a texture provider
		///
		/// \param provider = The provider
		Texture(TextureProvider *provider);

		virtual ~Texture();

		/// \brief Retrieves a Texture resource from the resource manager
		///
		/// \param gc = Graphic Context
		/// \param resources = Resource manager
		/// \param id = id
		static Resource<Texture> resource(GraphicContext &gc, const std::string &id, const ResourceManager &resources);

		/// \brief Loads a Texture from a XML resource definition
		static Texture load(GraphicContext &gc, const std::string &id, const XMLResourceDocument &doc, const ImageImportDescription &import_desc = ImageImportDescription());

		/// \brief Equality operator
		bool operator==(const Texture &other) const
		{
			return impl == other.impl;
		}

		/// \brief Inequality operator
		bool operator!=(const Texture &other) const
		{
			return impl != other.impl;
		}

		/// \brief Less than operator
		bool operator<(const Texture &other) const
		{
			return impl < other.impl;
		}

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Get the minimum level of detail.
		float get_min_lod() const;

		/// \brief Get the maximum level of detail.
		float get_max_lod() const;

		/// \brief Get the level of detail bias constant.
		float get_lod_bias() const;

		/// \brief Get the texture base level.
		int get_base_level() const;

		/// \brief Get the texture max level.
		int get_max_level() const;

		/// \brief Get the texture minification filter.
		TextureFilter get_min_filter() const;

		/// \brief Get the texture magnification filter.
		TextureFilter get_mag_filter() const;

		/// \brief Returns true if texture is resident in texture memory.
		bool is_resident() const;

		/// \brief Get the texture compare mode.
		TextureCompareMode get_compare_mode() const;

		/// \brief Get the texture compare function.
		CompareFunction get_compare_function() const;

		/// \brief Get Provider
		///
		/// \return provider
		TextureProvider *get_provider() const;

		/// \brief Get the implementation weakptr
		///
		/// This is used to assist is creating Texture caches internally within clanlib
		std::weak_ptr<Texture_Impl> get_impl() const;

		/// \brief Generate the mipmap
		void generate_mipmap();

		/// \brief Set the minimum level of detail texture parameter.
		void set_min_lod(float min_lod);

		/// \brief Set the maximum level of detail texture parameter.
		void set_max_lod(float max_lod);

		/// \brief Sets the level of detail bias constant.
		void set_lod_bias(float lod_bias);

		/// \brief Sets the texture base level texture parameter.
		void set_base_level(int base_level);

		/// \brief Sets the texture max level texture parameter.
		void set_max_level(int max_level);

		/// \brief Set the minification filter.
		void set_min_filter(TextureFilter filter);

		/// \brief Set the magnification filter.
		void set_mag_filter(TextureFilter filter);

		/// \brief Set the maximum degree of anisotropy.
		void set_max_anisotropy(float max_anisotropy);

		/// \brief Sets the texture compare mode and compare function texture parameters.
		void set_texture_compare(TextureCompareMode mode, CompareFunction func);

		/// \brief Dynamic cast to Texture1D
		Texture1D to_texture_1d() const;

		/// \brief Dynamic cast to Texture1DArray
		Texture1DArray to_texture_1d_array() const;

		/// \brief Dynamic cast to Texture2D
		Texture2D to_texture_2d() const;

		/// \brief Dynamic cast to Texture2DArray
		Texture2DArray to_texture_2d_array() const;

		/// \brief Dynamic cast to Texture3D
		Texture3D to_texture_3d() const;

		/// \brief Dynamic cast to TextureCube
		TextureCube to_texture_cube() const;

		/// \brief Dynamic cast to TextureCubeArray
		TextureCubeArray to_texture_cube_array() const;

	protected:
		std::shared_ptr<Texture_Impl> impl;

		friend class Texture2DArray;
	};

	/// \}
}
