#pragma once

typedef VOID(NTAPI* TThreadExitCallbackTemplate)(PVOID, BOOLEAN);

class CThreadExitWatcher
{
	public:
		CThreadExitWatcher() = default;
		~CThreadExitWatcher() = default;

		bool InitializeExitCallback(HANDLE hThread, TThreadExitCallbackTemplate pCallback, DWORD dwTimeout = INFINITE, PVOID pContext = nullptr);

	private:
		HANDLE m_hThread;
		HANDLE m_hWaitObj;
		LPVOID m_pCallback;
};
