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

	class DepthStencilStateDescription_Impl;

	/// \brief DepthStencil state description.
	class DepthStencilStateDescription
	{
	public:
		DepthStencilStateDescription();
		DepthStencilStateDescription clone() const;

		bool is_stencil_test_enabled() const;
		void get_stencil_compare_front(CompareFunction &out_front, int &out_front_ref, int &out_front_mask) const;
		void get_stencil_compare_back(CompareFunction &out_back, int &out_back_ref, int &out_back_mask) const;
		void get_stencil_write_mask(unsigned char &out_front_facing_mask, unsigned char &out_back_facing_mask) const;
		void get_stencil_op_front(StencilOp &out_fail_front, StencilOp &out_pass_depth_fail_front, StencilOp &out_pass_depth_pass_front) const;
		void get_stencil_op_back(StencilOp &out_fail_back, StencilOp &out_pass_depth_fail_back, StencilOp &out_pass_depth_pass_back) const;

		bool is_depth_test_enabled() const;
		bool is_depth_write_enabled() const;
		CompareFunction get_depth_compare_function() const;

		/// \brief Enables/disables stencil testing.
		void enable_stencil_test(bool enabled);

		/// \brief Set stencil compare function.
		void set_stencil_compare_front(CompareFunction front, int front_ref, int front_mask);

		/// \brief Set stencil compare function.
		void set_stencil_compare_back(CompareFunction back, int back_ref, int back_mask);

		/// \brief Set stencil write masks.
		void set_stencil_write_mask(unsigned char front_facing_mask, unsigned char back_facing_mask);

		/// \brief Set stencil operations.
		///
		/// Set stencil failed test operations.
		/// Set stencil passed test operations.
		/// Set stencil passed depth test but failed stencil test operations.
		void set_stencil_op_front(StencilOp fail_front, StencilOp pass_depth_fail_front, StencilOp pass_depth_pass_front);

		/// \brief Set stencil operations.
		///
		/// Set stencil failed test operations.
		/// Set stencil passed test operations.
		/// Set stencil passed depth test but failed stencil test operations.
		void set_stencil_op_back(StencilOp fail_back, StencilOp pass_depth_fail_back, StencilOp pass_depth_pass_back);

		/// \brief Enables/disables depth testing.
		void enable_depth_test(bool enabled);

		/// \brief Enables/disables depth writing.
		void enable_depth_write(bool enabled);

		/// \brief Set depth test function.
		void set_depth_compare_function(CompareFunction func);

		bool operator==(const DepthStencilStateDescription &other) const;
		bool operator<(const DepthStencilStateDescription &other) const;

	private:
		std::shared_ptr<DepthStencilStateDescription_Impl> impl;
	};

	/// \}
}
