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

	class PixelBuffer;

	/// \brief Provider type
	class ImageProviderType
	{
	public:
		/// \brief Registers a provider type in the ImageProviderFactory.
		ImageProviderType(const std::string &type);

		/// \brief Unregisters a provider type in the ImageProviderFactory.
		virtual ~ImageProviderType();

		/// \brief Called to load an image with this provider type.
		virtual PixelBuffer load(
			const std::string &filename,
			const FileSystem &fs,
			bool srgb) = 0;

		virtual PixelBuffer load(
			IODevice &file,
			bool srgb) = 0;

		/// \brief Called to save a given PixelBuffer to a file
		virtual void save(
			PixelBuffer buffer,
			const std::string &filename,
			FileSystem &fs) = 0;

		virtual void save(
			PixelBuffer buffer,
			IODevice &file) = 0;
	};

	/// \}
}
