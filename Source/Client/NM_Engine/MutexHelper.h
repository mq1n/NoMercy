#pragma once
#include <windows.h> 
#include <string>

class CLimitSingleInstance
{
	public:
		CLimitSingleInstance() = default;
		CLimitSingleInstance(const std::string & szMutexName) :
			m_szMutexName(szMutexName)
		{
		}   
		~CLimitSingleInstance() = default;

		bool CreateInstance()
		{
			SECURITY_ATTRIBUTES sa	= { 0 };
			sa.nLength				= sizeof(sa);

			m_hMutex = CreateMutexA(&sa, FALSE, m_szMutexName.c_str());
			if (!m_hMutex || m_hMutex == INVALID_HANDLE_VALUE)
				return false;

			return true;
		}

		void CloseInstance()
		{
			if (m_hMutex)
			{
				CloseHandle(m_hMutex);
				m_hMutex = NULL;
			}
		}

		bool IsAnotherInstanceRunning() 
		{
			CreateInstance();

			auto dwLastError = GetLastError();

			CloseInstance();
			return (ERROR_ALREADY_EXISTS == dwLastError);
		}

		bool ProtectInstance()
		{
			if (!m_hMutex || m_hMutex == INVALID_HANDLE_VALUE)
				return false;
			return SetHandleInformation(m_hMutex, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE);
		}

	private:
		std::string m_szMutexName;
		HANDLE		m_hMutex;
};
