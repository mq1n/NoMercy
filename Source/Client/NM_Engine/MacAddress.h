#pragma once

class CMacAddress
{
	public:
		CMacAddress();
		~CMacAddress();

		BOOL FindPhysicalMacAddress();

		std::string GetMacAddress()
		{
			return m_szPrimaryAdapterAddress;
		};
		std::string GetPhysicalMacAddress()
		{
			return m_szPhysicalMacAddress;
		};

	protected:
		bool InitAdapters();
		bool FindValidMac();
		bool IsPrimaryAdapter(std::uint32_t dwIndex);
		void SetMacAdapterInfo(const std::string & szAdapterAddress, const std::string & szAdapterName);

	private:
		CNetworkAdapter*	m_pAdapters;
		std::uint32_t		m_nCount;

		std::string			m_szPrimaryAdapterAddress;
		std::string			m_szPrimaryAdapterName;
		std::string			m_szPhysicalMacAddress;
};
