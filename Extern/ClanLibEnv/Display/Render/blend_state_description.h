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

#include "graphic_context.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class BlendStateDescription_Impl;

	/// \brief Blend state description.
	class BlendStateDescription
	{
	public:
		BlendStateDescription();
		BlendStateDescription clone() const;

		static BlendStateDescription blend(bool src_premultiplied, bool dest_premultiplied = true);
		static BlendStateDescription opaque();

		bool is_blending_enabled() const;
		void get_blend_equation(BlendEquation &out_color, BlendEquation &out_alpha) const;
		void get_blend_function(BlendFunc &out_src, BlendFunc &out_dest, BlendFunc &out_src_alpha, BlendFunc &out_dest_alpha) const;
		void get_color_write(bool &out_red, bool &out_green, bool &out_blue, bool &out_alpha) const;
		bool is_logic_op_enabled() const;
		LogicOp get_logic_op() const;

		/// \brief Enable/Disable blending
		void enable_blending(bool value);

		/// \brief Set the constant color used in the blend equations
		void set_blend_equation(BlendEquation color, BlendEquation alpha);

		/// \brief Set the blend functions
		void set_blend_function(BlendFunc src, BlendFunc dest, BlendFunc src_alpha, BlendFunc dest_alpha);

		/// \brief Enable/disable writing to the color buffer
		void enable_color_write(bool red, bool green, bool blue, bool alpha);

		/// \brief Enable/disable logic op. (clanGL only)
		void enable_logic_op(bool enabled);

		/// \brief Set logic operation. (clanGL only)
		void set_logic_op(LogicOp op);

		bool operator==(const BlendStateDescription &other) const;
		bool operator<(const BlendStateDescription &other) const;

	private:
		std::shared_ptr<BlendStateDescription_Impl> impl;
	};

	/// \}
}
