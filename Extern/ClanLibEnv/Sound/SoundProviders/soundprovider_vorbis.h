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

#include "soundprovider.h"
#include "../../Core/IOData/file_system.h"

namespace clan
{
	/// \addtogroup clanVorbis_Sound_Providers clanVorbis Sound Providers
	/// \{

	class InputSourceProvider;
	class SoundProvider_Vorbis_Impl;

	/// \brief Ogg Vorbis format sound provider.
	class SoundProvider_Vorbis : public SoundProvider
	{
	public:
		/// \brief Constructs a sound provider based on an ogg vorbis (.ogg) file.
		///
		/// \param filename Filename of module file.
		/// \param provider Input source provider used to retrieve module file.
		/// \param stream If true, will stream from disk. If false, will load it to memory.
		SoundProvider_Vorbis(
			const std::string &filename,
			const FileSystem &fs,
			bool stream = false);

		SoundProvider_Vorbis(
			const std::string &fullname,
			bool stream = false);

		SoundProvider_Vorbis(
			IODevice &file,
			bool stream = false);

		virtual ~SoundProvider_Vorbis();

		/// \brief Called by SoundBuffer when a new session starts.
		/** \return The soundbuffer session to be attached to the newly started session.*/
		virtual SoundProvider_Session *begin_session() override;

		/// \brief Called by SoundBuffer when a session has finished. After this call,
		/** <p>SoundBuffer will not access the session anymore. It can safely be deleted
			here (and in most cases should be delete here).</p>*/
		virtual void end_session(SoundProvider_Session *session) override;

	private:
		std::shared_ptr<SoundProvider_Vorbis_Impl> impl;

		friend class SoundProvider_Vorbis_Session;
	};

	/// \}
}
