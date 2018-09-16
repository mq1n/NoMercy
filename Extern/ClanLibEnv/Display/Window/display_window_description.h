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

#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_Window clanDisplay Window
	/// \{

	class Sizef;
	class Rectf;
	class DisplayWindow;
	class DisplayWindowDescription_Impl;
	struct DisplayWindowHandle;

	/// \brief Display window description class.
	///
	/// This class allows you to setup a more advanced description when creating a display window.
	class DisplayWindowDescription
	{
	public:
		/// \brief Constructs a window description with default values.
		DisplayWindowDescription();

		/// \brief Constructs a window description with default values and a title.
		DisplayWindowDescription(const std::string &title);

		/// \brief Constructs a window description with default values, a title and a position.
		DisplayWindowDescription(const std::string &title, const Rectf &position, bool client_area);

		/// \brief Constructs a window description with default values, a title and a size.
		DisplayWindowDescription(const std::string &title, const Sizef &size, bool client_area);

		/// \brief Constructs a window description with default values and a position.
		DisplayWindowDescription(const Rectf &position, bool client_area);

		/// \brief Constructs a DisplayWindowDescription
		/// \param copy = Display Window Description
		DisplayWindowDescription(const DisplayWindowDescription &copy);

		~DisplayWindowDescription();

		/// \brief Returns the window title stored in the description.
		const std::string &get_title() const;

		/// \brief Returns the size of the window stored in description.
		Sizef get_size() const;

		/// \brief Returns the initial position of the window.
		Rectf get_position() const;

		/// \brief Returns if the client area is used for the initial position of the window
		bool get_position_client_area() const;

		/// \brief Returns if window should be initially fullscreen.
		bool is_fullscreen() const;

		/// \brief Returns the amount of flipping buffers to be created.
		int get_flipping_buffers() const;

		/// \brief Returns the swap interval to be used for the window
		int get_swap_interval() const;

		/// \brief Returns true if window should be resizeable.
		bool get_allow_resize() const;

		/// \brief Returns the numbers of bytes per pixel in the window.
		int get_bpp() const;

		/** Returns the refresh rate of the DisplayMode in Hertz (cycles per
		 *  second). This value is also known as the vertical refresh rate.
		 */
		int get_refresh_rate() const;

		/// \brief Returns true if window is layered (black is transparent)
		bool is_layered() const;

		/** Returns an platform-specific internal display window handle object.
		 */
		DisplayWindowHandle get_handle() const;

		/// \brief Returns true if a title bar is shown.
		bool has_caption() const;

		/// \brief Returns true if the window has a window menu on its title bar.
		bool has_sysmenu() const;

		/// \brief Returns true if the window has a minimize button.
		bool has_minimize_button() const;

		/// \brief Returns true if the window has a maximize button.
		bool has_maximize_button() const;

		/// \brief Returns true if the window is initially visible.
		bool is_visible() const;

		/// \brief Returns true if the window should be placed above all non-topmost windows.
		bool is_topmost() const;

		/// \brief Indicates if window is automatically activated or not.
		bool has_no_activate() const;

		/// \brief Returns true if the window is drawn with a drop shadow effect.
		bool has_drop_shadow() const;

		/// \brief Returns the window owning this one.
		DisplayWindow get_owner() const;

		/// \brief Returns the minimum required depth buffer.
		int get_depth_size() const;

		/// \brief Returns the minimum required stencil buffer.
		int get_stencil_size() const;

		/// \brief Returns the index of the monitor to go fullscreen on. See ScreenInfo.
		int get_fullscreen_monitor() const;

		/// \brief Returns true if the screensaver is allowed
		bool get_allow_screensaver() const;

		/// \brief Returns true if DisplayWindow::update should be supported
		bool is_update_supported() const;

		/// \brief Returns the number of samples per pixel.
		int get_multisampling() const;

		/// \brief Returns true if it is a main window
		bool is_main() const;

		/// \brief Returns true if it is a dialog window
		bool is_dialog() const;

		/// \brief Returns true if it is a popup window
		bool is_popup() const;

		/// \brief Copy assignment operator.
		DisplayWindowDescription &operator =(const DisplayWindowDescription &copy);

		/// \brief Display this window as of the main windows of the application
		///
		/// Window is shown on the taskbar
		void set_main_window();

		/// \brief Display the window as a modal dialog
		///
		/// Window is shown on the taskbar
		void set_dialog_window();

		/// \brief Display the window as a popup 
		///
		/// This is treated as a transparent window (without clickthrough).
		/// If clickthrough is also required, use set_layered_window() as well
		void set_popup_window();

		/// \brief Controls if a title bar (including frame) is shown or not.
		///
		/// This is only used for the popup window types (Not Main and Dialog)
		void show_caption(bool value = true);

		/// \brief Sets if the window has a window menu on its title bar.
		///
		/// This is only used for the main window type (not Dialog and Popup)
		void show_sysmenu(bool value = true);

		/// \brief Sets if the window has a minimize button.
		///
		/// This is only used for the main and dialog window types (Not Popup)
		void show_minimize_button(bool value = true);

		/// \brief Sets if the window has a maximize button.
		///
		/// This is only used for the main and dialog window types (Not Popup)
		void show_maximize_button(bool value = true);

		/// \brief Toggles whether the window is created as initially visible.
		void set_visible(bool value = true);

		/// \brief Sets if the window should be placed above all non-topmost windows.
		///
		/// This is used for all window types (Main, Dialog and Popup)
		void set_topmost(bool value = true);

		/// \brief Prevents window from being automatically activated.
		void set_no_activate(bool value = true);

		/// \brief Enables a drop shadow effect on the window.
		void set_drop_shadow(bool value = true);

		/// \brief Specifies another window which owns this one.
		void set_owner_window(const DisplayWindow &owner);

		/// \brief Sets the title of the window.
		void set_title(const std::string &title);

		/// \brief Sets the size of the window (including window frame).
		/// \param size = Window size
		/// \param client_area = false = include the window frame. true = exclude the window frame
		void set_size(const Sizef &size, bool client_area);

		/// \brief Sets the position of the window (including window frame).
		///
		/// The default position, is centred on the screen. If this is wanted, use set_size() instead
		///
		/// \param position = Window position
		/// \param client_area = false = include the window frame. true = exclude the window frame
		void set_position(const Rectf &position, bool client_area);

		/// \brief Makes the window initially fullscreen.
		void set_fullscreen(bool fullscreen = true, int monitor = 0);

		/// \brief Sets the amount of flipping buffers to be used.
		void set_flipping_buffers(int num_buffers = 2);

		/// \brief Sets the swap interval to be used for the window
		void set_swap_interval(int interval = -1);

		/// \brief Sets if windows should be resizable.
		///
		/// This is used for all window types (Main, Dialog and Popup)
		void set_allow_resize(bool allow_resize = true);

		/// \brief Sets the number of bytes per pixel
		void set_bpp(int bpp);

		/** Sets the refresh rate of the DisplayMode in Hertz (cycles per second).
		 *  This value is also known as the vertical refresh rate.
		 */
		void set_refresh_rate(int refresh_rate);

		/// \brief Creates a layered window (complex shaped window)
		///
		/// Use with set_popup_window()
		void set_layered(bool layered);

		/// \brief Allow the screensaver to appear
		///
		/// \param allow_screensaver = Allow the allow_screensaver (default is true)
		void set_allow_screensaver(bool allow_screensaver = true);

		/** Sets the platform-specific internal display window handle object.
		 */
		void set_handle(DisplayWindowHandle handle);

		/// \brief Sets the minimum required depth buffer.
		/** <p>If this value is zero, the smallest available depth buffer is preferred. Otherwise,
			the largest available depth buffer of at least the minimum size is preferred.</p>*/
		void set_depth_size(int value);

		/// \brief Sets the minimum required stencil buffer.
		/** <p>If this value is zero, the smallest available stencil buffer is preferred. Otherwise,
			the largest available stencil buffer of at least the minimum size is preferred.</p>*/
		void set_stencil_size(int value);

		/// \brief Controls if DisplayWindow::update is supported
		///
		/// The Direct3D target needs to know at window creation time if the application intends to update
		/// the screen by calling DisplayWindow::update.
		void set_update_supported(bool value) const;

		/// \brief Sets the number of samples per pixel.
		void set_multisampling(int value);

	private:
		std::unique_ptr<DisplayWindowDescription_Impl> impl;
	};

	/// \}
}
