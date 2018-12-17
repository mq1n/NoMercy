#pragma once

class CThreadFunctions
{
	public:
		static DWORD	GetThreadOwnerProcessId(DWORD dwThreadID);
		static DWORD	GetThreadStartAddress(HANDLE hThread);
		static DWORD	GetMainThreadId();
		static DWORD	GetThreadIdFromAddress(DWORD dwAddress);
		static bool		ThreadIsItAlive(DWORD dwThreadID);
		static HANDLE	CreateThread(int iCustomThreadCode, LPTHREAD_START_ROUTINE pFunc, LPVOID lpParam, LPDWORD pdwThreadId);
		static DWORD	GetThreadID(HANDLE hThread);
		static bool		ChangeThreadsStatus(bool bSuspend);
};

