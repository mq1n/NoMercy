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

#include <memory>

namespace clan
{
	/// \addtogroup clanSound_Audio_Mixing clanSound Audio Mixing
	/// \{

	class SoundFilter_Impl;
	class SoundFilterProvider;

	/// \brief Sound Filter Class
	class SoundFilter
	{
	public:
		/// \brief Constructs a NULL instance
		SoundFilter() { }

		/// \brief Constructs a sound filter
		///
		/// \param provider = The provider
		SoundFilter(SoundFilterProvider *provider);

		~SoundFilter();

		/// \brief Equality operator
		bool operator==(const SoundFilter &other) const
		{
			return impl == other.impl;
		}

		/// \brief Inequality operator
		bool operator!=(const SoundFilter &other) const
		{
			return impl != other.impl;
		}

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Retrieves the provider.
		SoundFilterProvider *get_provider() const;

		/// \brief Filter callback.
		/** <p>All sound data is passed through this function,
			which modifies the sample data accordingly to the function of the
			filter.</p>
			<p>The format of the sample data is always 16 bit stereo. </p>*/
		void filter(float **sample_data, int num_samples, int channels);

		std::shared_ptr<SoundFilter_Impl> impl;
	};

	/// \}
}
