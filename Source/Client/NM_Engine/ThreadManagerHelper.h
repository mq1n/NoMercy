#pragma once
#include <map>
#include "ThreadHelper.h"

enum ESelfThreads
{
	SELF_THREAD_NONE,
	SELF_THREAD_DIRFUNCS,
	SELF_THREAD_ANTIMACRO,
	SELF_THREAD_ENUMPROCHANDLE,
	SELF_THREAD_DECREASEPRIVS,
	SELF_THREAD_WATCHDOG,
	SELF_THREAD_WMI,
	SELF_THREAD_WINDOWCHECK,
	SELF_THREAD_WINDOWHOOKENUM,
	SELF_THREAD_SECTIONCHECKSUM,
	SELF_THREAD_NMMAINROUTINE,
	SELF_THREAD_MMAPMODULES,
	SELF_THREAD_THREADTICKCHECKER,
	SELF_THREAD_NETWORKROUTINE,
	SELF_THREAD_NET_QUEUEWORKER,
	SELF_THREAD_SHADOW_INITIALIZER,
	SELF_THREAD_TIMER_CHECKER,
	SELF_THREAD_ANTI_SOFTBP,
	SELF_THREAD_MAX
};

typedef struct _self_threads
{
	int							iThreadCode;
	DWORD_PTR					dwThreadAddress;
	HANDLE						hThread;
	std::shared_ptr <CThread>	customThread;
	DWORD						dwThreadId;
	DWORD						dwMaxDelay;
	bool						bIsTemporaryThread;
	std::size_t					ulFuncSize;
	DWORD						ulFuncHash;
} SSelfThreads, *PSelfThreads;

class CThreadManager
{
	public:
		CThreadManager();
		~CThreadManager();

		std::shared_ptr<CThread> CreateCustomThread(int iThreadCode, LPTHREAD_START_ROUTINE pFunc, std::size_t ulFuncSize, LPVOID lpParam, DWORD dwMaxDelay, bool bIsTemporaryThread);
		bool DestroyThread(const std::shared_ptr<CThread> & thread);
		void OnThreadTerminated(DWORD dwThreadId);

		std::size_t GetThreadFuncSize(LPBYTE lpFunc);

		const std::vector < std::shared_ptr <SSelfThreads> > & GetThreadList();

		std::shared_ptr <CThread> GetThreadFromThreadCode(int iThreadCode);
		std::shared_ptr <CThread> GetThreadFromId(DWORD dwThreadId);
		std::shared_ptr <CThread> GetThreadFromAddress(DWORD dwThreadAddress);
		std::shared_ptr <CThread> GetThreadFromHandle(HANDLE hThread);
		std::shared_ptr <SSelfThreads> GetThreadInfo(int iThreadCode);

		std::size_t		GetThreadCount();
		int				GetSuspendedThreadCount();
		bool			HasSuspendedThread();

		bool SetAntiTrace(const std::shared_ptr <CThread> & targetThread);

	private:
		std::vector < std::shared_ptr <SSelfThreads> >			m_vThreadList;
		std::map <DWORD, /* dwThreadId */ HANDLE /* hThread */>	m_vSelfThreads;
};

