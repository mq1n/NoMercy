#pragma once
#include <deque>

template <typename T>
class CQueue
{
	typedef std::function<void(T)> TCustomCallback;

	public:
		CQueue() : m_pCallback(nullptr) { }
		~CQueue() = default;

		void BindCallback(TCustomCallback pFunc)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);
			
			m_pCallback(std::move(pFunc));
		}

		bool HasWorkObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			return m_kWorkList.size() > 0;
		}

		template <class cData>
		bool InsertObject(cData data)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			// If already queue'd object, dont add duplicate
			if (m_kWorkList.size() > 0 && std::find(m_kWorkList.begin(), m_kWorkList.end(), data) != m_kWorkList.end())
				return false;

			if (m_kCheckedList.size() > 0 && std::find(m_kCheckedList.begin(), m_kCheckedList.end(), data) != m_kCheckedList.end())
				return false;

			// Add to queue
			m_kWorkList.push_back(data);
			return true;
		}

		void ProcessFirstObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

#ifdef _DEBUG
			if (!m_pCallback)
			{
//				g_nmApp->LogHelperInstance()->sys_log(__FUNCTION__, LL_CRI, "Null callback ptr!!!");
				return;
			}
#endif

			if (!m_kWorkList.empty())
			{
				m_pCallback(m_kWorkList.front());
				m_kCheckedList.push_back(m_kWorkList.front());
				m_kWorkList.pop_front();
			}
		}

		auto GetFirstObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			return m_kWorkList.front();
		}

		void RemoveFirstObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			m_kCheckedList.push_back(m_kWorkList.front());
			m_kWorkList.pop_front();
		}

		template <class cData>
		auto HasListed(cData data)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			if (m_kWorkList.size() > 0 && std::find(m_kWorkList.begin(), m_kWorkList.end(), data) != m_kWorkList.end())
				return true;

			return false;
		}

		template <class cData>
		auto HasProcessed(cData data)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			if (m_kCheckedList.size() > 0 && std::find(m_kCheckedList.begin(), m_kCheckedList.end(), data) != m_kCheckedList.end())
				return true;

			return false;
		}

	private:
		mutable std::recursive_mutex m_Mutex;

		std::deque <T> m_kWorkList;
		std::deque <T> m_kCheckedList;

		TCustomCallback m_pCallback;
};

// --- network

typedef struct _cheat_detect_queue_data_struct
{
	DWORD	dwViolation;
	DWORD	dwErrorCode;
	char	szMessage[4096];
}SCheatDetectQueueData, *PCheatDetectQueueData;

class CCheatDetectQueue : public CQueue < std::shared_ptr <SCheatDetectQueueData> >
{
	public:
		CCheatDetectQueue() = default;
		~CCheatDetectQueue() = default;
};

// --- hook

typedef struct _thread_hook_scan_queue_struct
{
	DWORD	dwThreadID;
	LPVOID	lpStartAddress;
	LPVOID	lpEIP;
}SThreadHookQueueData, *PThreadHookQueueData;

typedef struct _module_hook_scan_queue_struct
{
	std::wstring	wszModuleName;
	DWORD			dwThreadID;
	int				iCheckType;
}SModuleHookQueueData, *PModuleHookQueueData;

typedef struct _module_req_hook_scan_queue_struct
{
	std::wstring wszModuleName;
}SModuleReqHookQueueData, *PModuleReqHookQueueData;

typedef struct _section_hook_scan_queue_struct
{
	DWORD dwBase;
	LPVOID lpArbitraryUserPointer;
}SSectionHookQueueData, *PSectionHookQueueData;

typedef struct _wndproc_hook_scan_queue_struct
{
	int iType;
	HWND hWnd;
	int nIndex;
	LONG dwNewLong;
}SWndProcHookQueueData, *PWndProcHookQueueData;
