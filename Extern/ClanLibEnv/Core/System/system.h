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
**    Animehunter
*/

#pragma once

#include "../System/cl_platform.h"
#include <vector>
#include <mutex>

namespace clan
{
	/// \addtogroup clanCore_System clanCore System
	/// \{

	/// \brief General system helper functions.
	class System
	{
	public:
		/// \brief Get the current time (since system boot), in milliseconds.
		static uint64_t get_time();

		/// \brief Get the current time microseconds.
		static uint64_t get_microseconds();

		enum CPU_ExtensionX86 { mmx, mmx_ex, _3d_now, _3d_now_ex, sse, sse2, sse3, ssse3, sse4_a, sse4_1, sse4_2, xop, avx, aes, fma3, fma4 };
		enum CPU_ExtensionPPC { altivec };

		static bool detect_cpu_extension(CPU_ExtensionX86 ext);
		static bool detect_cpu_extension(CPU_ExtensionPPC ext);

		/// \brief Return the number of CPU cores
		static int get_num_cores();

		/// \brief Allocates aligned memory
		static void *aligned_alloc(size_t size, size_t alignment = 16);

		/// \brief Frees aligned memory
		static void aligned_free(void *ptr);

		/// \brief Captures a stack back trace by walking up the stack and recording the information for each frame
		static int capture_stack_trace(int frames_to_skip, int max_frames, void **out_frames, unsigned int *out_hash = nullptr);

		/// \brief Returns the function names and lines for the specified stack frame addresses
		/** <p>On Linux, to obtain function names, remember to link with the -rdynamic flag </p>*/
		static std::vector<std::string> get_stack_frames_text(void **frames, int num_frames);

		/// \brief Sleep for 'millis' milliseconds.
		///
		/// It is possible for this function to sleep for more than millis, depending on the OS scheduler.
		/// If you require a more accurate sleep, consider pause.
		static void sleep(int millis);

		/// \brief Pause for 'millis' milliseconds.
		///
		/// This function acts as sleep, but the function "may" perform a spinlock on some operating systems
		/// to give a more accurate pause. This may have a side effect of causing 100% cpu usage.
		/// If you do not require an accurate pause, use sleep instead.
		static void pause(int millis);

		/// \brief Returns the full dirname of the executable that started this
		/// \brief process (aka argv[0])
		/** <p>This is necessary since when programms are started from
			the PATH there is no clean and simple way to figure out
			the location of the data files, thus information is read
			from <tt>/proc/self/exe</tt> under GNU/Linux and from
			GetModuleFileName() on Win32.</p>
			\return full dirname of the executable, trailing slash is included*/
		static std::string get_exe_path();
	};

	/// \}
}
