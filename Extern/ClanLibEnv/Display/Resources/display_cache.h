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

#include "../../Core/Resources/resource.h"
#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_Resources clanDisplay Resources
	/// \{

	class ResourceManager;
	class GraphicContext;
	class Canvas;
	class Sprite;
	class Image;
	class Texture;
	class Font;
	class FontDescription;

	class DisplayCache
	{
	public:
		virtual ~DisplayCache() { }
		virtual Resource<Sprite> get_sprite(Canvas &canvas, const std::string &id) = 0;
		virtual Resource<Image> get_image(Canvas &canvas, const std::string &id) = 0;
		virtual Resource<Texture> get_texture(GraphicContext &gc, const std::string &id) = 0;
		virtual Resource<Font> get_font(Canvas &canvas, const std::string &family_name, const FontDescription &desc) = 0;

		static DisplayCache &get(const ResourceManager &resources);
		static void set(ResourceManager &resources, const std::shared_ptr<DisplayCache> &cache);
	};

	/// \}
}
