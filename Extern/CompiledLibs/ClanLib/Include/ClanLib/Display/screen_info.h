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
**    Harry Storbacka
*/

#pragma once

#include <memory>
#include <vector>

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class ScreenInfo_Impl;
	class Rectf;

	/// \brief Screen Information class.
	class ScreenInfo
	{
	public:
		/// \brief Constructs a ScreenInfo object.
		ScreenInfo();

		/// \brief Returns the geometries of the attached screens and the index of the primary screen in the returned array.
		std::vector<Rectf> get_screen_geometries(int &primary_screen_index) const;

	private:
		std::shared_ptr<ScreenInfo_Impl> impl;
	};

	/// \}
}
