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

#include "../soundfilter.h"

namespace clan
{
	/// \addtogroup clanSound_Filters clanSound Filters
	/// \{

	class FadeFilterProvider;

	/// \brief Fade Filter Class
	class FadeFilter : public SoundFilter
	{
	public:
		/// \brief Fade Filter Constructor
		FadeFilter(float initial_volume);

		/// \brief Fade Filter Destructor
		virtual ~FadeFilter();

		/// \brief Returns the current volume of the fade filter, from 0.0f (no volume) to 1.0f (full volume).
		float get_volume() const;

		/// \brief Retrieves the provider.
		FadeFilterProvider *get_provider() const;

		/// \brief Sets the volume instant.
		void set_volume(float new_volume);

		/// \brief Fade to volume, from 0.0f (no volume) to 1.0f (full volume).
		void fade_to_volume(float new_volume, int duration = 1000);
	};

	/// \}
}
