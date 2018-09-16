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
	class FileSystem_Impl;
	class FileSystemProvider;
	class DirectoryListing;

	/// \brief Virtual File System (VFS).
	class FileSystem
	{
	public:
		/// \brief Constructs a file system.
		FileSystem();

		/// \brief Constructs a FileSystem
		///
		/// \param provider = Virtual File Source
		FileSystem(FileSystemProvider *provider);

		/// \brief Constructs a FileSystem
		///
		/// \param path = String
		/// \param is_zip_file = bool
		FileSystem(const std::string &path, bool is_zip_file = false);

		~FileSystem();

		/// \brief Returns true if the file system is null.
		bool is_null() const { return !impl; }

		/// \brief Returns true if a path is a mount point.
		bool is_mount(const std::string &mount_point);

		/// \brief Return directory listing for path.
		DirectoryListing get_directory_listing(const std::string &path_rel);

		/// \brief Return true if the root of the filesystem contains the specified file.
		bool has_file(const std::string &filename);

		/// \brief Return true if the root of the filesystem contains the specified directory.
		bool has_directory(const std::string &directory);

		/// \brief Returns the file source for this file system.
		FileSystemProvider *get_provider();

		/// \brief Returns a path to the file source for this file system.
		std::string get_path() const;

		/// \brief Get the identifier of this file source
		///
		/// The exact format of this identifier depends on the implementation of the underlying filesystem and mounts
		/// \return the identifier
		std::string get_identifier() const;

		/// \brief Opens a file.
		/** param: mode = File::OpenMode modes
			param: access = File::AccessFlags flags
			param: share = File::ShareFlags flags
			param: flags = File::Flags flags
			\return The IODevice*/
		IODevice open_file(const std::string &filename,
			File::OpenMode mode = File::open_existing,
			unsigned int access = File::access_read,
			unsigned int share = File::share_all,
			unsigned int flags = 0) const;

		/// \brief Mounts a file system at mount point.
		/** This is only available if FileSystem was set
			Filenames starting with "mount_point" at the start will be replaced by the filesystem specified by "fs"
			(ie the the base_path is ignored)
			For example:
			FileSystem new_vfs(new MyFileSource("Hello"));
			vfs.mount("ABC", new_vfs);
			param: mount_point = Mount alias name to use
			param: fs = Filesystem to use*/
		void mount(const std::string &mount_point, FileSystem fs);

		/// \brief Mounts a file system at mount point.
		/** Filenames starting with "mount_point" at the start will be replaced by the path specified by "path"
			(ie the the base_path is ignored)
			param: mount_point = Mount alias name to use
			param: path = Path which "mount_point" should point to
			param: is_zip_file = false, create as a FileSystemProvider_File, else create as a FileSystemProvider_Zip*/
		void mount(const std::string &mount_point, const std::string &path, bool is_zip_file);

		/// \brief Unmount a file system.
		/** param: mount_point = The mount point to unmount*/
		void unmount(const std::string &mount_point);

	private:
		class NullVFS { };
		explicit FileSystem(class NullVFS null_fs);

		std::shared_ptr<FileSystem_Impl> impl;
	};

	/// \}
}
