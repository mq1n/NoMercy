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

namespace clan
{
	/// \addtogroup clanCore_I_O_Data clanCore I/O Data
	/// \{

	/// \brief Path helper functions.
	class PathHelp
	{
	public:
		/// \brief Path types.
		enum PathType
		{
			/// \brief Native file system path. (On windows, '/' are turned into '\'. On linux, '\' are turned into '/')
			path_type_file,

			/// \brief Path using slashes ('/').
			path_type_virtual
		};

		/// \brief  Convert a relative path to an absolute path.
		///
		/// \param base_path The base path (Does not require a trailing slash)
		/// \param relative_path The relative path
		/// \param path_type The path type
		///
		/// \return The converted path
		///
		/// This function behaves differently depending on OS and path_type.
		/// In Windows, if the relative_path begins with a drive letter, and
		/// the driver letter differs from the base path, the path is
		/// converted to an absolute path using the current directory for that
		/// drive. Likewise, if the specified base path does not include a
		/// drive or its path is not absolute, the current drive and directory
		/// is added. The function converts all slashes to backslashes.
		///
		/// If the OS is unix based, there is no location (drive or share name)
		/// and the function also converts all backslashes to slashes.
		///
		/// If the type is path_type_virtual, the base path is required to be
		/// absolute. If it does not start in a slash, the function prefixes a
		/// slash to the path. The current drive or directory is never taken into
		/// account and all backslashes are converted to slashes.
		///
		/// This function calls normalise() on base_path and relative_path.
		///
		/// Example #1:
		/// \code
		/// std::string str = PathHelp::make_absolute("ABD/DEF/", "GFX/Filename.txt", PathHelp::path_type_file);
		/// On Linux: str = "/home/user/ABD/DEF/GFX/Filename.txt"
		/// On Windows: str = "c:\user\ABD\DEF\GFX\Filename.txt"
		/// \endcode
		///
		/// Example #2:
		/// \code
		/// std::string str = PathHelp::make_absolute("/ABD/DEF/", "../Filename.txt", PathHelp::path_type_file);
		/// On Linux: str = "/ABD/Filename.txt"
		/// On Windows: str = "c:\ABD\Filename.txt"
		/// \endcode
		///
		/// Example #3:
		/// \code
		/// std::string str = PathHelp::make_absolute("/ABD/DEF/", "../Filename.txt", PathHelp::path_type_virtual);
		/// On Linux: str = "/ABD/Filename.txt"
		/// On Windows: str = "/ABD/Filename.txt"
		/// \endcode
		static std::string make_absolute(
			const std::string &base_path,
			const std::string &relative_path,
			PathType path_type = path_type_file);

		/// \brief  Converts an absolute path into a path relative to a base path.
		///
		/// \param base_path The base path (Does not require a trailing slash)
		/// \param absolute_path The absolute path
		/// \param path_type The path type
		///
		/// \return The converted path
		///
		/// This function is the inverse of FileHelp::make_absolute. Please
		/// see the detailed information for make_absolute for more information
		/// about behavior.
		///
		/// This function calls normalise() on the base_path and absolute_path-
		///
		/// Example #1 (Windows):
		/// \code
		/// std::string str = PathHelp::make_relative("ABD/DEF/", "c:\user\ABD\Filename.txt", PathHelp::path_type_file);
		/// On Windows: str = "..\Filename.txt"
		/// \endcode
		static std::string make_relative(
			const std::string &base_path,
			const std::string &absolute_path,
			PathType path_type = path_type_file);

		/// \brief Check if a path is absolute.
		///
		/// \param path The path to check
		/// \param path_type The path type (of path)
		/// \return true if path is absolute
		static bool is_absolute(
			const std::string &path,
			PathType path_type = path_type_file);

		/// \brief Check if a path is relative.
		///
		/// \param path The path to check
		/// \param path_type The path type (of path)
		/// \return true if path is relative
		static bool is_relative(
			const std::string &path,
			PathType path_type = path_type_file);

		/// \brief  Normalize a path.
		///
		/// \param path The path to use
		/// \param path_type The path type (of the normalized path)
		/// \return The converted path
		///
		/// Converts all slashes and backslashes into their right type.
		/// Simplifies or removes all . and .., converting the path into its
		/// most readable form.
		static std::string normalize(
			const std::string &path,
			PathType path_type = path_type_file);

