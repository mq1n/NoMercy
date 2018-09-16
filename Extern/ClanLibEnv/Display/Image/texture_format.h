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

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	/// \brief Texture format.
	enum TextureFormat
	{
		// Standard ClanLib sized formats (most common listed first)
		tf_rgba8,
		tf_rgb8,
		tf_bgra8,
		tf_bgr8,

		// sized internal format
		tf_stencil_index1,		// For RenderBuffer ?
		tf_stencil_index4,		// For RenderBuffer ?
		tf_stencil_index8,		// For RenderBuffer ?
		tf_stencil_index16,		// For RenderBuffer ?

		tf_r8,
		tf_r8_snorm,
		tf_r16,
		tf_r16_snorm,
		tf_rg8,
		tf_rg8_snorm,
		tf_rg16,
		tf_rg16_snorm,
		tf_r3_g3_b2,
		tf_rgb4,
		tf_rgb5,
		//tf_rgb8, (listed earlier)
		tf_rgb8_snorm,
		tf_rgb10,
		tf_rgb12,
		tf_rgb16,
		tf_rgb16_snorm,
		tf_rgba2,
		tf_rgba4,
		tf_rgb5_a1,
		//tf_rgba8, (listed earlier)
		tf_rgba8_snorm,
		tf_rgb10_a2,
		tf_rgba12,
		tf_rgba16,
		tf_rgba16_snorm,
		tf_srgb8,
		tf_srgb8_alpha8,
		tf_r16f,
		tf_rg16f,
		tf_rgb16f,
		tf_rgba16f,
		tf_r32f,
		tf_rg32f,
		tf_rgb32f,
		tf_rgba32f,
		tf_r11f_g11f_b10f,
		tf_rgb9_e5,
		tf_r8i,
		tf_r8ui,
		tf_r16i,
		tf_r16ui,
		tf_r32i,
		tf_r32ui,
		tf_rg8i,
		tf_rg8ui,
		tf_rg16i,
		tf_rg16ui,
		tf_rg32i,
		tf_rg32ui,
		tf_rgb8i,
		tf_rgb8ui,
		tf_rgb16i,
		tf_rgb16ui,
		tf_rgb32i,
		tf_rgb32ui,
		tf_rgba8i,
		tf_rgba8ui,
		tf_rgba16i,
		tf_rgba16ui,
		tf_rgba32i,
		tf_rgba32ui,
		tf_depth_component16,
		tf_depth_component24,
		tf_depth_component32,
		tf_depth_component32f,
		tf_depth24_stencil8,
		tf_depth32f_stencil8,
		tf_compressed_red,
		tf_compressed_rg,
		tf_compressed_rgb,
		tf_compressed_rgba,
		tf_compressed_srgb,
		tf_compressed_srgb_alpha,
		tf_compressed_red_rgtc1,
		tf_compressed_signed_red_rgtc1,
		tf_compressed_rg_rgtc2,
		tf_compressed_signed_rg_rgtc2,
		tf_compressed_rgb_s3tc_dxt1,
		tf_compressed_rgba_s3tc_dxt1,
		tf_compressed_rgba_s3tc_dxt3,
		tf_compressed_rgba_s3tc_dxt5,
		tf_compressed_srgb_s3tc_dxt1,
		tf_compressed_srgb_alpha_s3tc_dxt1,
		tf_compressed_srgb_alpha_s3tc_dxt3,
		tf_compressed_srgb_alpha_s3tc_dxt5
	};

	/// \}
}
