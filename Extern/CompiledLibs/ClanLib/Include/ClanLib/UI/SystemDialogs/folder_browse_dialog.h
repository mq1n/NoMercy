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
**    Kenneth Gangstoe
*/

#pragma once

#include <memory>
#include <string>

namespace clan
{
	class View;
	class BrowseFolderDialogImpl;

	/// \brief Displays the system folder browsing dialog
	class BrowseFolderDialog
	{
	public:
		/// \brief Constructs an browse folder dialog.
		BrowseFolderDialog(View *owner);

		/// \brief Get the full path of the directory selected.
		std::string selected_path() const;

		/// \brief Sets the initial directory that is displayed.
		void set_initial_directory(const std::string &path);

		/// \brief Sets the text that appears in the title bar.
		void set_title(const std::string &title);

		/// \brief Shows the file dialog.
		/// \return true if the user clicks the OK button of the dialog that is displayed, false otherwise.
		bool show();

	private:
		std::shared_ptr<BrowseFolderDialogImpl> impl;
	};
}
