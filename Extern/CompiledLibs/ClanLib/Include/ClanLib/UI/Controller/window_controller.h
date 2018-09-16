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

#include <memory>
#include "../../Core/Signals/signal.h"

namespace clan
{
	class View;
	class WindowControllerImpl;

	/// Base class for controllers managing windows
	class WindowController
	{
	public:
		WindowController();
		virtual ~WindowController();

		/// Returns the root view
		const std::shared_ptr<View> &root_view() const;

		/// Changes the root view
		void set_root_view(std::shared_ptr<View> root_view);

		/// Returns the window title
		const std::string &title() const;

		/// Sets the window title
		void set_title(const std::string &title);

		/// Sets the size of the window
		void set_frame_size(const Sizef &size, bool resizable = true);

		/// Sets the size of the window
		void set_content_size(const Sizef &size, bool resizable = true);

		/// Sets the icon used for the window
		void set_icon(const std::vector<std::string> &icon_images);

		/// Closes the window
		void dismiss();

		/// Slot container helping with automatic disconnection of connected slots when the controller is destroyed
		SlotContainer slots;

	private:
		WindowController(const WindowController &) = delete;
		WindowController &operator=(const WindowController &) = delete;

		std::unique_ptr<WindowControllerImpl> impl;
		friend class WindowManager;
	};
}
