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

#include "../../Core/Text/string_format.h"
#include "../../Core/Math/cl_math.h"
#include "../../Display/2D/color.h"
#include "style_get_value.h"
#include <memory>

namespace clan
{
	class StyleImpl;
	class StyleGetValue;

	/// Style property set
	class Style
	{
	public:
		Style();
		Style(const Style &) = delete;
		~Style();
		Style &operator=(const Style &) = delete;

		/// Parse and apply CSS properties to property set
		///
		/// The properties string uses the same syntax as when styles are specified using the 'style' attribute in HTML.
		/// This function does not clear the current properties already set and can be called multiple times with
		/// individual sets of properties.
		///
		/// Additional arguments can be passed in and will be inserted with the same syntax as when using the
		/// string_format function.
		void set(const std::string &properties);

		template <class Arg1, typename... Values>
		void set(const std::string &properties, Arg1 arg1, Values... values)
		{
			set(string_format(properties, arg1, values...));
		}

		/// Retrieve the declared value for a property
		StyleGetValue declared_value(const char *property_name) const;
		StyleGetValue declared_value(const std::string &property_name) const { return declared_value(property_name.c_str()); }

		/// Static helper that generates a "rgba(%1,%2,%3,%4)" string for the given color.
		static std::string to_rgba(const Colorf &c)
		{
			return string_format(
				"rgba(%1,%2,%3,%4)",
				clamp((int)std::round(c.r * 255), 0, 255),
				clamp((int)std::round(c.g * 255), 0, 255),
				clamp((int)std::round(c.b * 255), 0, 255),
				c.a);
		}

	private:
		std::unique_ptr<StyleImpl> impl;
	};
}
