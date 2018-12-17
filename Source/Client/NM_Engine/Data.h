#pragma once
#include "Windows_Structs.h"
extern enum EAppTypes appTypes;

class CData : public std::enable_shared_from_this <CData>
{
	public:
		CData(EAppTypes eAppType);
		~CData();

		void			SetWatchdogFirstCheck();
		bool			WatchdogIsFirstChecked();

		void			SetAntiModuleInformations(LPCVOID lpModuleInfo);
		LPVOID			GetAntiModuleInformations();
		HMODULE			GetAntiModule();

		std::string		GetAntiFileName();
		std::string		GetAntiFullName();

		std::string		GetLicenseCode();
		void			SetLicenseCode(std::string tszLicenseCode);

		char**			GetLicensedIPArray();
		size_t			GetLicensedIPCount();
		void			SetLicensedIPs(char* cIpList[], int iIpCount);

		void			SetMainWnd(HINSTANCE hInstance);
		HINSTANCE		GetMainWnd();

		std::string		GetPatcherName();
		void			SetPatcherName(std::string tszPatcherName);

		int				GetAppType();
		void			SetAppType(int iCode);

		int				GetGameCode();
		void			SetGameCode(int iCode);

		HMODULE			GetPythonHandle();
		void			SetPythonHandle(HMODULE hModule);

		std::string		GetPythonName();
		void			SetPythonName(std::string szName);

		bool			IsPackedProcess();
		void			SetPackedProcess(bool bRet);

		bool			IsShadowInitialized();
		void			SetShadowInitialized(bool bRet);

		HANDLE			GetShadowProcessHandle();
		void			SetShadowProcessHandle(HANDLE hProcess);

		DWORD			GetMainThreadId();
		void			SetMainThreadId(DWORD dwThreadId);

	protected:
		void			Initialize();

	private:
		bool			m_bWatchdogFirstChecked;

		std::shared_ptr <ANTI_MODULE_INFO>	m_pAntiModuleInfo;
		HMODULE								m_hAntiModule;

		std::string		m_szLicenseCode;
		char**			m_cpLicensedIPs;
		size_t			m_szLicenseCount;

		HINSTANCE		m_hInstance;

		std::string		m_szPatcherName;

		int				m_iAppType;
		int				m_iGameCode;

		HMODULE			m_hPythonHandle;
		std::string		m_szPythonName;

		bool			m_bIsPacked;

		bool			m_bIsShadow;
		HANDLE			m_hShadowProcess;

		DWORD			m_dwMainThreadId;
};

