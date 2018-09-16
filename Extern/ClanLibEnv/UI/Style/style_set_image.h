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

#include <vector>
#include "style_set_value.h"

namespace clan
{
	/// Gradient stop in a style gradient
	class StyleGradientStop
	{
	public:
		StyleGradientStop() { }
		StyleGradientStop(const StyleSetValue &color, const StyleSetValue &position) : color(color), position(position) { }

		StyleSetValue color;
		StyleSetValue position;
	};

	/// Linear or radial gradient
	class StyleGradient
	{
	public:
		StyleSetValue type;
		StyleSetValue linear_angle;
		StyleSetValue radial_shape;
		StyleSetValue radial_size_x;
		StyleSetValue radial_size_y;
		StyleSetValue radial_position_x;
		StyleSetValue radial_position_y;
		std::vector<StyleGradientStop> stops;
	};

	/// Style image values
	class StyleImage
	{
	public:
		StyleImage() { }
		StyleImage(const StyleSetValue &image) : image(image) { }
		StyleImage(const StyleGradient &gradient) : gradient(gradient) { }

		StyleSetValue image;
		StyleGradient gradient;
	};
}
