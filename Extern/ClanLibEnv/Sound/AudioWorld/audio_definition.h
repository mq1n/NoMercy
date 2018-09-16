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

namespace clan
{
	class AudioDefinition_Impl;

	class AudioDefinition
	{
	public:
		AudioDefinition();

		bool is_null() const { return !impl; }

		std::string get_sound_id() const;

		float get_attenuation_begin() const;
		float get_attenuation_end() const;
		float get_volume() const;

		bool is_looping() const;
		bool is_ambience() const;

		void set_sound_id(const std::string &id);

		void set_attenuation_begin(float distance);
		void set_attenuation_end(float distance);
		void set_volume(float volume);

		void set_looping(bool loop);
		void set_ambience(bool ambience);

	private:
		std::shared_ptr<AudioDefinition_Impl> impl;
	};
}
