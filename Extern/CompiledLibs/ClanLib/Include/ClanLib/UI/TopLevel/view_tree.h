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

#include "../View/view.h"
#include "../Events/activation_change_event.h"

namespace clan
{
	class DisplayWindow;
	class Canvas;
	class ViewTreeImpl;

	/// Base class for managing a tree of views
	class ViewTree
	{
	public:
		ViewTree();
		virtual ~ViewTree();

		/// The view receiving keyboard events or nullptr if no view has the focus
		View *focus_view() const;

		/// Gets the display window used
		///
		/// This may return a null DisplayWindow
		virtual DisplayWindow display_window() = 0;

		/// Gets the current canvas used to render
		virtual Canvas canvas() const = 0;

		/// Retrieves the root of the view tree
		const std::shared_ptr<View> &root_view() const;

		/// Sets a new root view controller for the view tree
		void set_root_view(std::shared_ptr<View> root_view);

		/// Add a child view
		void add_child(const std::shared_ptr<View> &view)
		{
			root_view()->add_child(view);
		}

		template<typename T, typename... Types>
		std::shared_ptr<T> add_child(Types &&... args)
		{
			auto child = std::make_shared<T>(std::forward<Types>(args)...);
			add_child(child);
			return child;
		}

		std::shared_ptr<View> add_child()
		{
			return add_child<View>();
		}

	protected:
		/// Set or clears the focus
		void set_focus_view(View *view);

		/// Renders view into the specified canvas
		void render(Canvas &canvas, const Rectf &margin_box);

		/// Dispatch activation change event to all views
		void dispatch_activation_change(ActivationChangeType type);

		/// Signals that the root view needs to be rendered again
		virtual void set_needs_render() = 0;

		/// Map from client to screen coordinates
		virtual Pointf client_to_screen_pos(const Pointf &pos) = 0;

		/// Map from screen to client coordinates
		virtual Pointf screen_to_client_pos(const Pointf &pos) = 0;

	private:
		ViewTree(const ViewTree &) = delete;
		ViewTree &operator=(const ViewTree &) = delete;

		std::unique_ptr<ViewTreeImpl> impl;

		friend class View;
		friend class ViewImpl;
		friend class ViewController;
		friend class PositionedLayout;
	};
}
