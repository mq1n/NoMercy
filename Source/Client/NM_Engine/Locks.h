#pragma once
#include <Windows.h>
#include <cstdint>

class CLock
{
	public:
		virtual ~CLock() {}

		virtual void Lock() = 0;
		virtual void TryLock() = 0;
		virtual void Unlock() = 0;
};

class CCSLock : public CLock
{
	public:
		CCSLock(uint32_t uSpinCount = 0)
		{
			if (uSpinCount)
				InitializeCriticalSectionAndSpinCount(&m_cs, uSpinCount);
			else
				InitializeCriticalSection(&m_cs);
		}
		~CCSLock()				{ DeleteCriticalSection(&m_cs);		}

		virtual void Lock()		{ EnterCriticalSection(&m_cs);		}
		virtual void TryLock()	{ TryEnterCriticalSection(&m_cs);	}
		virtual void Unlock()	{ LeaveCriticalSection(&m_cs);		}

	private:
		CRITICAL_SECTION m_cs;
};

class CScopedLock
{
	public:
		CScopedLock(CLock & lock) :
			m_pLockPtr(&lock)
		{
			m_pLockPtr->Lock();
		}

		~CScopedLock()
		{
			m_pLockPtr->Unlock();
		}

	private:
		CLock * m_pLockPtr;
};

