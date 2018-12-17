#pragma once
#include "Windows_Structs.h"

class CThreadEnumerator
{
	public:
		CThreadEnumerator(DWORD dwProcessId);
		~CThreadEnumerator();

		LPVOID GetProcInfo();
		LPVOID GetThreadList(LPVOID procInfo);
		DWORD  GetThreadCount(LPVOID procInfo);

		LPVOID FindThread(LPVOID procInfo, DWORD dwThreadId);

	protected:
		BYTE * InitializeQuery();

	private:
		DWORD  m_dwProcessId;
		BYTE * m_Cap;
};

