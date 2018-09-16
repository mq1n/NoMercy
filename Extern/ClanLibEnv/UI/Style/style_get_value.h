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

#include <string>
#include "../../Display/2D/color.h"
#include "style_value_type.h"
#include "style_dimension.h"

namespace clan
{
	/// Style value returned by style classes
	class StyleGetValue
	{
	public:
		/// Variant type
		StyleValueType type() const { return _type; }

		/// Text when the type is a text string
		const char *text() const
		{
			switch (_type)
			{
			case StyleValueType::keyword:
			case StyleValueType::url:
			case StyleValueType::string:
				return _value.text;
			default:
				return "";
			}
		}

		/// Value number
		float number() const
		{
			switch (_type)
			{
			case StyleValueType::length:
			case StyleValueType::percentage:
			case StyleValueType::number:
			case StyleValueType::angle:
			case StyleValueType::time:
			case StyleValueType::frequency:
			case StyleValueType::resolution:
				return _value.float_value.number;
			default:
				return 0.0f;
			}
		}

		/// Dimension used by value
		StyleDimension dimension() const
		{
			switch (_type)
			{
			case StyleValueType::length:
			case StyleValueType::angle:
			case StyleValueType::time:
			case StyleValueType::frequency:
			case StyleValueType::resolution:
				return _value.float_value.dimension;
			default:
				return StyleDimension::px;
			}
		}

		/// Value color
		Colorf color() const
		{
			if (_type == StyleValueType::color)
				return Colorf(_value.color[0], _value.color[1], _value.color[2], _value.color[3]);
			else
				return Colorf();
		}

		/// Check if value is undefined
		bool is_undefined() const { return _type == StyleValueType::undefined; }

		/// Check if value is a keyword
		bool is_keyword() const { return _type == StyleValueType::keyword; }
		bool is_keyword(const char *keyword) const { return is_keyword() && strcmp(_value.text, keyword) == 0; }
		bool is_keyword(const std::string &keyword) const { return is_keyword() && _value.text == keyword; }

		/// Check if value is a length
		bool is_length() const { return _type == StyleValueType::length; }

		/// Check if value is an angle
		bool is_angle() const { return _type == StyleValueType::angle; }

		/// Check if value is a time
		bool is_time() const { return _type == StyleValueType::time; }

		/// Check if value is a frequency
		bool is_frequency() const { return _type == StyleValueType::frequency; }

		/// Check if value is a resolution
		bool is_resolution() const { return _type == StyleValueType::resolution; }

		/// Check if value is a percentage
		bool is_percentage() const { return _type == StyleValueType::percentage; }

		/// Check if value is a number
		bool is_number() const { return _type == StyleValueType::number; }

		/// Check if value is a string
		bool is_string() const { return _type == StyleValueType::string; }

		/// Check if value is an url
		bool is_url() const { return _type == StyleValueType::url; }

		/// Check if value is a color
		bool is_color() const { return _type == StyleValueType::color; }

		/// Create style value from a keyword
		static StyleGetValue from_keyword(const char *keyword) { StyleGetValue v; v._type = StyleValueType::keyword; v._value.text = keyword; return v; }

		/// Create style value from a string
		static StyleGetValue from_string(const char *text) { StyleGetValue v; v._type = StyleValueType::string; v._value.text = text; return v; }

		/// Create style value from a length
		static StyleGetValue from_length(float length, StyleDimension dimension = StyleDimension::px) { StyleGetValue v; v._type = StyleValueType::length; v._value.float_value.number = length; v._value.float_value.dimension = dimension; return v; }

		/// Create style value from an angle
		static StyleGetValue from_angle(float angle, StyleDimension dimension = StyleDimension::rad) { StyleGetValue v; v._type = StyleValueType::angle; v._value.float_value.number = angle; v._value.float_value.dimension = dimension; return v; }

		/// Create style value from a time
		static StyleGetValue from_time(float t, StyleDimension dimension = StyleDimension::s) { StyleGetValue v; v._type = StyleValueType::time; v._value.float_value.number = t; v._value.float_value.dimension = dimension; return v; }

		/// Create style value from a frequency
		static StyleGetValue from_frequency(float freq, StyleDimension dimension = StyleDimension::hz) { StyleGetValue v; v._type = StyleValueType::frequency; v._value.float_value.number = freq; v._value.float_value.dimension = dimension; return v; }

		/// Create style value from a resolution
		static StyleGetValue from_resolution(float resolution, StyleDimension dimension = StyleDimension::dppx) { StyleGetValue v; v._type = StyleValueType::resolution; v._value.float_value.number = resolution; v._value.float_value.dimension = dimension; return v; }

		/// Create style value from a percentage
		static StyleGetValue from_percentage(float percentage) { StyleGetValue v; v._type = StyleValueType::percentage; v._value.float_value.number = percentage; return v; }

		/// Create style value from a number
		static StyleGetValue from_number(float number) { StyleGetValue v; v._type = StyleValueType::number; v._value.float_value.number = number; return v; }

		/// Create style value from an url
		static StyleGetValue from_url(const char *url) { StyleGetValue v; v._type = StyleValueType::url; v._value.text = url; return v; }

		/// Create style value from a color
		static StyleGetValue from_color(const Colorf &color) { StyleGetValue v; v._type = StyleValueType::color; v._value.color[0] = color.r; v._value.color[1] = color.g; v._value.color[2] = color.b; v._value.color[3] = color.a; return v; }

	private:
		StyleValueType _type = StyleValueType::undefined;

		union
		{
			const char *text;
			struct
			{
				float number;
				StyleDimension dimension;
			} float_value;
			float color[4];
		} _value;
	};
}
