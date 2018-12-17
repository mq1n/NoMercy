#include "NM_Index.h"
#include "NM_Main.h"
#include "WidgetUI.h"

namespace gui
{
	std::unordered_map<HWND, widget*> widget::widget_list;

	LRESULT CALLBACK widget::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		widget* current_widget = widget::widget_list[hWnd];

		if (current_widget)
		{
			if (current_widget->custom_messages[message])
			{
				return current_widget->custom_messages[message](hWnd, wParam, lParam);
			}
			else
			{
				if (current_widget->is_window)
				{
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				else
				{
					return CallWindowProc(current_widget->original_wndproc, hWnd, message, wParam, lParam);
				}
			}
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	widget::widget(HWND hwnd_parent, HINSTANCE instance, bool is_window) 
		: hwnd_parent(hwnd_parent), is_window(is_window), font(0)
	{
		this->instance = (instance != NULL ? instance : reinterpret_cast<HINSTANCE>(GetWindowLong(this->hwnd_parent, GWLP_HINSTANCE)));
	}

	widget::~widget()
	{
		if (this->hwnd)
		{
			this->widget_list.erase(this->hwnd);
			DestroyWindow(this->hwnd);
		}

		if (this->font)
		{
			DeleteObject(this->font);
		}
	}

	bool widget::create(std::string const& class_name, std::string const& widget_name, rectangle& rect, unsigned int style, unsigned int ex_style, unsigned int menu_handle)
	{
		this->hwnd = CreateWindowExA(ex_style, class_name.c_str(), widget_name.c_str(), style, rect.get_x(), rect.get_y(), rect.get_width(), rect.get_height(), this->hwnd_parent, reinterpret_cast<HMENU>(menu_handle), this->instance, NULL);

		if (!this->hwnd)
		{
			return false;
		}

		this->font = CreateFontA(13, 0, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, 5, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");

		SendMessage(this->hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(this->font), TRUE);

		if (!this->is_window)
		{
			this->original_wndproc = reinterpret_cast<WNDPROC>(SetWindowLong(this->hwnd, GWLP_WNDPROC, reinterpret_cast<long>(widget::WndProc)));
		}

		this->widget_list[this->hwnd] = this;
		return true;
	}

	void widget::add_message_handlers(int count, ...)
	{
		va_list va;
		va_start(va, count);

		for (int i = 0; i < count; i++)
		{
			message_pair message = va_arg(va, message_pair);
			this->custom_messages[message.first] = message.second;
		}

		va_end(va);
	}

	void widget::remove_message_handler(unsigned int message)
	{
		this->custom_messages.erase(message);
	}

	bool widget::set_text(std::string const& text)
	{
		return (SetWindowTextA(this->hwnd, text.c_str()) != FALSE);
	}

	HWND widget::get_handle()
	{
		return this->hwnd;
	}

	void widget::set_instance(HINSTANCE instance)
	{
		this->instance = instance;
	}

	HINSTANCE widget::get_instance()
	{
		return this->instance;
	}
}