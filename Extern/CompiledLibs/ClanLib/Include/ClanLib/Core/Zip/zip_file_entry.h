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

#include <memory>
#include "../System/cl_platform.h"

namespace clan
{
	/// \addtogroup clanCore_I_O_Data clanCore I/O Data
	/// \{

	class ZipFileEntry_Impl;

	/// \brief File entry in zip file.
	class ZipFileEntry
	{
	public:
		ZipFileEntry();

		/// \brief Constructs a ZipFileEntry
		///
		/// \param copy = Zip File Entry
		ZipFileEntry(const ZipFileEntry &copy);

		~ZipFileEntry();

		/// \brief Returns the filename of file entry.
		std::string get_archive_filename() const;

		/// \brief Returns the filename of file entry.
		std::string get_input_filename() const;

		/// \brief Returns the uncompressed size of file entry.
		int64_t get_uncompressed_size();

		/// \brief Returns the compressed size of file entry.
		int64_t get_compressed_size();

		/// \brief Is Directory
		///
		/// \return true = directory
		bool is_directory() const;

		/// \brief Copy assignment operator.
		ZipFileEntry &operator =(const ZipFileEntry &copy);

		/// \brief Sets the filename of the file to be added to the archive.
		void set_input_filename(const std::string &filename);

		/// \brief Sets the filename of the file inside the archive.
		void set_archive_filename(const std::string &filename);

		/// \brief Set directory
		///
		/// \param is_directory = bool
		void set_directory(bool is_directory);

	private:
		std::shared_ptr<ZipFileEntry_Impl> impl;

		friend class ZipArchive;
		friend class ZipIODevice_FileEntry;
	};

	/// \}
}
