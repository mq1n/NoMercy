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

namespace clan
{
	/// \addtogroup clanCore_System clanCore System
	/// \{

	class ConsoleWindow_Impl;

	/// \brief Text console window.
	///
	///  Note: This class has no effect under Linux.
	class ConsoleWindow
	{
	public:
		/// \brief Console Window constructor.
		///
		/// \param title Window title of console window.
		/// \param width Columns in console window.
		/// \param height Rows in console window.
		ConsoleWindow(
			const std::string &title,
			int width = 80,
			int height = 25);

		/// \brief Console Window destructor.
		~ConsoleWindow();

		/// \brief Waits until user hits a key.
		///
		/// This function does not do anything on Linux.
		void wait_for_key();

		/// \brief Displays 'press any key to close this console window',
		/// \brief and waits until user hits a key.
		///
		/// This function does not do anything on Linux.
		void display_close_message();

	private:
		ConsoleWindow_Impl *impl;
	};

	/// \}
}
