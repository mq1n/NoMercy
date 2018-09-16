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
#include <string>
#include <vector>
#include "style_get_value.h"

namespace clan
{
	class Style;
	class StyleGetValue;
	class Canvas;
	class Font;
	class ViewGeometry;

#if defined(MICROSOFT_FINALLY_IMPLEMENTED_CONSTEXPR_TEN_YEARS_AFTER_EVERYONE_ELSE)
	/// Allows property name hashes to be evaluated at compile time
	class PropertyNameConst
	{
	public:
		template<std::size_t Length>
		constexpr PropertyNameConst(const char(&text)[Length]) : text(text), length(Length - 1) { }

		constexpr char operator[](std::size_t index) const { return index < length ? text[index] : throw std::out_of_range("PropertyNameConst operator[] out of bounds"); }
		constexpr std::size_t size() const { return length; }
		constexpr std::size_t hash() const { return hash(2166136261U, 0); }

	private:
		constexpr std::size_t hash(std::size_t value, std::size_t index) const { return index == length ? value : hash((value ^ (std::size_t)text[index]) * 16777619U, index + 1); }

		const char * const text;
		const std::size_t length;
	};
#endif

	/// Style value resolver
	class StyleCascade
	{
	public:
		StyleCascade() { }
		StyleCascade(std::vector<Style *> cascade, const StyleCascade *parent = nullptr) : cascade(std::move(cascade)), parent(parent) { }

		/// Property sets to be examined
		std::vector<Style *> cascade;

		/// Parent cascade used for inheritance
		const StyleCascade *parent = nullptr;
		
		/// Find the first declared value in the cascade for the specified property
		StyleGetValue cascade_value(const char *property_name) const;
		StyleGetValue cascade_value(const std::string &property_name) const { return cascade_value(property_name.c_str()); }

		/// Resolve any inheritance or initial values for the cascade value
		StyleGetValue specified_value(const char *property_name) const;
		StyleGetValue specified_value(const std::string &property_name) const { return specified_value(property_name.c_str()); }

		/// Find the computed value for the specified value
		///
		/// The computed value is a simplified value for the property. Lengths are resolved to device independent pixels and so on.
		StyleGetValue computed_value(const char *property_name) const;
		StyleGetValue computed_value(const std::string &property_name) const { return computed_value(property_name.c_str()); }
		
		/// Convert length into px (device independent pixel) units
		StyleGetValue compute_length(const StyleGetValue &length) const;

		/// Convert angle into radians
		StyleGetValue compute_angle(const StyleGetValue &angle) const;

		/// Convert time to seconds
		StyleGetValue compute_time(const StyleGetValue &time) const;

		/// Convert frequency to Hz
		StyleGetValue compute_frequency(const StyleGetValue &frequency) const;

		/// Convert resolution to dots per px unit (pixel ratio scale)
		StyleGetValue compute_resolution(const StyleGetValue &resolution) const;
		
		/// Value array size for the property
		int array_size(const char *property_name) const;
		int array_size(const std::string &property_name) const { return array_size(property_name.c_str()); }
		
		/// Render styled background
		void render_background(Canvas &canvas, const ViewGeometry &geometry) const;

		/// Render styled border
		void render_border(Canvas &canvas, const ViewGeometry &geometry) const;
		
		/// Font used by this style cascade
		Font font(Canvas &canvas) const;
	};
}
