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

#include <functional>
#include <future>

namespace clan
{
	/// \brief Main thread message pump processing
	class RunLoop
	{
	public:
		/// \brief Continously process messages until RunLoop::exit() is called
		static void run();
		
		/// \brief Signal that message processing should end immediately
		static void exit();
		
		/// \brief Process messages for specified amount of time
		/// \retval Returns false if RunLoop::exit() has been called or the OS signalled the app needs to shut down
		static bool process(int timeout_ms = 0);
		
		/// \brief Executes a function on the main thread during message processing
		///
		/// This provides a thread-safe way to execute some code on the main thread
		/// as part of the message processing step.
		static void main_thread_async(std::function<void()> func);
		
		/// \brief Executes a task on the main thread with a future result
		///
		/// This provides a thread-safe way to execute some code on the main thread
		/// as part of the message processing step. The caller can then wait and access
		/// the result of the operation through the returned future.
		template<typename T>
		static std::future<T> main_thread_task(std::function<T()> func)
		{
			auto promise = std::make_shared<std::promise<T>>();
			main_thread_async([=]()
			{
				try
				{
					promise->set_value(func());
				}
				catch (...)
				{
					promise->set_exception(std::current_exception());
				}
			});
			return promise->get_future();
		}

		static std::future<void> main_thread_task(std::function<void()> func)
		{
			auto promise = std::make_shared<std::promise<void>>();
			main_thread_async([=]()
			{
				try
				{
					func();
					promise->set_value();
				}
				catch (...)
				{
					promise->set_exception(std::current_exception());
				}
			});
			return promise->get_future();
		}
	};
}
