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
**    Mark Page
*/

#pragma once

#include <map>
#include "../../Core/IOData/file_system.h"

namespace clan
{
	/// \addtogroup clanDisplay_Image_Providers clanDisplay Image Providers
	/// \{

	class PixelBuffer;
	class ImageProviderType;

	/// \brief Image Provider factory
	class ImageProviderFactory
	{
	public:
		/// \brief  Attempts to load an image file from 'filename', using the provider identified by 'type'.
		/// \return If the function fails to load the image, a null pixel buffer object is returned.
		///
		/// <p>If the type is an empty string, it uses the extension of
		/// the filename to determine what type it is.</p>
		/// <p>The virtual directory will be used as a the source.</p>
		static PixelBuffer try_load(
			const std::string &filename,
			const std::string &type = std::string(),
			const FileSystem &fs = FileSystem(),
			std::string *out_failure_reason = nullptr,
			bool srgb = false);

		/// \brief Loads an image file from 'filename', using the provider
		/// \brief identified by 'type'.
		/** <p>If the type is an empty string, it uses the extension of
			the filename to determine what type it is.</p>
			<p>The virtual directory will be used as a the source.</p>*/
		static PixelBuffer load(
			const std::string &filename,
			const FileSystem &fs,
			const std::string &type = std::string(),
			bool srgb = false);

		static PixelBuffer load(
			const std::string &fullname,
			const std::string &type = std::string(),
			bool srgb = false);

		static PixelBuffer load(
			IODevice &file,
			const std::string &type,
			bool srgb = false);

		/// \brief Saves the given PixelBuffer to the file given by 'filename'.
		/** <p>If the type is an empty string, it uses the extension of the
			filename to determine what type it is </p>*/
		static void save(
			PixelBuffer buffer,
			const std::string &filename,
			FileSystem &fs,
			const std::string &type = std::string());

		static void save(
			PixelBuffer buffer,
			const std::string &fullname,
			const std::string &type = std::string());

		static void save(
			PixelBuffer buffer,
			IODevice &file,
			const std::string &type);
	};

	/// \}
}
