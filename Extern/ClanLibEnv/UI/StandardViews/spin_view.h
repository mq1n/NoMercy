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
**    Kenneth Gangstoe
**    Mark Page
*/


#pragma once

#include "../View/view.h"

namespace clan
{
	class SpinView_Impl;

	/// \brief SpinView component.
	class SpinView : public View
	{
	public:

		SpinView();

		std::shared_ptr<View> button_decrement() const;
		std::shared_ptr<View> button_increment() const;

		bool disabled() const;
		void set_disabled();
		void set_enabled();

		/// \brief Get Value
		///
		/// \return value
		double value() const;

		/// \brief Get Min
		///
		/// \return min
		double minimum() const;

		/// \brief Get Max
		///
		/// \return max
		double maximum() const;

		/// \brief Set value
		///
		/// \param value = value
		void set_value(double value);

		/// \brief Set ranges
		///
		/// \param min = value
		/// \param max = value
		void set_ranges(double min, double max);

		/// \brief Set step size
		///
		/// \param step_size = value
		void set_step_size(double step_size);

		/// \brief Set number of decimal places
		///
		/// \param decimal_places = value
		void set_number_of_decimal_places(int decimal_places);

		/// \brief Func value changed
		std::function<void()> &func_value_changed();

	private:
		std::shared_ptr<SpinView_Impl> impl;
	};

}
