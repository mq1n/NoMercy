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

#include "../../Core/Math/rect.h"

namespace clan
{
	class StyleCascade;
	
	// \brief View position and size after being laid out by layout functions
	class ViewGeometry
	{
	public:
		ViewGeometry() { }
		ViewGeometry(const StyleCascade &style);

		static ViewGeometry from_margin_box(const StyleCascade &style, const Rectf &box);
		static ViewGeometry from_border_box(const StyleCascade &style, const Rectf &box);
		static ViewGeometry from_padding_box(const StyleCascade &style, const Rectf &box);
		static ViewGeometry from_content_box(const StyleCascade &style, const Rectf &box);

		// \brief Margin box relative to parent view
		Rectf margin_box() const;

		// \brief Border box relative to parent view
		Rectf border_box() const;

		// \brief Padding box relative to parent view
		Rectf padding_box() const;

		// \brief Content box relative to parent view
		Rectf content_box() const;

		// \brief Content offset relative to parent view
		Pointf content_pos() const;

		// \brief Content box size
		Sizef content_size() const;

		// \brief Left margin width
		float margin_left = 0.0f;

		// \brief Top margin width
		float margin_top = 0.0f;

		// \brief Right margin width
		float margin_right = 0.0f;

		// \brief Bottom margin width
		float margin_bottom = 0.0f;

		// \brief Left border width
		float border_left = 0.0f;

		// \brief Top border width
		float border_top = 0.0f;

		// \brief Right border width
		float border_right = 0.0f;

		// \brief Bottom border width
		float border_bottom = 0.0f;

		// \brief Left padding width
		float padding_left = 0.0f;

		// \brief Top padding width
		float padding_top = 0.0f;

		// \brief Right padding width
		float padding_right = 0.0f;

		// \brief Bottom padding width
		float padding_bottom = 0.0f;

		// \brief Content area horizontal offset relative to parent view
		float content_x = 0.0f;

		// \brief Content area vertical offset relative to parent view
		float content_y = 0.0f;

		// \brief Content area width
		float content_width = 0.0f;

		// \brief Content area height
		float content_height = 0.0f;
	};
}
