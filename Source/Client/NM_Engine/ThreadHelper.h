#pragma once

class CThread
{
	public:
		CThread() = default;
		CThread(DWORD dwThreadId);
		CThread(HANDLE hThread);
		CThread(HANDLE hThread, int iThreadCode);
		CThread(HANDLE hThread, DWORD dwThreadId, int iThreadCode);
		~CThread();

		void		Terminate();
		void		ClearDebugRegisters();
		void		Wait(DWORD dwMSDelay);

		void		SetPriority(int iPriority);
		void		SetRoutineCompleted(bool bCompleted);

		HANDLE		GetHandle();
		DWORD		GetId();
		int			GetCustomCode();
		std::string GetThreadCustomName();
		DWORD		GetStartAddress();
		int			GetPriority();
		DWORD		GetProcessId();
		std::shared_ptr<CONTEXT>	GetContext();
		DWORD		GetModuleBaseAddress();
		std::size_t	GetModuleSize();

		std::string GetThreadOwnerFullName();
		std::string GetThreadOwnerFileName();

		bool		IsOpenedThread();
		bool		IsCorrect();
		bool		IsItAlive();
		bool		IsRemoteThread();
		bool		IsSelfThread();
		bool		IsGoodPriority();
		bool		IsRoutineCompleted();

		bool		HasSuspend();
		bool		HasDebugRegisters();

		bool		TrySuspend();
		bool		TryResume();

	private:
		DWORD		m_dwThreadId;
		HANDLE		m_hThread;
		int			m_iThreadCode;
		bool		m_bOpenedThread;
		bool		m_bRoutineCompleted;
};

