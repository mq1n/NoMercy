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
#include "../../Core/Math/point.h"
#include "window_controller.h"

namespace clan
{
	class View;
	class WindowManagerImpl;

	/// Manages one or more windows
	class WindowManager
	{
	public:
		WindowManager();
		~WindowManager();

		/// Notifices RunLoop to exit when last presented window is dismissed
		void set_exit_on_last_close(bool enable = true);

		/// Shows a main window
		void present_main(const std::shared_ptr<WindowController> &controller);

		template<typename T, typename... Types>
		std::shared_ptr<T> present_main(Types &&... args)
		{
			auto controller = std::make_shared<T>(std::forward<Types>(args)...);
			present_main(controller);
			return controller;
		}

		/// Shows a modal dialog
		void present_modal(View *owner, const std::shared_ptr<WindowController> &controller);

		template<typename T, typename... Types>
		std::shared_ptr<T> present_modal(View *owner, Types &&... args)
		{
			auto controller = std::make_shared<T>(std::forward<Types>(args)...);
			present_modal(owner, controller);
			return controller;
		}

		/// Shows a popup window
		void present_popup(View *owner, const Pointf &pos, const std::shared_ptr<WindowController> &controller);

		template<typename T, typename... Types>
		std::shared_ptr<T> present_popup(View *owner, const Pointf &pos, Types &&... args)
		{
			auto controller = std::make_shared<T>(std::forward<Types>(args)...);
			present_popup(owner, controller);
			return controller;
		}

	private:
		WindowManager(const WindowManager &) = delete;
		WindowManager &operator=(const WindowManager &) = delete;

		std::unique_ptr<WindowManagerImpl> impl;
		friend class WindowController;
	};
}
