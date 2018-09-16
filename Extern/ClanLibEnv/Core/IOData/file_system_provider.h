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
#include "file.h"

namespace clan
{
	/// \addtogroup clanCore_I_O_Data clanCore I/O Data
	/// \{

	class IODevice;
	class DirectoryListingEntry;

	/// \brief Virtual File System (VFS) file source.
	class FileSystemProvider
	{
	public:
		virtual ~FileSystemProvider() { return; }

		/// \brief Open file from this source.
		/** param: filename = The filename to use
			param: mode = File::OpenMode modes
			param: access = File::AccessFlags flags
			param: share = File::ShareFlags flags
			param: flags = File::Flags flags
			\return The IODevice*/
		virtual IODevice open_file(const std::string &filename,
			File::OpenMode mode = File::open_existing,
			unsigned int access = File::access_read | File::access_write,
			unsigned int share = File::share_all,
			unsigned int flags = 0) = 0;

		/// \brief Initiate directory listing.
		virtual bool initialize_directory_listing(const std::string &path) = 0;

		/// \brief Update directory listing item.
		virtual bool next_file(DirectoryListingEntry &entry) = 0;

		/// \brief Return the path of this file source.
		virtual std::string get_path() const = 0;

		/// \brief Get the identifier of this file source
		///
		/// The exact format of this identifier depends on the implementation of the underlying filesystem and mounts
		/// \return the identifier
		virtual std::string get_identifier() const = 0;
	};

	/// \}
}
