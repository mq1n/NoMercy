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
**    Kenneth Gangstoe
**    Magnus Norddahl
*/

#pragma once

#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_2D clanDisplay 2D
	/// \{

	class Size;
	class Rect;
	class Texture2D;
	class Subtexture;
	class TextureGroup_Impl;
	class GraphicContext;

	/// \brief Texture grouping class.
	class TextureGroup
	{
	public:
		/// \brief Texture allocation policy.
		enum TextureAllocationPolicy
		{
			create_new_texture,
			search_previous_textures
		};

		/// \brief Constructs a null instance
		TextureGroup();

		/// \brief Constructs a texture group
		TextureGroup(const Size &texture_sizes);

		~TextureGroup();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the amount of sub-textures allocated in group.
		int get_subtexture_count() const;

		/// \brief Returns the amount of sub-textures for a specific texture index.
		int get_subtexture_count(unsigned int texture_index) const;

		/// \brief Returns the amount of textures used by group.
		int get_texture_count() const;

		/// \brief Returns the texture allocation policy.
		TextureAllocationPolicy get_texture_allocation_policy() const;

		/// \brief Returns the size of the textures used by this texture group.
		Size get_texture_sizes() const;

		/// \brief Returns the textures.
		std::vector<Texture2D> get_textures() const;

		/// \brief Allocate space for another sub texture.
		Subtexture add(GraphicContext &context, const Size &size);

		/// \brief Deallocate space, from a previously allocated texture
		///
		/// Warning - It is advised to set TextureAllocationPolicy to search_previous_textures
		/// if using this function.  Also be aware of texture fragmentation.
		/// Empty textures are not removed.
		void remove(Subtexture &subtexture);

		/// \brief Set the texture allocation policy.
		void set_texture_allocation_policy(TextureAllocationPolicy policy);

		/// \brief Insert an existing texture into the texture group
		///
		/// \param texture = Texture to insert
		/// \param texture_rect = Free space within the texture that the texture group can use
		void insert_texture(Texture2D &texture, const Rect &texture_rect);

	private:
		std::shared_ptr<TextureGroup_Impl> impl;
	};

	/// \}
}
