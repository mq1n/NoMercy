#pragma once
#include <windows.h>
#include <tchar.h>
#include <iphlpapi.h>
#include <ipifcons.h>
#include <vector>

class CNetworkAdapter;

#define DEFAULT_GATEWAY_ADDR	0

struct _IPINFO
{
	std::string sIp;
	std::string sSubnet;
};

class CIpInfoArray : public std::vector < _IPINFO > {};
class StringArray : public std::vector < std::string > {};

DWORD EnumNetworkAdapters(CNetworkAdapter* lpBuffer, ULONG ulSzBuf, LPDWORD lpdwOutSzBuf);

class CNetworkAdapter
{
public:
	CNetworkAdapter();
	~CNetworkAdapter();
	bool SetupAdapterInfo(IP_ADAPTER_INFO* pAdaptInfo);

	// information about the adapters name for the users
	// and its name to the system
	std::string GetAdapterName() const;
	std::string GetAdapterDescription() const;

	// dhcp lease access functions
	time_t	GetLeaseObtained() const;
	time_t	GetLeaseExpired() const;

	// access to lists of various server's ip address
	SIZE_T	GetNumIpAddrs() const;
	SIZE_T	GetNumDnsAddrs() const;
	std::string	GetIpAddr(int nIp = 0) const;
	std::string GetSubnetForIpAddr(int nIp = 0) const;
	std::string	GetDnsAddr(int nDns = 0) const;
	std::string GetCurrentIpAddress() const;

	// dhcp function
	bool	IsDhcpUsed() const;
	std::string	GetDchpAddr() const;

	// wins function
	bool	IsWinsUsed() const;
	std::string GetPrimaryWinsServer() const;
	std::string GetSecondaryWinsServer() const;

	std::string	GetGatewayAddr(int nGateway = DEFAULT_GATEWAY_ADDR) const;
	SIZE_T	GetNumGatewayAddrs() const;

	static	std::string GetAdapterTypeString(UINT nType);
	UINT	GetAdapterType() const;

	DWORD	GetAdapterIndex() const;

protected:
	std::string	GetStringFromArray(const StringArray* pPtr, int nIndex) const;

private:
	std::string			m_sName;		// adapter name with the computer.  For human readable name use m_sDesc.
	std::string			m_sDesc;
	std::string			m_sPriWins;
	std::string			m_sSecWins;
	std::string			m_sDefGateway;
	std::string			m_sDhcpAddr;
	_IPINFO			m_sCurIpAddr;	// this is also in the ip address list but this is the address currently active.
	DWORD			m_dwIndex;		// machine index of the adapter.
	UINT			m_nAdapterType;
	bool			m_bDhcpUsed;
	bool			m_bWinsUsed;
	StringArray		m_DnsAddresses;
	CIpInfoArray	m_IpAddresses;
	StringArray		m_GatewayList;
	time_t			m_tLeaseObtained;
	time_t			m_tLeaseExpires;

	struct UNNAMED
	{
		BYTE	ucAddress[MAX_ADAPTER_ADDRESS_LENGTH];
		UINT	nLen;
	} m_ucAddress;
public:
	// returns formatted MAC address in HEX punctuated with "::"
	std::string GetAdapterAddress(void);
};

