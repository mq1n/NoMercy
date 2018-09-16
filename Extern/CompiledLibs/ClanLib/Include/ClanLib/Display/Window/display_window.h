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
#include "../display_target.h"
#include <memory>

#if !defined(WIN32) && !defined(__ANDROID__) && !defined(__APPLE__)
// We prefer not to include Xlib.h in clanlib (to prevent namespace issues when "using namespace clan")
struct _XDisplay;
typedef struct _XDisplay Display;
typedef unsigned long Window;
#endif

#if defined(__ANDROID__)
struct ANativeWindow;
#endif

namespace clan
{
	/// \addtogroup clanDisplay_Window clanDisplay Window
	/// \{

	class Sizef;
	class Rectf;
	class Display;
	class GraphicContext;
	class DisplayWindowMode;
	class DisplayWindowDescription;
	struct DisplayWindowHandle;
	class InputDevice;
	class Pointf;
	class Cursor;
	class DisplayWindowProvider;
	class DisplayWindow_Impl;
	class PixelBuffer;

	/// \brief Standard Cursor
	enum class StandardCursor
	{
		arrow,
		appstarting,
		cross,
		hand,
		ibeam,
		no,
		size_all,
		size_nesw,
		size_ns,
		size_nwse,
		size_we,
		uparrow,
		wait
	};

	struct DisplayWindowHandle
	{
#ifdef WIN32
		HWND hwnd = 0;
#elif defined(__ANDROID__)
		ANativeWindow *window = 0;
#elif defined(__APPLE__)
#else
		::Display *display = 0;
		::Window window = 0;
		int screen = -1;
#endif
	};

	/// \brief Top-level window class.
	class DisplayWindow
	{
	public:
		/// \brief Constructs a null instance.
		DisplayWindow();

		/// \brief Constructs a window.
		///
		/// \param title = Titlebar text.
		/// \param width = Width in pixels of window.
		/// \param height = Height in pixels of window.
		/// \param start_fullscreen = If true, window will go fullscreen.
		/// \param allow_resize = If true, window will have resize grabbers and can be resized.
		/// \param flipping_buffers = Number of flipping buffers in system. Default is that there is a front buffer and a back buffer.
		/// \param target = Display target used to create the window.
		DisplayWindow(
			const std::string &title,
			float width,
			float height,
			bool start_fullscreen = false,
			bool allow_resize = false,
			int flipping_buffers = 2);

		/// \brief Constructs a window.
		///
		/// \param description = Structure that describes how to create the display window.
		/// \param target = Display target used to create the window.
		DisplayWindow(
			const DisplayWindowDescription &description);

		/// \brief Constructs a window.
		///
		/// \param provider = Display target implementation object.
		DisplayWindow(DisplayWindowProvider *provider);

		~DisplayWindow();

		/// \brief Returns the position and size of the window frame.
		Rectf get_geometry() const;

		/// \brief Returns the drawable area of the window (excluding window frame).
		Rectf get_viewport() const;

		/// \brief Returns true if window is currently running fullscreen.
		bool is_fullscreen() const;

		/// \brief Returns true if window has focus.
		bool has_focus() const;

		/// \brief Return the graphic context for the window.
		GraphicContext& get_gc() const;

		/// \brief Returns the keyboard input device.
		InputDevice &get_keyboard();

		/// \brief Returns the mouse input device.
		InputDevice &get_mouse();

		/// \brief Returns the game controller input device.
		std::vector<InputDevice> &get_game_controllers();

		/// \brief Returns the input device with the given device name
		InputDevice &get_input_device(const std::string &device_name);

		/// \brief Signal emitted when window lost focus.
		Signal<void()> &sig_lost_focus();

		/// \brief Signal emitted when window gain focus.
		Signal<void()> &sig_got_focus();

		/// \brief Signal emitted when window is resized.
		Signal<void(float, float)> &sig_resize();

		/// \brief Signal emitted when the window is invalidated.
		Signal<void()> &sig_paint();

		/// \brief Signal emitted when window is closed.
		Signal<void()> &sig_window_close();

		/// \brief Signal emitted when window is destroyed.
		Signal<void()> &sig_window_destroy();

		/// \brief Signal emitted when window is minimized.
		Signal<void()> &sig_window_minimized();

		/// \brief Signal emitted when window is maximized.
		Signal<void()> &sig_window_maximized();

		/// \brief Signal emitted when window is restored.
		Signal<void()> &sig_window_restored();

		/// \brief Signal emitted after a window has been moved.
		Signal<void()> &sig_window_moved();

		/// \brief Signal emitted when window flip() was called
		Signal<void()> &sig_window_flip();

		/// \brief Callback called when a window is being resized.
		std::function<void(Rectf &)> &func_window_resize();

		/// \brief Callback called when a window is asked to minimize itself.
		std::function<bool()> &func_minimize_clicked();

#ifdef WIN32
		/// \brief Callback called when a windows messages is received.
		std::function<bool(HWND, UINT, WPARAM, LPARAM)> &func_window_message();

		/// \brief Signal called when a windows messages is received.
		Signal<void(HWND, UINT, WPARAM, LPARAM)> &sig_window_message();
#endif

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief returns true if this display window is visible
		bool is_visible() const;

