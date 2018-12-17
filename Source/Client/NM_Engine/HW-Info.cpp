#include "NM_Index.h"
#include "NM_Main.h"
#include "HW-Info.h"
#include "NetworkAdapter.h"
#include "HddData.h"
#include "MacAddress.h"

bool CHwidManager::Initilize()
{
	auto pMacAddressMgr = std::make_unique<CMacAddress>();
	if (IS_VALID_SMART_PTR(pMacAddressMgr))
	{
		m_szMacAddress			= pMacAddressMgr->GetMacAddress();
		m_szPhysicalMacAddress	= pMacAddressMgr->GetPhysicalMacAddress();
	}

	auto pHddAdressMgr = std::make_unique<CHddData>();
	if (IS_VALID_SMART_PTR(pMacAddressMgr))
	{
		m_szHDDSerial	= pHddAdressMgr->getHDDSerialNumber();
		m_szHDDModel	= pHddAdressMgr->getHDDModelNumber();
	}

	m_szCpuID				= GenerateCpuID();
	m_szUserName			= GenerateUserName();
	m_szComputerName		= GenerateComputerName();
	m_szMachineGUIDRegistry	= GenerateGuidFromRegistry();
	m_szMachineGUIDWinapi	= GenerateGuidFromWinapi();
	m_szMachineVolumeHash	= GenerateVolumeHashFromWinapi();
	m_szNetbiosMacAddr		= GenerateMacAddressFromNetbios();

	GenerateRegistrySpecificInformations();

	if (g_winapiApiTable && g_winapiApiTable.get() && g_winapiApiTable->RtlGetVersion)
	{
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof(verInfo);
		if (g_winapiApiTable->RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
		{
			m_szBuildNumber			= std::to_string(verInfo.dwBuildNumber);
			m_szOSVersion			= std::to_string(verInfo.dwMajorVersion)	+ xorstr(".").crypt_get() + std::to_string(verInfo.dwMinorVersion);
			m_szServicePackVersion	= std::to_string(verInfo.wServicePackMajor) + xorstr(".").crypt_get() + std::to_string(verInfo.wServicePackMinor);
		}
	}

	return true;
}

static const ULONG gsc_uCpuIDValueList[] = { 0, 7, 4, 0x80000008, 0x80000007, 0x80000005, 0x80000006, 0x80000001, 0x80000000 };

inline void StringUInt32ToHexStringW(ULONG n, LPWSTR outbuf)
{
	int i = 12;
	int j = 0;

	if (outbuf == NULL)
		return;

	do
	{
		outbuf[i] = L"0123456789ABCDEF"[n % 16];
		i--;
		n = n / 16;
	} while (n > 0);

	while (++i < 13)
	{
		outbuf[j++] = outbuf[i];
	}

	outbuf[j] = 0;
}

std::string CHwidManager::GenerateCpuID()
{
	auto wszCpuID = (LPWSTR)g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(256 * sizeof(WCHAR) + sizeof(UNICODE_NULL));

	auto ulIndex = 0UL;
	for (std::size_t i = 0; i < _countof(gsc_uCpuIDValueList); i++)
	{
		int iCpuinfo[4] = { 0, 0, 0, 0 };
		__cpuid(iCpuinfo, gsc_uCpuIDValueList[i]);

		auto pBlockInfo = (LPBYTE)iCpuinfo;

		for (auto ulBlock = 0UL; ulBlock < 16; ulBlock++)
		{
			StringUInt32ToHexStringW((ULONG)pBlockInfo[ulBlock], &wszCpuID[ulIndex]);
			ulIndex += 1;
		}
	}

	wszCpuID[256] = L'\0';

	auto szHashedId = g_nmApp->DirFunctionsInstance()->GetSHA256(wszCpuID, 256);

	g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(wszCpuID);
	return szHashedId;
}

std::string CHwidManager::GenerateComputerName()
{
	auto dwComputerNameSize = 1024UL;
	char szComputerName[1024] = { 0 };

	g_winapiApiTable->GetComputerNameA(szComputerName, &dwComputerNameSize);
	return szComputerName;
}

std::string CHwidManager::GenerateUserName()
{
	auto dwUserNameSize = 1024UL;
	char szUserName[1024] = { 0 };

	g_winapiApiTable->GetUserNameA(szUserName, &dwUserNameSize);
	return szUserName;
}

std::string CHwidManager::GenerateGuidFromRegistry()
{
	HKEY hKey = NULL;
	auto res = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("SOFTWARE\\Microsoft\\Cryptography").crypt_get(), NULL, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (res == ERROR_SUCCESS)
	{
		auto dwDataType = REG_SZ;
		char szRegKey[_MAX_PATH] = { 0 };
		DWORD BufSize = _MAX_PATH;

		auto lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("MachineGuid").crypt_get(), NULL, &dwDataType, (LPBYTE)&szRegKey, &BufSize);
		if (lVal == ERROR_SUCCESS)
		{
			return szRegKey;
		}

		g_winapiApiTable->RegCloseKey(hKey);
	}
	return "";
}

std::string CHwidManager::GenerateGuidFromWinapi()
{
	char szMainDisk[MAX_PATH] = { 0 };
	auto szMainDiskRet = GetLogicalDriveStringsA(_countof(szMainDisk) - 1, szMainDisk);
	if (szMainDiskRet)
	{
		char pGuid[1024] = { 0 };
		auto bGuidRet = GetVolumeNameForVolumeMountPointA(szMainDisk, pGuid, sizeof(pGuid));
		if (bGuidRet)
		{
			std::string szGuid = pGuid;
			szGuid = szGuid.substr(11);
			szGuid = szGuid.substr(0, szGuid.size() - 2);
			return szGuid;
		}
	}
	return "";
}

