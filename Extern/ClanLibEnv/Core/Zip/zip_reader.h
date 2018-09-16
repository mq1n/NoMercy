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
#include <vector>

namespace clan
{
	/// \addtogroup clanCore_I_O_Data clanCore I/O Data
	/// \{

	class IODevice;
	class ZipReader_Impl;

	/// \brief Zip file reader.
	class ZipReader
	{
	public:
		/// \brief Constructs a ZipReader
		///
		/// \param input = IODevice
		ZipReader(IODevice &input);

		/// \brief Begins reading a file entry in the zip file.
		/** <p>If allow_data_descriptor is set to false, an exception will be thrown
			if the file entry sizes and crc32 values are written in a data descriptor
			instead of the local file header.</p>
			<p>Returns false if there are no more file entries.</p>*/
		bool read_local_file_header(bool allow_data_descriptor = false);

		/// \brief Returns the file name that was stored in the local file header.
		std::string get_filename();

		/// \brief Returns true if the file entry is followed by a data descriptor.
		bool has_data_descriptor() const;

		/// \brief Returns the compressed size of the file entry.
		int64_t get_compressed_size() const;

		/// \brief Returns the uncompressed size of the file entry.
		int64_t get_uncompressed_size() const;

		/// \brief Informs the zip reader what the data descriptor contains.
		void set_data_descriptor_data(int64_t compressed_size, int64_t uncompressed_size, uint32_t crc32);

		/// \brief Reads some file data from the zip file.
		int64_t read_file_data(void *data, int64_t size, bool read_all = true);

	private:
		std::shared_ptr<ZipReader_Impl> impl;
	};

	/// \}
}
