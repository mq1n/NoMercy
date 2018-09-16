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

#include <vector>
#include <string>
#include <memory>

namespace clan
{
	/// \addtogroup clanApp_System clanApp System
	/// \{

	/// \brief Base class for the application object
	///
	/// All applications using the clanApp module need to inherit from this class. See ApplicationInstance for more information.
	class Application
	{
	public:
		virtual ~Application() { }

		/// \brief Main loop update handler
		///
		/// Once the target environment has been initialized this function is called continously to update and render the application.
		/// If the function returns false the main loop will end and the application will exit.
		virtual bool update() { return true; }

		/// \brief Set update handler to wait a certain amount of milliseconds between each update
		static void use_timeout_timing(int timeout_ms);

		/// \brief Returns the command line arguments passed to the application
		static const std::vector<std::string> &main_args();
	};

	class ApplicationInstancePrivate
	{
	public:
		ApplicationInstancePrivate(bool catch_exceptions = true);
		virtual std::unique_ptr<Application> create() = 0;
	};

	/// \brief Helper class to create cross platform applications
	///
	/// To use this class, inherit from Application and make a single global instance of ApplicationInstance<YourClass>.
	///
	/// \code
	/// class MyApplication : public clan::Application
	/// {
	/// public:
	///   bool update() override;
	/// };
	///
	/// ApplicationInstance<MyApplication> clanapp;
	/// \endcode
	/// If you do not want exceptions to be automatically caught, pass "false" to the optional catch_exceptions parameter in Application\n
	/// Your program does not have to use this class. For more advanced usage use a normal main function for the target platform and interface with the RunLoop class in clanDisplay\n
	template<typename ApplicationClass>
	class ApplicationInstance : ApplicationInstancePrivate
	{
	public:
		/// \brief Constructor
		ApplicationInstance(bool catch_exceptions = true) : ApplicationInstancePrivate(catch_exceptions)
		{
		}

	private:
		std::unique_ptr<Application> create() override
		{
			return std::unique_ptr<Application>(new ApplicationClass());
		}
	};

	/// \}
}
