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
**    Harry Storbacka
*/

/// \brief <p>This is the heart of ClanLib. The core library contain the glue that
/// \brief binds all other clanlib libraries together.</p>
//! Global=Core

#pragma once

#define CLANLIB_VERSION(x,y,z)	( (x << 16) | (y << 8) | (z) )
#define CLANLIB_CURRENT_VERSION	CLANLIB_VERSION(4,0,1)
#define CLANLIB_VERSION_STRING "4.0.1"

#ifdef _MSC_VER
#pragma warning( disable : 4786)
#endif

#ifdef __cplusplus_cli
#pragma managed(push, off)
#endif

#include <string>

#ifdef WIN32
#include <windows.h>
#endif

#include "Core/System/cl_platform.h"
#include "Core/System/comptr.h"
#include "Core/Text/file_logger.h"
#include "Core/Text/console.h"
#include "Core/Text/console_logger.h"
#include "Core/Text/logger.h"
#include "Core/Text/string_format.h"
#include "Core/Text/string_help.h"
#include "Core/Text/utf8_reader.h"
#include "Core/System/databuffer.h"
#include "Core/System/block_allocator.h"
#include "Core/System/console_window.h"
#include "Core/System/datetime.h"
#include "Core/System/disposable_object.h"
#include "Core/System/exception.h"
#include "Core/System/service.h"
#include "Core/System/thread_local_storage.h"
#include "Core/System/system.h"
#include "Core/System/registry_key.h"
#include "Core/System/userdata.h"
#include "Core/System/game_time.h"
#include "Core/System/work_queue.h"
#include "Core/ErrorReporting/crash_reporter.h"
#include "Core/ErrorReporting/exception_dialog.h"
#include "Core/Signals/signal.h"
#include "Core/Resources/resource.h"
#include "Core/Resources/resource_container.h"
#include "Core/Resources/resource_object.h"
#include "Core/Resources/resource_manager.h"
#include "Core/Resources/file_resource_document.h"
#include "Core/Resources/file_resource_manager.h"
#include "Core/JSON/json_value.h"
#include "Core/IOData/file.h"
#include "Core/IOData/file_help.h"
#include "Core/IOData/path_help.h"
#include "Core/IOData/iodevice.h"
#include "Core/IOData/iodevice_provider.h"
#include "Core/IOData/directory.h"
#include "Core/IOData/cl_endian.h"
#include "Core/IOData/directory_scanner.h"
#include "Core/IOData/file_system.h"
#include "Core/IOData/file_system_provider.h"
#include "Core/IOData/directory_listing.h"
#include "Core/IOData/memory_device.h"
#include "Core/IOData/html_url.h"
#include "Core/Zip/zip_archive.h"
#include "Core/Zip/zip_writer.h"
#include "Core/Zip/zip_reader.h"
#include "Core/Zip/zip_file_entry.h"
#include "Core/Zip/zlib_compression.h"
#include "Core/Math/angle.h"
#include "Core/Math/base64_encoder.h"
#include "Core/Math/base64_decoder.h"
#include "Core/Math/bezier_curve.h"
#include "Core/Math/circle.h"
#include "Core/Math/vec2.h"
#include "Core/Math/vec3.h"
#include "Core/Math/vec4.h"
#include "Core/Math/delauney_triangulator.h"
#include "Core/Math/ear_clip_triangulator.h"
#include "Core/Math/ear_clip_result.h"
#include "Core/Math/line_math.h"
#include "Core/Math/cl_math.h"
#include "Core/Math/quaternion.h"
#include "Core/Math/mat2.h"
#include "Core/Math/mat3.h"
#include "Core/Math/mat4.h"
#include "Core/Math/origin.h"
#include "Core/Math/outline_triangulator.h"
#include "Core/Math/point.h"
#include "Core/Math/pointset_math.h"
#include "Core/Math/quad.h"
#include "Core/Math/rect.h"
#include "Core/Math/rect_packer.h"
#include "Core/Math/half_float.h"
#include "Core/Math/half_float_vector.h"
#include "Core/Math/big_int.h"
#include "Core/Math/frustum_planes.h"
#include "Core/Math/intersection_test.h"
#include "Core/Math/aabb.h"
#include "Core/Math/obb.h"
#include "Core/Math/easing.h"
#include "Core/Crypto/random.h"
#include "Core/Crypto/secret.h"
#include "Core/Crypto/sha1.h"
#include "Core/Crypto/md5.h"
#include "Core/Crypto/sha224.h"
#include "Core/Crypto/sha256.h"
#include "Core/Crypto/sha384.h"
#include "Core/Crypto/sha512.h"
#include "Core/Crypto/sha512_224.h"
#include "Core/Crypto/sha512_256.h"
#include "Core/Crypto/aes128_encrypt.h"
#include "Core/Crypto/aes128_decrypt.h"
#include "Core/Crypto/aes192_encrypt.h"
#include "Core/Crypto/aes192_decrypt.h"
#include "Core/Crypto/aes256_encrypt.h"
#include "Core/Crypto/aes256_decrypt.h"
#include "Core/Crypto/rsa.h"
#include "Core/Crypto/tls_client.h"
#include "Core/Math/size.h"
#include "Core/Math/triangle_math.h"
#include "Core/Math/line.h"
#include "Core/Math/line_ray.h"
#include "Core/Math/line_segment.h"
#include "Core/Crypto/hash_functions.h"
#include "Core/core_iostream.h"

#ifdef __cplusplus_cli
#pragma managed(pop)
#endif

#if defined(_MSC_VER)
	#if !defined(_MT)
		#error Your application is set to link with the single-threaded version of the run-time library. Go to project settings, in the C++ section, and change it to multi-threaded.
	#endif
	#if !defined(_DEBUG)
		#if defined(DLL)
			#pragma comment(lib, "clanCore-dll.lib")
		#elif defined(_DLL)
			#pragma comment(lib, "clanCore-static-mtdll.lib")
		#else
			#pragma comment(lib, "clanCore-static-mt.lib")
		#endif
	#else
		#if defined(DLL)
			#pragma comment(lib, "clanCore-dll-debug.lib")
		#elif defined(_DLL)
			#pragma comment(lib, "clanCore-static-mtdll-debug.lib")
		#else
			#pragma comment(lib, "clanCore-static-mt-debug.lib")
		#endif
	#endif
	#pragma comment(lib, "advapi32.lib")
#endif

