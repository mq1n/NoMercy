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
**    Kenneth Gangstoe
**    Harry Storbacka
*/

/// \brief <p>ClanLib display and input library.</p>
//! Global=Display

#pragma once

#ifdef __cplusplus_cli
#pragma managed(push, off)
#endif

#include "Display/display_target.h"
#include "Display/screen_info.h"
#include "Display/Resources/display_cache.h"
#include "Display/2D/canvas.h"
#include "Display/2D/color.h"
#include "Display/2D/color_hsv.h"
#include "Display/2D/color_hsl.h"
#include "Display/2D/gradient.h"
#include "Display/2D/image.h"
#include "Display/2D/sprite.h"
#include "Display/2D/path.h"
#include "Display/2D/pen.h"
#include "Display/2D/brush.h"
#include "Display/2D/subtexture.h"
#include "Display/2D/texture_group.h"
#include "Display/2D/span_layout.h"
#include "Display/System/run_loop.h"
#include "Display/System/timer.h"
#include "Display/System/detect_hang.h"
#include "Display/Font/font_family.h"
#include "Display/Font/font.h"
#include "Display/Font/font_description.h"
#include "Display/Font/font_metrics.h"
#include "Display/Font/glyph_metrics.h"
#include "Display/Image/pixel_buffer.h"
#include "Display/Image/pixel_buffer_lock.h"
#include "Display/Image/pixel_buffer_help.h"
#include "Display/Image/pixel_buffer_set.h"
#include "Display/Image/icon_set.h"
#include "Display/Image/perlin_noise.h"
#include "Display/Image/image_import_description.h"
#include "Display/Image/pixel_converter.h"
#include "Display/ImageProviders/jpeg_provider.h"
#include "Display/ImageProviders/png_provider.h"
#include "Display/ImageProviders/provider_factory.h"
#include "Display/ImageProviders/provider_type.h"
#include "Display/ImageProviders/provider_type_register.h"
#include "Display/ImageProviders/targa_provider.h"
#include "Display/ImageProviders/dds_provider.h"
#include "Display/Render/blend_state.h"
#include "Display/Render/blend_state_description.h"
#include "Display/Render/depth_stencil_state.h"
#include "Display/Render/depth_stencil_state_description.h"
#include "Display/Render/rasterizer_state.h"
#include "Display/Render/rasterizer_state_description.h"
#include "Display/Render/element_array_buffer.h"
#include "Display/Render/element_array_vector.h"
#include "Display/Render/transfer_buffer.h"
#include "Display/Render/transfer_vector.h"
#include "Display/Render/frame_buffer.h"
#include "Display/Render/graphic_context.h"
#include "Display/Render/occlusion_query.h"
#include "Display/Render/primitives_array.h"
#include "Display/Render/program_object.h"
#include "Display/Render/uniform_buffer.h"
#include "Display/Render/uniform_vector.h"
#include "Display/Render/storage_buffer.h"
#include "Display/Render/storage_vector.h"
#include "Display/Render/render_batcher.h"
#include "Display/Render/render_buffer.h"
#include "Display/Render/shader_object.h"
#include "Display/Render/shared_gc_data.h"
#include "Display/Render/texture.h"
#include "Display/Render/transfer_texture.h"
#include "Display/Render/texture_1d.h"
#include "Display/Render/texture_1d_array.h"
#include "Display/Render/texture_2d.h"
#include "Display/Render/texture_2d_array.h"
#include "Display/Render/texture_3d.h"
#include "Display/Render/texture_cube.h"
#include "Display/Render/texture_cube_array.h"
#include "Display/Render/vertex_array_buffer.h"
#include "Display/Render/vertex_array_vector.h"
#include "Display/ShaderEffect/shader_effect.h"
#include "Display/ShaderEffect/shader_effect_description.h"
#include "Display/TargetProviders/cursor_provider.h"
#include "Display/TargetProviders/display_target_provider.h"
#include "Display/TargetProviders/display_window_provider.h"
#include "Display/TargetProviders/element_array_buffer_provider.h"
#include "Display/TargetProviders/transfer_buffer_provider.h"
#include "Display/TargetProviders/pixel_buffer_provider.h"
#include "Display/TargetProviders/frame_buffer_provider.h"
#include "Display/TargetProviders/graphic_context_provider.h"
#include "Display/TargetProviders/input_device_provider.h"
#include "Display/TargetProviders/occlusion_query_provider.h"
#include "Display/TargetProviders/program_object_provider.h"
#include "Display/TargetProviders/render_buffer_provider.h"
#include "Display/TargetProviders/shader_object_provider.h"
#include "Display/TargetProviders/texture_provider.h"
#include "Display/TargetProviders/uniform_buffer_provider.h"
#include "Display/TargetProviders/storage_buffer_provider.h"
#include "Display/TargetProviders/vertex_array_buffer_provider.h"
#include "Display/TargetProviders/primitives_array_provider.h"
#include "Display/Window/cursor.h"
#include "Display/Window/cursor_description.h"
#include "Display/Window/display_window.h"
#include "Display/Window/display_window_description.h"
#include "Display/Window/input_code.h"
#include "Display/Window/input_device.h"
#include "Display/Window/input_event.h"
#include "Display/Window/keys.h"

#ifdef __cplusplus_cli
#pragma managed(pop)
#endif

#if defined(_MSC_VER)
	#if !defined(_MT)
		#error Your application is set to link with the single-threaded version of the run-time library. Go to project settings, in the C++ section, and change it to multi-threaded.
	#endif
	#if !defined(_DEBUG)
		#if defined(DLL)
			#pragma comment(lib, "clanDisplay-dll.lib")
		#elif defined(_DLL)
			#pragma comment(lib, "clanDisplay-static-mtdll.lib")
		#else
			#pragma comment(lib, "clanDisplay-static-mt.lib")
		#endif
	#else
		#if defined(DLL)
			#pragma comment(lib, "clanDisplay-dll-debug.lib")
		#elif defined(_DLL)
			#pragma comment(lib, "clanDisplay-static-mtdll-debug.lib")
		#else
			#pragma comment(lib, "clanDisplay-static-mt-debug.lib")

		#endif
	#endif
	#pragma comment(lib, "winmm.lib")
	#pragma comment(lib, "gdi32.lib")
#endif
