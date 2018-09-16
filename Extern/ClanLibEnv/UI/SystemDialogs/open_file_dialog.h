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
#include <vector>

namespace clan
{
	class View;
	class FileDialogImpl;

	/// \brief Displays the system open file dialog.
	class OpenFileDialog
	{
	public:
		/// \brief Constructs an open file dialog.
		OpenFileDialog(View *owner);

		/// \brief Get the full path of the file selected.
		///
		/// If multiple files are selected, this returns the first file.
		std::string filename() const;

		/// \brief Gets an array that contains one file name for each selected file.
		std::vector<std::string> filenames() const;

		/// \brief Sets if multiple files can be selected or not.
		/// \param multiselect = When true, multiple items can be selected.
		void set_multi_select(bool multiselect);

		/// \brief Sets a string containing the full path of the file selected.
		void set_filename(const std::string &filename);

		/// \brief Add a filter that determines what types of files are displayed.
		void add_filter(const std::string &filter_description, const std::string &filter_extension, bool is_default = false);

		/// \brief Clears all filters.
		void clear_filters();

		/// \brief Sets a default filter, on a 0-based index.
		void set_filter_index(int filter_index);

		/// \brief Sets the initial directory that is displayed.
		void set_initial_directory(const std::string &path);

		/// \brief Sets the text that appears in the title bar.
		void set_title(const std::string &title);

		/// \brief Shows the file dialog.
		/// \return true if the user clicks the OK button of the dialog that is displayed, false otherwise.
		bool show();

	private:
		std::shared_ptr<FileDialogImpl> impl;
	};
}
