#include "NM_Index.h"
#include "NM_Main.h"
#include "MacAddress.h"
#include "HW-Info.h"

#include <NtDDNdis.h>

CMacAddress::CMacAddress()
{
	FindValidMac();
	FindPhysicalMacAddress();
}

CMacAddress::~CMacAddress()
{
	if (m_pAdapters)
	{
		delete[] m_pAdapters;
		m_pAdapters = NULL;
	}
}

bool CMacAddress::IsPrimaryAdapter(std::uint32_t dwIndex)
{
	bool bIsPrimaryAdapter = false;

	if (m_pAdapters)
	{
		auto dwMinIndex = m_pAdapters->GetAdapterIndex();
		for (auto i = 0UL; i < m_nCount; i++)
		{
			auto pAdapt = &m_pAdapters[i];
			if (pAdapt->GetAdapterIndex() < dwMinIndex)
			{
				dwMinIndex = pAdapt->GetAdapterIndex();
			}
		}

		if (dwIndex == dwMinIndex)
		{
			bIsPrimaryAdapter = true;
		}
	}

	return bIsPrimaryAdapter;
}

bool CMacAddress::InitAdapters()
{
	bool	bRet = true;
	DWORD	dwErr = 0;
	ULONG	ulNeeded = 0;

	dwErr = EnumNetworkAdapters(m_pAdapters, 0, &ulNeeded);
	if (dwErr == ERROR_INSUFFICIENT_BUFFER)
	{
		m_nCount = ulNeeded / sizeof(CNetworkAdapter);
		m_pAdapters = new CNetworkAdapter[ulNeeded / sizeof(CNetworkAdapter)];
		dwErr = EnumNetworkAdapters(m_pAdapters, ulNeeded, &ulNeeded);
		if (!m_pAdapters)
		{
			bRet = false;
		}
	}
	else
	{
		bRet = false;
	}
	return bRet;
}

bool CMacAddress::FindValidMac()
{
	InitAdapters();

	if (m_pAdapters)
	{
		for (UINT i = 0; i < m_nCount; i++)
		{
			auto pAdapter = &m_pAdapters[i];

			auto dwIndex = pAdapter->GetAdapterIndex();
			if (IsPrimaryAdapter(dwIndex))
			{
				SetMacAdapterInfo(pAdapter->GetAdapterAddress(), pAdapter->GetAdapterName());
				return true;
			}
		}
	}
	return false;
}

void CMacAddress::SetMacAdapterInfo(const std::string & szAdapterAddress, const std::string & szAdapterName)
{
	m_szPrimaryAdapterAddress	= szAdapterAddress;
	m_szPrimaryAdapterName		= szAdapterName;
}


BOOL CMacAddress::FindPhysicalMacAddress()
{
	DWORD dwLength;
	IP_INTERFACE_INFO * pInterface = { 0 };
	if (GetInterfaceInfo(pInterface, &dwLength) == ERROR_INSUFFICIENT_BUFFER)
		pInterface = (IP_INTERFACE_INFO *)malloc(dwLength);

	if (GetInterfaceInfo(pInterface, &dwLength) == NO_ERROR)
	{
		for (int i = 0; i < pInterface->NumAdapters; i++)
		{
			auto wszBuffer = std::wstring(pInterface->Adapter[i].Name);
			if (wszBuffer.size() != 38 && wszBuffer.size() != 52) // 38 = adapter name, 14 = '\DEVICE\TCPIP_' tag
				continue;

			auto wszAdapterName = wszBuffer.substr(wszBuffer.size() - 38, 38);
			auto szAdapterName = std::string(wszAdapterName.begin(), wszAdapterName.end());

			if (szAdapterName != m_szPrimaryAdapterName) // current adapter is not primary adapter
				continue;

			WCHAR wszPath[MAX_PATH] = { L'\0'};
			swprintf(wszPath, MAX_PATH, L"\\\\.\\%ls", wszAdapterName.c_str());

			auto hDevice = CreateFileW(wszPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (!IS_VALID_HANDLE(hDevice))
				continue;

			UCHAR uch[6];
//			DWORD dwIoObj = OID_802_3_CURRENT_ADDRESS;
			DWORD dwIoObj = OID_802_3_PERMANENT_ADDRESS;
			if (!DeviceIoControl(hDevice, IOCTL_NDIS_QUERY_GLOBAL_STATS, &dwIoObj, sizeof(dwIoObj), &uch, sizeof(uch), &dwLength, NULL))
				continue;

			if (dwLength != 6)
				continue;

			if (uch[0] == 0 && uch[1] == 0 && uch[2] == 0 && uch[3] == 0 && uch[4] == 0 && uch[5] == 0)
				continue;

			char szOutput[128];
			sprintf(szOutput, "%02X:%02X:%02X:%02X:%02X:%02X", uch[0], uch[1], uch[2], uch[3], uch[4], uch[5]);
			m_szPhysicalMacAddress = szOutput;
			break;
		}

		return TRUE;
	}

	return FALSE;
}



