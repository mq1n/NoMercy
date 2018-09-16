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

#include "../../Core/IOData/file_system.h"

namespace clan
{
	/// \addtogroup clanSound_Sound_Providers clanSound Sound Providers
	/// \{

	class SoundProvider;
	class InputSourceProvider;
	class FileSystem;

	/// \brief Sound provider type
	class SoundProviderType
	{
	public:
		/// \brief Registers a sound provider type in the SoundProviderFactory.
		SoundProviderType(const std::string &type);

		/// \brief Unregisters a sound provider type in the SoundProviderFactory.
		virtual ~SoundProviderType();

		/// \brief Called to load with this sound provider type.
		virtual SoundProvider *load(
			const std::string &filename,
			bool streamed,
			const FileSystem &file_system) = 0;

		virtual SoundProvider *load(
			IODevice &file, bool streamed) = 0;
	};

	/// \}
}
