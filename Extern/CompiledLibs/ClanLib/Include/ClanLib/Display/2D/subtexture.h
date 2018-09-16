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
	class Subtexture_Impl;

	/// \brief Sub-texture description.
	class Subtexture
	{
	public:
		/// \brief Constructs a null instance.
		Subtexture();

		/// \brief Constructs a Subtexture
		///
		/// \param texture = Texture
		/// \param geometry = Rect
		Subtexture(Texture2D texture, const Rect &geometry);
		~Subtexture();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the texture.
		Texture2D get_texture();

		/// \brief Returns the part of the texture used by this object.
		Rect get_geometry() const;

	private:
		std::shared_ptr<Subtexture_Impl> impl;
	};

	/// \}
}