std::string CHwidManager::GenerateVolumeHashFromWinapi()
{
	char szMainDisk[MAX_PATH] = { 0 };
	auto szMainDiskRet = GetLogicalDriveStringsA(_countof(szMainDisk) - 1, szMainDisk);
	if (szMainDiskRet)
	{
		auto dwSerialNum = 0UL;
		auto bVolumeInfoRet = GetVolumeInformationA(szMainDisk, nullptr, 0, &dwSerialNum, nullptr, nullptr, nullptr, 0);
		if (bVolumeInfoRet)
		{
			return std::to_string(dwSerialNum);
		}
	}
	return "";
}

std::string CHwidManager::GenerateMacAddressFromNetbios()
{
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS adapt;
		NAME_BUFFER NameBuff[30];
	} ASTAT, *PASTAT;
	ASTAT pAdapter = { 0 };

	LANA_ENUM laLanaEnum = { 0 };

	NCB ncb = { 0 };
	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (LPBYTE)&laLanaEnum;
	ncb.ncb_length = sizeof(laLanaEnum);

	auto pRet = Netbios(&ncb);
	if (pRet == NRC_GOODRET)
	{
		for (int lana = 0; lana < laLanaEnum.length; lana++)
		{
			ncb.ncb_command = NCBRESET;
			ncb.ncb_lana_num = laLanaEnum.lana[lana];
			pRet = Netbios(&ncb);
			if (pRet == NRC_GOODRET)
				break;
		}
		if (pRet == NRC_GOODRET)
		{
			memset(&ncb, 0, sizeof(ncb));

			ncb.ncb_command = NCBASTAT;
			ncb.ncb_lana_num = laLanaEnum.lana[0];
			strcpy((char*)ncb.ncb_callname, "*");
			ncb.ncb_buffer = (LPBYTE)&pAdapter;
			ncb.ncb_length = sizeof(pAdapter);

			pRet = Netbios(&ncb);
			if (pRet == NRC_GOODRET)
			{
				char szMacAddr[128];
				sprintf(szMacAddr, "%02X-%02X-%02X-%02X-%02X-%02X",
					pAdapter.adapt.adapter_address[0],
					pAdapter.adapt.adapter_address[1],
					pAdapter.adapt.adapter_address[2],
					pAdapter.adapt.adapter_address[3],
					pAdapter.adapt.adapter_address[4],
					pAdapter.adapt.adapter_address[5]);

				return szMacAddr;
			}
		}
	}
	return "";
}

void CHwidManager::GenerateRegistrySpecificInformations()
{
	HKEY hKey = NULL;
	auto res = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion").crypt_get(), NULL, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (res == ERROR_SUCCESS)
	{
		auto nBufSize = 512UL;

		char szBuffer[_MAX_PATH] = { 0 };
		auto dwDataType = REG_SZ;
		auto lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("ProductId").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szProductId = szBuffer;
		}

		auto dwBuffer = 0UL;
		dwDataType = REG_DWORD;
		lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("InstallDate").crypt_get(), NULL, &dwDataType, (LPBYTE)&dwBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szInstallDate = std::to_string(dwBuffer);
		}

		g_winapiApiTable->RegCloseKey(hKey);
	}

	res = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", NULL, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (res == ERROR_SUCCESS)
	{
		auto nBufSize = 512UL;

		char szBuffer[_MAX_PATH] = { 0 };
		auto dwDataType = REG_SZ;
		auto lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("Identifier").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szScsiId = szBuffer;
		}

		lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("SerialNumber").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szScsiSerial = szBuffer;
		}

		g_winapiApiTable->RegCloseKey(hKey);
	}

	res = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\SystemInformation", NULL, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (res == ERROR_SUCCESS)
	{
		auto nBufSize = 512UL;

		char szBuffer[_MAX_PATH] = { 0 };
		auto dwDataType = REG_SZ;
		auto lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("ComputerHardwareId").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szPcHwid = szBuffer;
		}

		g_winapiApiTable->RegCloseKey(hKey);
	}

	res = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", NULL, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (res == ERROR_SUCCESS)
	{
		auto nBufSize = 512UL;

		char szBuffer[_MAX_PATH] = { 0 };
		auto dwDataType = REG_SZ;
		auto lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("BIOSReleaseDate").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szBiosDate = szBuffer;
		}

		lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("BIOSVendor").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szBiosVendor = szBuffer;
		}

		lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("BIOSVersion").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szBiosVersion = szBuffer;
		}

		g_winapiApiTable->RegCloseKey(hKey);
	}

	res = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate", NULL, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (res == ERROR_SUCCESS)
	{
		auto nBufSize = 512UL;

		char szBuffer[_MAX_PATH] = { 0 };
		auto dwDataType = REG_SZ;
		auto lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("SusClientId").crypt_get(), NULL, &dwDataType, (LPBYTE)&szBuffer, &nBufSize);
		if (lVal == ERROR_SUCCESS)
		{
			m_szSusCid = szBuffer;
		}

		g_winapiApiTable->RegCloseKey(hKey);
	}
}

