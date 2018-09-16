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
	class ZipWriter_Impl;

	/// \brief Zip file writer.
	class ZipWriter
	{
	public:
		/// \brief Constructs a ZipWriter
		///
		/// \param output = IODevice
		/// \param storeFilenamesAsUTF8 = bool
		ZipWriter(IODevice &output, bool storeFilenamesAsUTF8 = false);

		/// \brief Begins file entry in the zip file.
		void begin_file(const std::string &filename, bool compress);

		/// \brief Writes some file data to the zip file.
		void write_file_data(const void *data, int64_t size);

		/// \brief Ends the file entry.
		void end_file();

		/// \brief Writes the table of contents part of the zip file.
		void write_toc();

	private:
		std::shared_ptr<ZipWriter_Impl> impl;
	};

	/// \}
}
