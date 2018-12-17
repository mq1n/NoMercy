#pragma once

#include <Windows.h>
#include "BasicTypesUI.h"

#include <functional>
#include <unordered_map>

namespace gui
{
	typedef std::function<int(HWND, WPARAM, LPARAM)> message_function;
	typedef std::pair<unsigned int, message_function> message_pair;

	class widget
	{
	public:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static std::unordered_map<HWND, widget*> widget_list;

		widget(HWND hwnd_parent = NULL, HINSTANCE instance = NULL, bool is_window = false);
		~widget();

		bool create(std::string const& class_name, std::string const& widget_name, rectangle& rect, 
			unsigned int style, unsigned int ex_style = 0, unsigned int menu_handle = 0);

		void add_message_handlers(int count, ...);
		void remove_message_handler(unsigned int message);

		bool set_text(std::string const& text);

		HWND get_handle();

		void set_instance(HINSTANCE instance);
		HINSTANCE get_instance();

	private:
		std::unordered_map<unsigned int, message_function> custom_messages;

		HFONT font;

		HWND hwnd;
		HWND hwnd_parent;

		HINSTANCE instance;

		WNDPROC original_wndproc;

		bool is_window;
	};
}