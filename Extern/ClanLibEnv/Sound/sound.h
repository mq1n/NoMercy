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

namespace clan
{
	/// \addtogroup clanSound_Audio_Mixing clanSound Audio Mixing
	/// \{

	class SoundOutput;

	/// \brief Sound interface in ClanLib.
	///
	//    <p>All the functions that share name with those in SoundOutput have the
	//    same functionality. The only difference is, that the ones Sound
	//    operate on a selected sound output. This saves the trouble of passing around
	//    a pointer to the sound output, when only a single one is used anyway.</p>
	class Sound
	{
	public:
		/// \brief Returns the name of the current sound output:
		static const std::string &get_name();

		/// \brief Returns the mixing frequency of the current sound output.
		static int get_mixing_frequency();

		/// \brief Returns the current selected sound output.
		static SoundOutput get_current_output();

		/// \brief Change the current selected sound output.
		///
		/// \param output The new current selected sound output.
		static void select_output(const SoundOutput &output);
	};

	/// \}
}
