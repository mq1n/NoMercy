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

#include "../Image/pixel_buffer_set.h"
#include "../../Core/IOData/file_system.h"

namespace clan
{
	/// \addtogroup clanDisplay_Image_Providers clanDisplay Image Providers
	/// \{

	class FileSystem;

	/// \brief Image provider that can load Direct3D texture (.dds) files.
	class DDSProvider
	{
	public:
		/// \brief Called to load an image with this provider type.
		///
		/// \param filename Name of the file to load.
		/// \param directory Directory that file name is relative to.
		static PixelBufferSet load(const std::string &filename, const FileSystem &file_system);
		static PixelBufferSet load(const std::string &fullname);
		static PixelBufferSet load(IODevice &file);
	};

	/// \}
}