		/// \brief  Add trailing slash or backslash to path.
		///
		/// This function checks if a path already has a trailing
		/// slash/backslash and adds it if its missing.
		/// \param path The path to use
		/// \param path_type The path type (of path)
		/// \return The converted path
		static std::string add_trailing_slash(
			const std::string &path,
			PathType path_type = path_type_file);

		/// \brief  Remove trailing slash or backslash from path.
		///
		/// This function checks if a path has a trailing
		/// slash/backslash and removes it if needed.
		/// \param path The path to use
		/// \return The converted path
		static std::string remove_trailing_slash(
			const std::string &path);

		/// \brief  Returns the drive (C:) or share name ( \ \ computer \ share)
		///
		/// If (path_type == path_type_virtual) or not using windows, then this function always returns an empty string.
		/// \param fullname The full path name to use
		/// \param path_type The path type
		/// \return The drive or share name*/
		static std::string get_location(
			const std::string &fullname,
			PathType path_type = path_type_file);

		/// \brief  Returns the path excluding the location and filename.
		///
		/// \param fullname The full path name to use
		/// \param path_type The path type
		/// \return The base path
		///
		/// If (path_type == path_type_virtual) or not using windows, then this function returns the path (excluding filename)
		static std::string get_basepath(
			const std::string &fullname,
			PathType path_type = path_type_file);

		/// \brief Splits the path, excluding the location, into parts.
		/** param: fullname = The full path name to use
			param: path_type = The path type
			\return The split path*/
		static std::vector<std::string> split_basepath(
			const std::string &fullname,
			PathType path_type = path_type_file);

		/// \brief Returns the path including the location.
		/** param: fullname = The full path to use (including filename)
			param: path_type = The path type
			\return The full path including the location (excluding filename)*/
		static std::string get_fullpath(
			const std::string &fullname,
			PathType path_type = path_type_file);

		/// \brief Returns the filename part of a fullname.
		/** param: fullname = The full path to use
			param: path_type = The path type
			\return The filename*/
		static std::string get_filename(
			const std::string &fullname,
			PathType path_type = path_type_file);

		/// \brief Returns the basename part of a fullname.
		/** param: fullname = The full path to use
			param: path_type = The path type
			\return The filename (excluding the extension)*/
		static std::string get_basename(
			const std::string &fullname,
			PathType path_type = path_type_file);

		/// \brief Returns the extension part of a fullname.
		/** <p>If no extension was found, an empty string is returned.</p>
			param: fullname = The full path to use
			param: path_type = The path type
			\return The extension*/
		static std::string get_extension(
			const std::string &fullname,
			PathType path_type = path_type_file);

		/// \brief Create a fullname from parts.
		/** param: fullpath = The full path to use
			param: filename = The filename to use
			param: path_type = The path type
			\return The full name*/
		static std::string get_fullname(
			const std::string &fullpath,
			const std::string &filename,
			PathType path_type = path_type_file);

		/// \brief Create a fullname from parts.
		/** param: fullpath = The full path to use
			param: filename = The filename to use
			param: extension = The filename extension to use
			param: path_type = The path type
			\return The full name*/
		static std::string get_fullname(
			const std::string &fullpath,
			const std::string &filename,
			const std::string &extension,
			PathType path_type = path_type_file);

		/// \brief Create a fullname from parts.
		/** param: location = The location to use
			param: fullpath = The full path to use
			param: filename = The filename to use
			param: extension = The filename extension to use
			param: path_type = The path type
			\return The full name*/
		static std::string get_fullname(
			const std::string &location,
			const std::string &basepath,
			const std::string &filename,
			const std::string &extension,
			PathType path_type = path_type_file);

		/// \brief Concatenates two strings into one adding a trailing slash to first string if missing.
		/// \param part1 First part of path
		/// \param part2 Second part of path
		/// \param path_type The path type (of path)
		static std::string combine(const std::string &part1, const std::string &part2, PathType path_type = path_type_file);
	};

	/// \}
}