		/// \brief Returns true if the window is minimized.
		bool is_minimized() const;

		/// \brief Returns true if the window is maximized.
		bool is_maximized() const;

		/// \brief Returns the display window provider
		DisplayWindowProvider *get_provider() const;

		/// \brief Returns true if text is available in the clipboard.
		bool is_clipboard_text_available() const;

		/// \brief Returns true if an image is available in the clipboard.
		bool is_clipboard_image_available() const;

		/// \brief Returns the text stored in the clipboard.
		std::string get_clipboard_text() const;

		/// \brief Returns an image stored in the clipboard.
		/// <p>Returns a null pixelbuffer if no image is available.</p>
		PixelBuffer get_clipboard_image() const;

		/// \brief Returns the minimum size the window can be resized to by the application user.
		Sizef get_minimum_size(bool client_area = false);

		/// \brief Returns the maximum size the window can be resized to by the application user.
		Sizef get_maximum_size(bool client_area = false);

		/// \brief Returns the window title.
		std::string get_title() const;

		/** Returns an platform-specific internal display window handle object.
		 */
		DisplayWindowHandle get_handle() const;

		/// \brief Convert from window client coordinates to screen coordinates.
		Pointf client_to_screen(const Pointf &client);

		/// \brief Convert from screen coordinates to client coordinates.
		Pointf screen_to_client(const Pointf &screen);

		/// \brief Capture/Release the mouse.
		void capture_mouse(bool capture);

		/// \brief Invalidates the screen, causing a repaint.
		void request_repaint();

		/// \brief Change window title.
		void set_title(const std::string &title);

		/// \brief Set window position and size.
		///
		/// \param pos = Window position and size.
		/// \param client_area = true - Position relative to window client area
		void set_position(const Rectf &pos, bool client_area);

		/// \brief Set window position.
		///
		/// \param x Window x position on desktop.
		/// \param y Window y position on desktop.
		void set_position(float x, float y);

		/// \brief Set enabled
		///
		/// \param enable = bool
		void set_enabled(bool enable);

		/// \brief Set visible
		///
		/// \param visible = bool
		/// \param activate = bool
		void set_visible(bool visible, bool activate);

		/// \brief Resize window.
		///
		/// \param width = New width of window in pixels.
		/// \param height = New height of window in pixels.
		/// \param client_area = true - relative to the window client area
		void set_size(float width, float height, bool client_area);

		/// \brief Minimum size a window can be resized to by the application user.
		void set_minimum_size(float width, float height, bool client_area);

		/// \brief Maximum size a window can be resized to by the application user.
		void set_maximum_size(float width, float height, bool client_area);

		/// \brief Minimizes the window.
		void minimize();

		/// \brief Restores the window.
		void restore();

		/// \brief Maximizes the window.
		void maximize();

		/// Toggle fullscreen
		/// Only Win32 implementation for now
		void toggle_fullscreen();

		/// \brief Displays the window in its current size and position.
		void show(bool activate = true);

		/// \brief Hides the window.
		void hide();

		/// \brief Raises the window on top of other windows.
		void bring_to_front();

		/// \brief Flip back buffer to front, making changes visible on screen.
		///
		/// <p>The parameter interval specifies the minimum number of video frames
		/// that are displayed before a buffer swap will occur.</p>
		/// <p>If interval is set to a value of 0, buffer swaps are not synchronized
		/// to a video frame. </p>
		/// <p>If interval is set to a value of -1 (the default), then it will use
		/// the buffer swap used for previous flip. If its the first flip, it will
		/// use the system default.</p>
		///
		/// \param interval = See note
		void flip(int interval = -1);

		/// \brief Shows the mouse cursor.
		void show_cursor();

		/// \brief Sets the current cursor icon.
		void set_cursor(const Cursor &cursor);

		/// \brief Set cursor
		///
		/// \param type = Standard Cursor
		void set_cursor(StandardCursor type);

#ifdef WIN32
		/// \brief Sets the current cursor handle (win32 only)
		void set_cursor_handle(HCURSOR cursor);
#endif

		/// \brief Hides the mouse cursor.
		void hide_cursor();

		/// \brief Stores text in the clipboard.
		void set_clipboard_text(const std::string &text);

		/// \brief Stores an image in the clipboard.
		void set_clipboard_image(const PixelBuffer &buf);

		/// \brief Sets the large icon used for this window.
		void set_large_icon(const PixelBuffer &image);

		/// \brief Sets the small icon used for this window.
		void set_small_icon(const PixelBuffer &image);

		/// \brief Enable alpha channel for this window.
		///
		/// This is only supported on Windows Vista and above (Else use Layered windows instead)
		/// \param blur_rect = Blur rectangle. If size = 0, then the entire window is used
		void enable_alpha_channel(const Rectf &blur_rect);

		/// \brief Exend the window frame into the client area
		///
		/// Only implemented on win32
		void extend_frame_into_client_area(float left, float top, float right, float bottom);

	private:
		std::shared_ptr<DisplayWindow_Impl> impl;
	};

	/// \}
}
