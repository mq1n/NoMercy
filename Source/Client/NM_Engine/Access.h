#pragma once
#include <Windows.h>
#include <vector>

class CAccess
{
	public:
		CAccess() = default;
		~CAccess() = default;

		BOOL IsRunAsAdmin();
		BOOL IsProcessElevated();
		DWORD GetIntegrityLevel(HANDLE hTarget);

		bool BlockAccess(HANDLE hTarget);
		bool DenyProcessAccess();
		bool DenyThreadAccess(HANDLE hThread);
		bool SetMitigationPolicys();

		bool RemoveProcessDebugPriv(DWORD dwProcessId, HANDLE hProcess);
		bool DecreasePrivilege(HANDLE hProcess);

		bool EnablePermanentDep();
		bool EnableNullPageProtection();

		bool EnableDebugPrivileges();

	private:
		std::vector <DWORD> m_vBlockedProcessIds;
		std::vector <DWORD> m_vUnopenedProcessIds;
};
