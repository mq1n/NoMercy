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
**    Harry Storbacka
*/

#pragma once

#include <memory>

#if defined(_MSC_VER)
#define cl_tls_variable _declspec(thread)
#elif defined(__APPLE__) // To do: change check to only apply to iOS/ARM target
#define cl_tls_variable
#else
#define cl_tls_variable __thread
#endif

namespace clan
{
	/// \addtogroup clanCore_System clanCore System
	/// \{

	class ThreadLocalStorage_Instance;

	class ThreadLocalStorageData
	{
	public:
		virtual ~ThreadLocalStorageData() { }
	};

	class ThreadLocalStorage_Impl;

	/// \brief ThreadLocalStorage class.
	class ThreadLocalStorage
	{
	public:
		/// \brief Constructs a Thread Local Storage object.
		ThreadLocalStorage();
		~ThreadLocalStorage();

		/// \brief Get a variable.
		static std::shared_ptr<ThreadLocalStorageData> get_variable(const std::string &name);

		/// \brief Set a variable.
		static void set_variable(const std::string &name, std::shared_ptr<ThreadLocalStorageData> ptr);

	private:
		static void init_core();
		static ThreadLocalStorage_Instance *instance;
		friend class ThreadLocalStorage_Instance;
	};

	/// \}
}
