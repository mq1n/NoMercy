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

#include "view_tree.h"
#include "../../Display/Window/display_window.h"
#include "../../Display/Window/keys.h"

namespace clan
{
	class InputEvent;
	class TextureWindow_Impl;

	class TextureWindow : public ViewTree
	{
	public:
		TextureWindow(Canvas &canvas);

		/// \brief Set position and size within the canvas
		///
		/// \param rect = position and size.
		void set_viewport(const Rectf &rect);

		/// \brief Returns the position and size within the canvas
		Rectf viewport() const;

		void set_background_color(const Colorf &background_color = StandardColorf::transparent());
		void set_clear_background(bool enable = true);

		/// \brief Always redraw the entire gui
		void set_always_render(bool enable = true);

		void update();

		/// \brief Set the window that will contain this canvas.
		///
		/// This is used to set the mouse cursor and send events
		///
		/// \param event_window = The window to use
		/// \param enable_automatic_events = Automatically control the window events, passing to the on_...() functions
		/// \param transform_mouse_matrix = Transform the mouse coordinates when enable_automatic_events is true
		void set_window(const DisplayWindow &window, bool enable_automatic_events = true, const Mat4f &transform_mouse_matrix = Mat4f::identity());

		/// User defined events. Call these if set_event_window() is not used
		void on_window_close();
		void on_lost_focus();
		void on_got_focus();
		void on_key_down(const clan::InputEvent &);
		void on_key_up(const clan::InputEvent &);
		void on_mouse_down(const clan::InputEvent &);
		void on_mouse_dblclk(const clan::InputEvent &);
		void on_mouse_up(const clan::InputEvent &);
		void on_mouse_move(const clan::InputEvent &);

		DisplayWindow display_window() override;
		Canvas canvas() const override;

	protected:
		void set_needs_render() override;

		Pointf client_to_screen_pos(const Pointf &pos) override;
		Pointf screen_to_client_pos(const Pointf &pos) override;

	private:
		std::shared_ptr<TextureWindow_Impl> impl;

		friend class TextureWindow_Impl;
	};
}
