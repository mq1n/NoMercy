#pragma once
#include "Timer.h"

class CWatchdog
{
	public:
		CWatchdog();
		~CWatchdog() = default;

		size_t						GetWatchdogCount();
		bool						IsWatchdogWindow(HWND hWnd);
		std::map <HWND, WNDPROC>	GetWindowBackups();

		void SetInitilizationStatus(bool bNewStat)	{ m_bInitialized = bNewStat; };
		bool GetInitilizationStatus()				{ return m_bInitialized; };

		bool PreCheckLoadedWatchdogs();
		bool CheckLoadedWatchdogs();
		bool LoadWatchdog();
		bool InitializeWatchdog();

	private:
		mutable std::recursive_mutex		m_mutex;

		bool								m_bInitialized;

		CTimer <std::chrono::milliseconds>	m_watchdogTimer;
		CTimer <std::chrono::milliseconds>	m_tickCheckTimer;

		std::map <HWND, WNDPROC>			m_windowBackupMap;

		int									m_iWatchDogCheckCount;
};
