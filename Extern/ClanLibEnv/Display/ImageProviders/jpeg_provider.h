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

#include "../Image/pixel_buffer.h"
#include "../../Core/IOData/file_system.h"

namespace clan
{
	/// \addtogroup clanDisplay_Image_Providers clanDisplay Image Providers
	/// \{

	class FileSystem;

	/// \brief Image provider that can load JPEG (.jpg) files.
	class JPEGProvider
	{
	public:
		/// \brief Called to load an image with this provider type.
		///
		/// \param filename Name of the file to load.
		/// \param directory Directory that file name is relative to.
		static PixelBuffer load(
			const std::string &filename,
			const FileSystem &fs,
			bool srgb = false);

		static PixelBuffer load(
			const std::string &fullname,
			bool srgb = false);

		static PixelBuffer load(
			IODevice &file,
			bool srgb = false);

		/// \brief Save the given PixelBuffer into a JPEG
		///
		/// \param buffer The PixelBuffer to save, format doesn't matter its converted if needed
		/// \param filename File name of JPEG.
		/// \param directory Directory that the file name is relative to.
		/// \param quality The quality level of the JPEG (0-100), 100 being best quality.
		static void save(
			PixelBuffer buffer,
			const std::string &filename,
			FileSystem &fs,
			int quality = 85);

		static void save(
			PixelBuffer buffer,
			const std::string &fullname,
			int quality = 85);

		static void save(
			PixelBuffer buffer,
			IODevice &file,
			int quality = 85);
	};

	/// \}
}
