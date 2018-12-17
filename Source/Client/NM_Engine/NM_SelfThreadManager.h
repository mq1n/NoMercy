#pragma once
#include "Defines.h"

class CSelfThreadManager
{
	public:
		CSelfThreadManager();
		~CSelfThreadManager() = default;

	public:
		DWORD GetLastCheckTime(DWORD dwThreadCode);
		void SetLastCheckTime(DWORD dwThreadCode, DWORD dwTime);

		void IncreaseThreadTick(DWORD dwThreadCode);
		void DecreaseThreadTick(DWORD dwThreadCode);
		void ReleaseThreadTicks(DWORD dwThreadCode);
		DWORD GetThreadTick(DWORD dwThreadCode);

		bool	InitThreadTickChecker();
		bool	IsTickCheckerThreadIntegrityCorrupted();
		void	CheckSelfThreads();

	protected:
		__forceinline bool CheckThread(HANDLE hThread, LPDWORD pdwErrorCode);

	private:
		std::map <DWORD, DWORD> m_threadTimeMap;
		std::map <DWORD, DWORD> m_threadTicksMap;
};

