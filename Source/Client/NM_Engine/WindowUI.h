#pragma once
#include <Windows.h>

#include "BasicTypesUI.h"
#include "WidgetUI.h"
#include "ProgressBarUI.h"
#include "resource.h"

#include <functional>
#include <unordered_map>

namespace gui
{
	WPARAM execute();

	class window : public widget
	{
	public:
		window();
		~window();

		bool assemble(std::string const& class_name, std::string const& window_name, rectangle rect, const char* icon_name);

		static window* singleton()
		{
			static window* singleton = new window;
			return singleton;
		}

		auto GetProgressBarInstance() { return progress; };

	protected:
		bool create_class(std::string const& class_name, const char* icon_name);
		bool create_window(std::string const& class_name, std::string const& window_name, rectangle& rect);

		void set_message_handlers();

	private:
		progressbar* progress;

		HFONT paint_font;
		HBITMAP background;
	};
}