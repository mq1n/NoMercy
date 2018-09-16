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
#include <memory>

namespace clan
{
	/// \addtogroup clanCore_ErrorReporting clanCore ErrorReporting
	/// \{

	class CrashReporter_Impl;

	/// \brief Generates crash reports (aka. minidumps or core dumps) when the application crashes, or when requested.
	class CrashReporter
	{
	public:
		/// \brief Constructs a crash reporter that creates crash dumps in the specified directory.
		///
		/// If uploader_executable is a non-empty string the executable will be launched if the application crashes.
		CrashReporter(const std::string &reports_directory, const std::string &uploader_executable = std::string());
		~CrashReporter();

		/// \brief Hooks the current thread to the crash reporter.
		///
		/// The crash reporter will only work for hooked threads.
		static void hook_thread();

		/// \brief Force a crash.
		///
		/// This is useful for failing asserts.
		static void invoke();

		/// \brief Generate a crash report for the current thread without terminating.
		static void generate_report();

	private:
		std::shared_ptr<CrashReporter_Impl> impl;
	};

	/// \}
}
