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
**    Artem Khomenko
*/

#pragma once

#include "../View/view.h"
#include "scroll_view.h"

namespace clan
{
	class ListBoxViewImpl;

	class ListBoxView : public ScrollView
	{
	public:
		ListBoxView();
		~ListBoxView();
		
		void set_items(const std::vector<std::shared_ptr<View>> &items);
		
		template<typename T>
		void set_items(const std::vector<T> &items, const std::function<std::shared_ptr<View>(const T &item)> &map_function)
		{
			std::vector<std::shared_ptr<View>> views;
			views.reserve(items.size());
			for (const auto &item : items)
			{
				views.push_back(map_function(item));
			}
			set_items(views);
		}
		
		int selected_item() const;
		void set_selected_item(int index);

		std::function<void()> &func_selection_changed();

		void layout_children(Canvas &canvas) override;

	private:
		std::unique_ptr<ListBoxViewImpl> impl;

		// Sets in set_selected_item() for scroll to selected in layout_children().
		bool needScrollToSelected = false;
	};
}
