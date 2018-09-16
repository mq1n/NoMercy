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
**    Mark Page
*/

#pragma once

#include "../../Core/Math/mat3.h"
#include "../../Display/2D/color.h"
#include "image.h"
#include <vector>

namespace clan
{
	class BrushGradientStop
	{
	public:
		BrushGradientStop() { }
		BrushGradientStop(const Colorf &color, float position) : color(color), position(position) { }

		Colorf color;
		float position = 0.0f;
	};

	enum class BrushType
	{
		solid,
		linear,
		radial,
		image
	};

	enum class BrushWrapMode
	{
		clamp,
		wrap,
		mirror
	};

	enum class BrushInterpolateMode
	{
		nearest,
		linear
	};

	class Brush
	{
	public:
		Brush() { }
		Brush(const Colorf &color) : color(color) { }

		BrushType type = BrushType::solid;

		// Generic brush properties
		float opacity = 1.0f;
		Mat3f transform = Mat3f::identity();

		// Solid color
		Colorf color;

		// Gradient color stops
		std::vector<BrushGradientStop> stops;

		// Linear gradient
		Pointf start_point;
		Pointf end_point;

		// Radial gradient
		Pointf center_point;
		Pointf gradient_origin_offset;
		float radius_x = 0.0f;
		float radius_y = 0.0f;

		// Texture
		Image image;
		BrushWrapMode wrap_x = BrushWrapMode::wrap;
		BrushWrapMode wrap_y = BrushWrapMode::wrap;
		BrushInterpolateMode interpolate = BrushInterpolateMode::linear;

		static Brush solid(float r, float g, float b, float a = 1.0f)
		{
			Brush brush;
			brush.color = Colorf(r, g, b, a);
			return brush;
		}
		static Brush solid(const Colorf &color)
		{
			Brush brush;
			brush.color = color;
			return brush;
		}
		static Brush solid_rgb8(int r, int g, int b)
		{
			Brush brush;
			brush.color = Colorf(r, g, b);
			return brush;
		}

		static Brush solid_rgba8(int r, int g, int b, int a)
		{
			Brush brush;
			brush.color = Colorf(r, g, b, a);
			return brush;
		}
	};

}
