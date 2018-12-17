#pragma once

inline FARPROC WINAPI CustomGetProcAddress(const HMODULE hModule, PCSTR lpProcName);

typedef struct _MAPPED_MODULE_INFO
{
	HMODULE hModule;
	char	szModuleFullName[MAX_PATH];
	char	szModuleBaseName[MAX_PATH];
	DWORD	dwMappedBase;
	bool	bOptional;
} SMappedModuleInfo, *PMappedModuleInfo;

class CManualMap
{
	public:
		CManualMap();
		~CManualMap() = default;

		std::shared_ptr <SMappedModuleInfo> GetMappedModuleInformations(HMODULE hTargetModule);
		bool ManualMapModule(const std::string & szFullDllName, const std::string & szBaseDllName, bool bRun, LPDWORD pdwBase, ULONG_PTR pDllBase);
		bool MapSystemModules();
		bool RefreshModules();

	protected:
		bool ScanAndFixSection(PCHAR szSectionName, PVOID pKnown, PVOID pSuspect, SIZE_T stLength);
		bool ScanAndFixModule(PVOID pKnown, PVOID pSuspect, const std::string & szBaseDllName);

	private:
		std::vector < std::shared_ptr <SMappedModuleInfo> > m_vMappedModuleList;
};

