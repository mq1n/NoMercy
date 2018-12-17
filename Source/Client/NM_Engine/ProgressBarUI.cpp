#include "NM_Index.h"
#include "NM_Main.h"
#include "ProgressBarUI.h"

namespace gui
{
	progressbar::progressbar(HWND hwnd_parent, HINSTANCE instance)
		: widget(hwnd_parent, instance, true)
	{

	}

	bool progressbar::assemble(rectangle rect, std::size_t minimum, std::size_t maximum)
	{
		if (!this->create(PROGRESS_CLASSA, "", rect, WS_VISIBLE | WS_CHILD | PBS_SMOOTH, NULL, NULL))
		{
			return false;
		}

		SendMessage(this->get_handle(), PBM_SETSTATE, static_cast<LPARAM>(progressbar_state::green), 0);
		SendMessage(this->get_handle(), PBM_SETRANGE32, minimum, maximum);
		SendMessage(this->get_handle(), PBM_SETPOS, 0, 0);
		return true;
	}

	void progressbar::set_position(unsigned int position)
	{
		SendMessage(this->get_handle(), PBM_SETPOS, position, 0);
	}

	void progressbar::set_state(progressbar_state state)
	{
		SendMessage(this->get_handle(), PBM_SETSTATE, static_cast<LPARAM>(state), 0);
	}
}