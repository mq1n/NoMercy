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
**    Harry Storbacka
**    Mark Page
*/

#pragma once

#include "../View/view.h"

namespace clan
{
	class RadioButtonView_Impl;

	/// \brief Radio button component.
	class RadioButtonView : public View
	{
	public:

		RadioButtonView();

		bool disabled() const;
		void set_disabled();
		void set_enabled();

		/// \brief Get Id
		///
		/// \return id
		int radio_id() const;

		/// \brief Is Selected
		///
		/// \return true = selected
		bool selected() const;

		/// \brief Get Group name
		///
		/// \return group_name
		std::string group_name() const;

		/// \brief Set id
		///
		/// \param id = value
		void set_radio_id(int id);

		/// \brief Set selected
		///
		/// \param selected = bool
		void set_selected(bool selected);

		/// \brief Set group name
		void set_group_name(const std::string &str);

		/// \brief Func selected
		std::function<void()> &func_selected();

	private:
		std::shared_ptr<RadioButtonView_Impl> impl;

		friend class RadioButtonView_Impl;
	};

}
