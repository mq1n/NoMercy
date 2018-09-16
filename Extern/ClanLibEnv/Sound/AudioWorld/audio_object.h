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

#include <string>
#include "audio_definition.h"
#include "../../Core/Math/vec3.h"

namespace clan
{
	class SoundBuffer;
	class AudioWorld;
	class AudioObject_Impl;

	class AudioObject
	{
	public:
		AudioObject();
		AudioObject(AudioWorld &world);
		AudioObject(AudioWorld &world, AudioDefinition definition);

		bool is_null() const { return !impl; }

		Vec3f get_position() const;

		float get_attenuation_begin() const;
		float get_attenuation_end() const;
		float get_volume() const;
		bool is_looping() const;
		bool is_ambience() const;
		bool is_playing() const;

		void set_position(const Vec3f &position);

		void set_attenuation_begin(float distance);
		void set_attenuation_end(float distance);
		void set_volume(float volume);

		void set_sound(const std::string &id);
		void set_sound(const SoundBuffer &buffer);

		void set_looping(bool loop);
		void set_ambience(bool ambience);

		void play();
		void stop();

	private:
		std::shared_ptr<AudioObject_Impl> impl;

		friend class AudioWorld;
		friend class AudioWorld_Impl;
	};
}
