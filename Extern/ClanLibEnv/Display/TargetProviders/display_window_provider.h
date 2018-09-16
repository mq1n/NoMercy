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
*/

#pragma once

#include "../../Core/Signals/signal.h"
#include "../Window/display_window.h"
#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class Rect;
	class Size;
	class Point;
	class DisplayWindowDescription;
	class GraphicContext;
	class InputDevice;
	class CursorProvider;
	class CursorDescription;

	/// Display Window site.
	class DisplayWindowSite
	{
	public:
		/// Lost focus signal.
		Signal<void()> sig_lost_focus;

		/// Obtained focus signal.
		Signal<void()> sig_got_focus;

		/// Resize signal.
		Signal<void(float, float)> sig_resize;

		/// Paint signal.
		Signal<void()> sig_paint;

		/// Window close signal.
		Signal<void()> sig_window_close;

		/// Window destroy signal.
		Signal<void()> sig_window_destroy;

		/// Window minimized signal.
		Signal<void()> sig_window_minimized;

		/// Window maximized signal.
		Signal<void()> sig_window_maximized;

		/// Window restored signal.
		Signal<void()> sig_window_restored;

		/// Window resize callback function.
		std::function<void(Rectf &)> func_window_resize;

		/// Minimize button is clicked callback function.
		std::function<bool()> func_minimize_clicked;

		/// Window moved signal.
		Signal<void()> sig_window_moved;

#ifdef WIN32
		std::function<bool(HWND, UINT, WPARAM, LPARAM)> func_window_message;
		Signal<void(HWND, UINT, WPARAM, LPARAM)> sig_window_message;
#endif
	};

	/// Interface for implementing a DisplayWindow target.
	class DisplayWindowProvider
	{
	public:
		virtual ~DisplayWindowProvider() { return; }

		/// Returns the position and size of the window frame.
		virtual Rect get_geometry() const = 0;

		/// Returns the drawable area of the window.
		virtual Rect get_viewport() const = 0;

		/** Returns the display pixel ratio of the window.
		 *  \seealso Resolution Independence
		 */
		virtual float get_pixel_ratio() const = 0;

		/// Returns true if window has focus.
		virtual bool has_focus() const = 0;

		/// Returns true if the window is minimized.
		virtual bool is_minimized() const = 0;

		/// Returns true if the window is maximized.
		virtual bool is_maximized() const = 0;

		/// Returns true if the window is visible.
		virtual bool is_visible() const = 0;

		/// Returns true if the window is fullscreen.
		virtual bool is_fullscreen() const = 0;

		/// Returns the minimum size of the window.
		virtual Size get_minimum_size(bool client_area) const = 0;

		/// Returns the maximum size of the window.
		virtual Size get_maximum_size(bool client_area) const = 0;

		/// Returns the maximum size of the window.
		virtual std::string get_title() const = 0;

		/// Returns the graphic context for the window.
		virtual GraphicContext& get_gc() = 0;

		/// \brief Returns the keyboard input device.
		virtual InputDevice &get_keyboard() = 0;

		/// \brief Returns the mouse input device.
		virtual InputDevice &get_mouse() = 0;

		/// \brief Returns the game controller input device.
		virtual std::vector<InputDevice> &get_game_controllers() = 0;

		/** Returns an platform-specific internal display window handle object.
		 */
		virtual DisplayWindowHandle get_handle() const = 0;

		/// Returns true if text is available in the clipboard.
		virtual bool is_clipboard_text_available() const = 0;

		/// Returns true if an image is available in the clipboard.
		virtual bool is_clipboard_image_available() const = 0;

		/// Returns the text stored in the clipboard.
		virtual std::string get_clipboard_text() const = 0;

		/// Returns the image stored in the clipboard.
		virtual PixelBuffer get_clipboard_image() const = 0;

		/// Convert from window client coordinates to screen coordinates.
		virtual Point client_to_screen(const Point &client) = 0;

		/// Convert from screen coordinates to client coordinates.
		virtual Point screen_to_client(const Point &screen) = 0;

		/// Capture/Release the mouse.
		virtual void capture_mouse(bool capture) = 0;

		/// Invalidates the screen, causing a repaint.
		virtual void request_repaint() = 0;

		/// Creates window, assigning site and description to provider.
		virtual void create(DisplayWindowSite *site, const DisplayWindowDescription &description) = 0;

		/// Shows the mouse cursor.
		virtual void show_system_cursor() = 0;

		/// Hides the mouse cursor.
		virtual void hide_system_cursor() = 0;

		/// Creates a new custom cursor.
		virtual CursorProvider *create_cursor(const CursorDescription &cursor_description) = 0;

		/// Sets the current cursor icon.
		virtual void set_cursor(CursorProvider *cursor) = 0;

		/// Sets the current cursor icon.
		virtual void set_cursor(StandardCursor type) = 0;

#ifdef WIN32
		/// Sets the current cursor handle (win32 only)
		virtual void set_cursor_handle(HCURSOR cursor) = 0;
#endif

		/// Change window title.
		virtual void set_title(const std::string &new_title) = 0;

		/// Sets the position and size of this window on the screen.
		virtual void set_position(const Rect &pos, bool client_area) = 0;

		/** Sets the size of this window.
		 *  \param width       Minimum width of the window.
		 *  \param height      Minimum height of the window.
		 *  \param client_area Size includes the entire window frame?
		 */
		virtual void set_size(int width, int height, bool client_area) = 0;

		/** Sets the minimum size allowed for this window when resizing.
		 *  \param width       Minimum width of the window.
		 *  \param height      Minimum height of the window.
		 *  \param client_area Size includes the entire window frame?
		 */
		virtual void set_minimum_size(int width, int height, bool client_area) = 0;

		/** Sets the maximum size allowed for this window when resizing.
		 *  \param width       Maximum width of the window.
		 *  \param height      Maximum height of the window.
		 *  \param client_area Size includes the entire window frame?
		 */
		virtual void set_maximum_size(int width, int height, bool client_area) = 0;

		/** Sets the display pixel ratio of this window.
		 *  \param ratio The new display pixel ratio to use on this window.
		 */
		virtual void set_pixel_ratio(float ratio) = 0;

		/// Enables or disables input into this window.
		virtual void set_enabled(bool enable) = 0;

		/// Minimizes the window.
		virtual void minimize() = 0;

		/// Restores the window.
		virtual void restore() = 0;

		/// Maximizes the window.
		virtual void maximize() = 0;

		/// Toggle fullscreen
		/// Only Win32 implementation for now
		virtual void toggle_fullscreen() = 0;

		/// Displays the window in its current size and position.
		virtual void show(bool activate) = 0;

		/// Hides the window.
		virtual void hide() = 0;

		/// Raise window on top of other windows.
		virtual void bring_to_front() = 0;

		/// Flip the window display buffers.
		virtual void flip(int interval) = 0;

		/// Stores text in the clipboard.
		virtual void set_clipboard_text(const std::string &text) = 0;

		/// Stores an image in the clipboard.
		virtual void set_clipboard_image(const PixelBuffer &buf) = 0;

		/// Sets the large icon used for this window.
		virtual void set_large_icon(const PixelBuffer &image) = 0;

		/// Sets the small icon used for this window.
		virtual void set_small_icon(const PixelBuffer &image) = 0;

		/** Enable alpha channel blending for this window.
		 *
		 *  \note This is only supported on Windows Vista and above. You can use
		 *        layered windows to achieve the same effect on systems that do
		 *        not support this.
		 *
		 *  \param blur_rect Window blur area. If its size is `0`, the area of
		 *                   the entire window will be used.
		 */
		virtual void enable_alpha_channel(const Rect &blur_rect) = 0;

		/** Extend the window frame into the client area.
		 *  \note This is only applicable in Windows.
		 */
		virtual void extend_frame_into_client_area(int left, int top, int right, int bottom) = 0;
	};

	/// \}
}
