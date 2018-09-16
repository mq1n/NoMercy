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
**    Harry Storbacka
*/

#pragma once

#include <memory>
#include "zip_file_entry.h"
#include <vector>

namespace clan
{
	/// \addtogroup clanCore_I_O_Data clanCore I/O Data
	/// \{

	class IODevice;
	class ZipArchive_Impl;

	/// \brief Zip archive.
	class ZipArchive
	{
	public:
		/// \brief Constructs or loads a ZIP archive.
		///
		/// \param filename .zip archive to load.
		ZipArchive();

		/// \brief Constructs a ZipArchive
		///
		/// \param input = IODevice
		ZipArchive(IODevice &input);

		/// \brief Constructs a ZipArchive
		///
		/// \param filename = String Ref
		ZipArchive(const std::string &filename);

		/// \brief Constructs a ZipArchive
		///
		/// \param copy = Zip Archive
		ZipArchive(const ZipArchive &copy);

		~ZipArchive();

		/// \brief List of file entries in archive.
		std::vector<ZipFileEntry> get_file_list();

		std::vector<ZipFileEntry> get_file_list(const std::string &path);

		/// \brief Opens a file in the archive.
		IODevice open_file(const std::string &filename);

		/// \brief Get full path to source:
		std::string get_pathname(const std::string &filename);

		/// \brief Creates a new file entry
		IODevice create_file(const std::string &filename, bool compress = true);

		/// \brief Adds a file to zip archive.
		/** <p>File is not added to zip file until it save() is called.</p>
			\param filename Filename of file.*/
		void add_file(const std::string &input_filename, const std::string &filename_in_archive);

		/// \brief Saves zip archive.
		///
		/// \param filename Filename of zip archive. Must not be used to save to the same as loaded from.
		/// <p>If no filename parameter was passed, it will modify the zip
		/// archive loaded at construction time. It does this by creating a
		/// temporary file, saving the new archive, deletes the old one and
		/// renames the temp file to the original archive filename.</p>
		/// -
		/// <p>If the archive was created instead of loaded, a filename must
		/// be specify a filename. Likewise, if saving to same archive as
		/// loaded from, a filename must not be specified. Doing so will
		/// cause the save operation to fail.</p>
		void save();

		/// \brief Save
		///
		/// \param filename = the filename to save to
		void save(const std::string &filename);

		/// \brief Save
		///
		/// \param iodev = The file to save to
		void save(IODevice iodev);

		/// \brief Loads the zip archive from a input device (done automatically at construction).
		void load(IODevice &input);

	private:
		std::shared_ptr<ZipArchive_Impl> impl;
	};

	/// \}
}
