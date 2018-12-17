#include "NM_Index.h"
#include "NM_Main.h"
#include "Data.h"

CData::CData(EAppTypes eAppType)
{
	Initialize();

	if (eAppType == NM_SHADOW)
		m_iGameCode = NM_SHADOW;

	m_pAntiModuleInfo = std::make_shared<ANTI_MODULE_INFO>();
}

CData::~CData()
{
	Initialize();

	m_pAntiModuleInfo.reset();
	m_pAntiModuleInfo = nullptr;
}

void CData::Initialize()
{
	m_bWatchdogFirstChecked = false;

	m_hAntiModule			= nullptr;

	m_szLicenseCode			= "";
	m_cpLicensedIPs			= nullptr;
	m_szLicenseCount		= NULL;

	m_hInstance				= nullptr;

	m_szPatcherName			= "";

	m_iGameCode				= NULL;

	m_hPythonHandle			= nullptr;
	m_szPythonName			= "";

	m_bIsPacked				= false;

	m_bIsShadow				= false;
	m_hShadowProcess		= INVALID_HANDLE_VALUE;

	m_dwMainThreadId		= 0;
}

#pragma optimize("", off )
void CData::SetWatchdogFirstCheck()
{
	m_bWatchdogFirstChecked = true;
}
bool CData::WatchdogIsFirstChecked()
{
	return m_bWatchdogFirstChecked;
}

void CData::SetAntiModuleInformations(LPCVOID lpModuleInfo)
{
	if (lpModuleInfo)
	{
		PANTI_MODULE_INFO p = (PANTI_MODULE_INFO)lpModuleInfo;
		memcpy(m_pAntiModuleInfo.get(), p, sizeof(ANTI_MODULE_INFO));
	}
}
LPVOID CData::GetAntiModuleInformations()
{
	auto lpAntiInfo = (LPVOID)m_pAntiModuleInfo.get();
	return lpAntiInfo;
}
HMODULE	CData::GetAntiModule()
{
	return m_hAntiModule;
}

std::string CData::GetAntiFileName()
{
	if (IS_VALID_SMART_PTR(m_pAntiModuleInfo) && m_pAntiModuleInfo->BaseDllName.Length && m_pAntiModuleInfo->BaseDllName.Buffer)
	{
		auto wszFileName	= std::wstring(m_pAntiModuleInfo->BaseDllName.Buffer);
		auto szFileName		= std::string(wszFileName.begin(), wszFileName.end());
		return szFileName;
	}
	return std::string("");
}

std::string CData::GetAntiFullName()
{
	if (IS_VALID_SMART_PTR(m_pAntiModuleInfo) && m_pAntiModuleInfo->FullDllName.Length && m_pAntiModuleInfo->FullDllName.Buffer)
	{
		auto wszFullName	= std::wstring(m_pAntiModuleInfo->FullDllName.Buffer);
		auto szFullName		= std::string(wszFullName.begin(), wszFullName.end());
		return szFullName;
	}
	return std::string("");
}


std::string CData::GetLicenseCode()
{
	return m_szLicenseCode;
}
void CData::SetLicenseCode(std::string tszLicenseCode)
{
	m_szLicenseCode = tszLicenseCode;
}

char** CData::GetLicensedIPArray()
{
	return m_cpLicensedIPs;
}
size_t CData::GetLicensedIPCount()
{
	return m_szLicenseCount;
}
void CData::SetLicensedIPs(char* cIpList[], int iIpCount)
{
	m_cpLicensedIPs = cIpList;
	m_szLicenseCount = iIpCount;
}

void CData::SetMainWnd(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
}
HINSTANCE CData::GetMainWnd()
{
	return m_hInstance;
}

std::string	CData::GetPatcherName()
{
	return m_szPatcherName;
}
void CData::SetPatcherName(std::string tszPatcherName)
{
	m_szPatcherName = tszPatcherName;
}

int CData::GetAppType()
{
	return m_iAppType;
}
void CData::SetAppType(int iCode)
{
	m_iAppType = iCode;
}

int CData::GetGameCode()
{
	return m_iGameCode;
}
void CData::SetGameCode(int iCode)
{
	m_iGameCode = iCode;
}

HMODULE CData::GetPythonHandle()
{
	return m_hPythonHandle;
}
void CData::SetPythonHandle(HMODULE hModule)
{
	m_hPythonHandle = hModule;
}

std::string CData::GetPythonName()
{
	return m_szPythonName;
}
void CData::SetPythonName(std::string szName)
{
	m_szPythonName = szName;
}

bool CData::IsPackedProcess()
{
	return m_bIsPacked;
}
void CData::SetPackedProcess(bool bRet)
{
	m_bIsPacked = bRet;
}

bool CData::IsShadowInitialized()
{
	return m_bIsShadow;
}
void CData::SetShadowInitialized(bool bRet)
{
	m_bIsShadow = bRet;
}

HANDLE CData::GetShadowProcessHandle()
{
	return m_hShadowProcess;
}
void CData::SetShadowProcessHandle(HANDLE hProcess)
{
	m_hShadowProcess = hProcess;
}

DWORD CData::GetMainThreadId()
{
	return m_dwMainThreadId;
}
void CData::SetMainThreadId(DWORD dwThreadId)
{
	m_dwMainThreadId = dwThreadId;
}
#pragma optimize("", on )
