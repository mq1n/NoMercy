#include "NM_Index.h"
#include "NM_Main.h"
#include "NetworkAdapter.h"

#define ALLOCATE_FROM_PROCESS_HEAP( bytes )		::HeapAlloc( ::GetProcessHeap(), HEAP_ZERO_MEMORY, bytes )
#define DEALLOCATE_FROM_PROCESS_HEAP( ptr )		if( ptr ) ::HeapFree( ::GetProcessHeap(), 0, ptr )
#define REALLOC_FROM_PROCESS_HEAP( ptr, bytes )	::HeapReAlloc( ::GetProcessHeap(), HEAP_ZERO_MEMORY, ptr, bytes )

////////////////////////////////////////////////////////////
//	Desc:
//		Builds a listing of all network adapters found
//		on the current system.  If there is not enough
//		space in the passed in buffer ERROR_INSUFFICIENT_BUFFER
//		is returned. If error lpdwOutSzBuf contains the
//		needed buffer size.  If no error it returned
//		used buffer size.
////////////////////////////////////////////////////////////
DWORD EnumNetworkAdapters(CNetworkAdapter* pAdapters, ULONG ulSzBuf, LPDWORD lpdwOutSzBuf)
{
	IP_ADAPTER_INFO* pAdptInfo = NULL;
	IP_ADAPTER_INFO* pNextAd = NULL;
	ULONG ulLen = 0;
	int nCnt = 0;
	DWORD erradapt;

	erradapt = ::GetAdaptersInfo(pAdptInfo, &ulLen);
	if (erradapt == ERROR_BUFFER_OVERFLOW)
	{
		pAdptInfo = (IP_ADAPTER_INFO*)ALLOCATE_FROM_PROCESS_HEAP(ulLen);
		erradapt = ::GetAdaptersInfo(pAdptInfo, &ulLen);
	}

	// acquire a count of how many network adapters
	// we are going to need to work on so that
	// we can make sure we have adequate buffer space.
	pNextAd = pAdptInfo;
	while (pNextAd)
	{
		nCnt++;
		pNextAd = pNextAd->Next;
	}

	// now that we have the count we need to determine if the caller passed in an
	// adequate buffer for us to store all the needed adapters.  we set the variable
	// to the needed number of bytes so the caller will now how many there are
	// and how much space is needed to hold them.
	*lpdwOutSzBuf = nCnt * sizeof(CNetworkAdapter);
	if (ulSzBuf < *lpdwOutSzBuf)
	{
		DEALLOCATE_FROM_PROCESS_HEAP(pAdptInfo);
		return ERROR_INSUFFICIENT_BUFFER;
	}

	// this needs to be reset for future use.
	nCnt = 0;

	if (erradapt == ERROR_SUCCESS)
	{
		// initialize the pointer we use the move through
		// the list.
		pNextAd = pAdptInfo;

		// loop through for all available interfaces and setup an associated
		// CNetworkAdapter class.
		while (pNextAd)
		{
			pAdapters[nCnt].SetupAdapterInfo(pNextAd);
			// move forward to the next adapter in the list so
			// that we can collect its information.
			pNextAd = pNextAd->Next;
			nCnt++;
		}
	}

	// free any memory we allocated from the heap before
	// exit.  we wouldn't wanna leave memory leaks now would we? ;p
	DEALLOCATE_FROM_PROCESS_HEAP(pAdptInfo);

	return ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Construction.  Initializes the variables and makes
//		sure all the collection classes are cleaned out.
////////////////////////////////////////////////////////////
CNetworkAdapter::CNetworkAdapter() : 
	m_tLeaseObtained(0), m_tLeaseExpires(0)
{
	m_bDhcpUsed = FALSE;
	m_bWinsUsed = FALSE;
	m_dwIndex = 0;
	m_nAdapterType = 0;

	::memset(&m_ucAddress.ucAddress, 0, MAX_ADAPTER_ADDRESS_LENGTH);
	m_ucAddress.nLen = 0;

	m_DnsAddresses.clear();
	m_IpAddresses.clear();
	m_GatewayList.clear();

	return;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Deconstruction.
////////////////////////////////////////////////////////////
CNetworkAdapter::~CNetworkAdapter()
{
#ifdef _DEBUG
	::memset(&m_ucAddress.ucAddress, 0, MAX_ADAPTER_ADDRESS_LENGTH);
	m_ucAddress.nLen = 0;
	m_bDhcpUsed = FALSE;
	m_bWinsUsed = FALSE;
	m_dwIndex = 0;
	m_nAdapterType = 0;
#endif

	m_DnsAddresses.clear();
	m_IpAddresses.clear();
	m_GatewayList.clear();

	return;
}

////////////////////////////////////////////////////////////
//	Desc:
//
////////////////////////////////////////////////////////////
bool CNetworkAdapter::SetupAdapterInfo(IP_ADAPTER_INFO* pAdaptInfo)
{
	bool bSetupPassed = FALSE;
	IP_ADDR_STRING* pNext = NULL;
	IP_PER_ADAPTER_INFO* pPerAdapt = NULL;
	ULONG ulLen = 0;
	DWORD err;

	_IPINFO iphold;

	if (pAdaptInfo)
	{
		m_sName = pAdaptInfo->AdapterName;
		m_sDesc = pAdaptInfo->Description;

		m_sPriWins = pAdaptInfo->PrimaryWinsServer.IpAddress.String;
		m_sSecWins = pAdaptInfo->SecondaryWinsServer.IpAddress.String;
		m_dwIndex = pAdaptInfo->Index;
		m_nAdapterType = pAdaptInfo->Type;
		m_bDhcpUsed = pAdaptInfo->DhcpEnabled ? true : false;
		m_bWinsUsed = pAdaptInfo->HaveWins ? true : false;
		m_tLeaseObtained = pAdaptInfo->LeaseObtained;
		m_tLeaseExpires = pAdaptInfo->LeaseExpires;
		m_sDhcpAddr = pAdaptInfo->DhcpServer.IpAddress.String;

		//rsheikh: set adpater address length, and then copy each byte from
		//pAdaptInfo->Address (the MAC address) to the adapter address structure.
		m_ucAddress.nLen = pAdaptInfo->AddressLength;
		for (int i = 0; i < (int)m_ucAddress.nLen; i++)
		{
			m_ucAddress.ucAddress[i] = pAdaptInfo->Address[i];
		}

		if (pAdaptInfo->CurrentIpAddress)
		{
			m_sCurIpAddr.sIp = pAdaptInfo->CurrentIpAddress->IpAddress.String;
			m_sCurIpAddr.sSubnet = pAdaptInfo->CurrentIpAddress->IpMask.String;
		}
		else
		{
			m_sCurIpAddr.sIp = xorstr("0.0.0.0").crypt_get();
			m_sCurIpAddr.sSubnet = xorstr("0.0.0.0").crypt_get();
		}

		// since an adapter may have more than one ip address we need
		// to populate the array we have setup with all available
		// ip addresses.
		pNext = &(pAdaptInfo->IpAddressList);
		while (pNext)
		{
			iphold.sIp = pNext->IpAddress.String;
			iphold.sSubnet = pNext->IpMask.String;
			m_IpAddresses.push_back(iphold);
			pNext = pNext->Next;
		}

		// an adapter usually has just one gateway however the provision exists
		// for more than one so to "play" as nice as possible we allow for it here
		// as well.
		pNext = &(pAdaptInfo->GatewayList);
		while (pNext)
		{
			m_GatewayList.push_back(pNext->IpAddress.String);
			pNext = pNext->Next;
		}

		// we need to generate a IP_PER_ADAPTER_INFO structure in order
		// to get the list of dns addresses used by this adapter.
		err = ::GetPerAdapterInfo(m_dwIndex, pPerAdapt, &ulLen);
		if (err == ERROR_BUFFER_OVERFLOW)
		{
			pPerAdapt = (IP_PER_ADAPTER_INFO*)ALLOCATE_FROM_PROCESS_HEAP(ulLen);
			err = ::GetPerAdapterInfo(m_dwIndex, pPerAdapt, &ulLen);

			// if we succeed than we need to drop into our loop
			// and fill the dns array will all available IP
			// addresses.
			if (err == ERROR_SUCCESS)
			{
				pNext = &(pPerAdapt->DnsServerList);
				while (pNext)
				{
					m_DnsAddresses.push_back(pNext->IpAddress.String);
					pNext = pNext->Next;
				}
				bSetupPassed = TRUE;
			}

			// this is done outside the err == ERROR_SUCCES just in case. the macro
			// uses NULL pointer checking so it is ok if pPerAdapt was never allocated.
			DEALLOCATE_FROM_PROCESS_HEAP(pPerAdapt);
		}
	}

	return bSetupPassed;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Generic function to grab a string from an array.
//		purpose of this function is just to add error
//		checking.
////////////////////////////////////////////////////////////
std::string	CNetworkAdapter::GetStringFromArray(const StringArray* pPtr, int nIndex) const
{
	std::string sStr = "";
	if (pPtr && ((SIZE_T)nIndex < pPtr->size()))
	{
		sStr = (*pPtr)[nIndex];
	}

	return sStr;
}

////////////////////////////////////////////////////////////
//	Desc:
//		Returns a string translation of the integer type
//		identifer for an adapter.
////////////////////////////////////////////////////////////
std::string CNetworkAdapter::GetAdapterTypeString(UINT nType)
{
	std::string sType = "";
	switch (nType)
	{
	case MIB_IF_TYPE_OTHER:
		sType = xorstr("Other").crypt_get();
		break;
	case MIB_IF_TYPE_ETHERNET:
		sType = xorstr("Ethernet").crypt_get();
		break;
	case MIB_IF_TYPE_TOKENRING:
		sType = xorstr("Token Ring").crypt_get();
		break;
	case MIB_IF_TYPE_FDDI:
		sType = xorstr("FDDI").crypt_get();
		break;
	case MIB_IF_TYPE_PPP:
		sType = xorstr("PPP").crypt_get();
		break;
	case MIB_IF_TYPE_LOOPBACK:
		sType = xorstr("Loopback").crypt_get();
		break;
	case MIB_IF_TYPE_SLIP:
		sType = xorstr("SLIP").crypt_get();
		break;
	default:
		sType = xorstr("Invalid Adapter Type").crypt_get();
		break;
	};

	return sType;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Data Accessor Functions														//
//											Pretty Self Explanitory														//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string CNetworkAdapter::GetAdapterName()				const
{
	return m_sName;
}
std::string CNetworkAdapter::GetAdapterDescription()		const
{
	return m_sDesc;
}
time_t CNetworkAdapter::GetLeaseObtained()				const
{
	return m_tLeaseObtained;
}
time_t CNetworkAdapter::GetLeaseExpired()				const
{
	return m_tLeaseExpires;
}
SIZE_T	CNetworkAdapter::GetNumIpAddrs()				const
{
	return m_IpAddresses.size();
}
SIZE_T	CNetworkAdapter::GetNumDnsAddrs()				const
{
	return m_DnsAddresses.size();
}
std::string	CNetworkAdapter::GetDnsAddr(int nDns)			const
{
	return GetStringFromArray(&m_DnsAddresses, nDns);
}
std::string CNetworkAdapter::GetCurrentIpAddress()			const
{
	return m_sCurIpAddr.sIp;
}
bool CNetworkAdapter::IsDhcpUsed()						const
{
	return m_bDhcpUsed;
}
std::string	CNetworkAdapter::GetDchpAddr()					const
{
	return m_sDhcpAddr;
}
bool CNetworkAdapter::IsWinsUsed()						const
{
	return m_bWinsUsed;
}
std::string CNetworkAdapter::GetPrimaryWinsServer()			const
{
	return m_sPriWins;
}
std::string CNetworkAdapter::GetSecondaryWinsServer()		const
{
	return m_sSecWins;
}
std::string	CNetworkAdapter::GetGatewayAddr(int nGateway) const
{
	return GetStringFromArray(&m_GatewayList, nGateway);
}
SIZE_T	CNetworkAdapter::GetNumGatewayAddrs()			const
{
	return m_GatewayList.size();
}
DWORD CNetworkAdapter::GetAdapterIndex()				const
{
	return m_dwIndex;
}
UINT CNetworkAdapter::GetAdapterType()					const
{
	return m_nAdapterType;
}

std::string	CNetworkAdapter::GetIpAddr(int nIp) const
{
	std::string sAddr = "";
	if ((SIZE_T)nIp < m_IpAddresses.size())
	{
		sAddr = m_IpAddresses[nIp].sIp;
	}

	return sAddr;
}

std::string CNetworkAdapter::GetSubnetForIpAddr(int nIp) const
{
	std::string sAddr = "";
	if ((SIZE_T)nIp < m_IpAddresses.size())
	{
		sAddr = m_IpAddresses[nIp].sSubnet;
	}
	return sAddr;
}

// rsheikh: returns formatted MAC address in HEX punctuated with hyphens "-"

std::string CNetworkAdapter::GetAdapterAddress(void)
{
	std::string sTemp = "";
	char macTemp[8];
	for (unsigned int i = 0; i < m_ucAddress.nLen; i++)
	{
		if (i > 0)
		{
			sTemp += "::";
		}

		sprintf_s(macTemp, sizeof(macTemp), xorstr("%02X").crypt_get(), m_ucAddress.ucAddress[i]);
		sTemp += macTemp;
	}

	return sTemp;
}