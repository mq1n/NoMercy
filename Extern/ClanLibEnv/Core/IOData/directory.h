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
	/// \addtogroup clanCore_I_O_Data clanCore I/O Data
	/// \{

	/// \brief Directory utility class
	class Directory
	{
	public:
		/// \brief Create directory.
		///
		/// \param dir_name = Directory name for create.
		/// \param recursive = If true, function will create all directories in the path, otherwise only the last directory
		/// \return true on success or false on error.
		static bool create(const std::string &dir_name, bool recursive = false);

		/// \brief Remove directory.
		///
		/// \param dir_name = Directory name for delete.
		/// \param delete_files = If true, function will delete files.
		/// \param delete_sub_directories = If true, function will delete subdirectories too.
		/// \return true on success or false on error.
		static bool remove(const std::string &dir_name, bool delete_files = false, bool delete_sub_directories = false);

		/// \brief Rename directory.
		///
		/// \param old_name = Old name of the directory to be renamed.
		/// \param new_name = New directory name.
		/// \return true on success or false on error.
		static bool rename(const std::string &old_name, const std::string &new_name);

		/// \brief Change current directory.
		///
		/// \param dir_name = Directory name to change to.
		/// \return true on success or false on error.
		static bool set_current(const std::string &dir_name);

		/// \brief Get current directory.
		///
		/// \return the current directory path.
		static std::string get_current();

		/// \brief Returns the current user's roaming application data directory.
		///
		/// In Windows, this functions returns special folder directory CSIDL_APPDATA
		///    appended with the "company_name\application_name\version" string. A typical path would be
		///    "C:\Documents and Settings\username\Application Data\company_name\application_name\version\".\n
		/// In OS X, this function returns the directory "~/Library/company_name/application_name/version/".\n
		/// In Linux, this function returns the directory "~/.company_name/application_name/version/".
		///
		/// \param company_name = Company name.
		/// \param application_name = Application name.
		/// \param version = Application version.
		/// \param create_dirs_if_missing = If true, function will create all missing directories in the path.
		///
		/// \return the current user's roaming application data directory.
		static std::string get_appdata(const std::string &company_name, const std::string &application_name, const std::string &version, bool create_dirs_if_missing = true);

		/// \brief Returns the current user's local (nonroaming) application data directory.
		///
		/// In Windows, this functions returns special folder directory CSIDL_LOCAL_APPDATA
		///    appended with the "company_name\application_name\version" string. A typical path would be
		///    "C:\Documents and Settings\username\Local Settings\Application Data\company_name\application_name\version\".\n
		/// In OS X, this function returns the directory "~/Library/company_name/application_name/version/".\n
		/// In Linux, this function returns the directory "~/.company_name/application_name/version/".
		///
		/// \param company_name = Company name.
		/// \param application_name = Application name.
		/// \param version = Application version.
		/// \param create_dirs_if_missing = If true, function will create all missing directories in the path.
		///
		/// \return the current user's local (nonroaming) application data directory.
		static std::string get_local_appdata(const std::string &company_name, const std::string &application_name, const std::string &version, bool create_dirs_if_missing = true);

		/// \brief Returns the application resource data directory.
		///
		/// In Windows, this function returns a data_dir_name subdirectory located at the
		///    executable.  If the executable path is "C:\Program Files\My Application\MyApp.exe",
		///    then it will return the path "C:\Program Files\My Application\data_dir_name\".\n
		/// In OS X, this function returns a "Resources" subdirectory inside the application bundle.
		///    For example, if the application executable path is
		///    "/Applications/MyApplication.app/Contents/MacOS/MyApplication", then it will return
		///    "/Applications/MyApplication.app/Contents/Resources/". If the executable is not in
		///    an application bundle, it will use the same behavior as in Windows; that is, it will
		///    return a data_dir_name subdirectory next to the executable.\n
		/// In Linux, this function will return the directory "../share/application_name/"
		///    relative to the executable, so if it is located in "/usr/bin" it will return
		///    "/usr/share/application_name/"
		///
		/// \param application_name = Application name.
		/// \param data_dir_name = Data directory name.
		///
		/// \return the application resource data directory.
		static std::string get_resourcedata(const std::string &application_name, const std::string &data_dir_name = "Resources");
	};

	/// \}
}
