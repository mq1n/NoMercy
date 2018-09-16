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

#include <list>
#include "../../Core/Math/vec3.h"
#include "../../Core/Math/quaternion.h"

namespace clan
{
	class ResourceManager;
	class SoundBuffer;
	class AudioWorld_Impl;

	class AudioWorld
	{
	public:
		AudioWorld(const ResourceManager &resources);

		void update();

		void set_listener(const Vec3f &position, const Quaternionf &orientation);

		void enable_ambience(bool enable);
		bool is_ambience_enabled() const;

		void enable_reverse_stereo(bool enable);
		bool is_reverse_stereo_enabled() const;

	private:
		std::shared_ptr<AudioWorld_Impl> impl;

		friend class AudioObject;
		friend class AudioObject_Impl;
	};
}
