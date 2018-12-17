#pragma once

class CSafeProcessHandle
{
	public:
		CSafeProcessHandle(DWORD dwAccess = MAXIMUM_ALLOWED);
		~CSafeProcessHandle();

		HANDLE					FindProcessFromPID(DWORD dwProcessID, bool bAliveOnly = false);
		std::vector <HANDLE> 	EnumerateProcesses(bool bAliveOnly = false);

	protected:
		void	CloseUselessHandles();

	private:
		DWORD					m_dwAccess;
		std::vector <HANDLE>	m_vHandleList;
};

