#pragma once
#include <memory>
#include <string>
#include "NetworkAdapter.h"

#ifdef GetUserName
	#undef GetUserName
#endif

#ifdef GetComputerName
	#undef GetComputerName
#endif

class CHwidManager
{
	public:
		CHwidManager() = default;
		~CHwidManager() = default;

		bool Initilize();

		// ---
		std::string GetMacAddress() const
		{
			return m_szMacAddress;
		}
		std::string GetPhysicalMacAddress() const
		{
			return m_szPhysicalMacAddress;
		}
		std::string GetHDDSerialNumber() const
		{
			return m_szHDDSerial;
		}
		std::string GetHDDModelID() const
		{
			return m_szHDDModel;
		}
		std::string GetCpuID() const
		{
			return m_szCpuID;
		}
		std::string GetUserName() const
		{
			return m_szUserName;
		}
		std::string GetComputerName() const
		{
			return m_szComputerName;
		}
		std::string GetWinBuildNumber() const
		{
			return m_szBuildNumber;
		}
		std::string GetWinVersion() const
		{
			return m_szOSVersion;
		}
		std::string GetWinSPNumber() const
		{
			return m_szServicePackVersion;
		}
		std::string GetGUIDFromRegistry() const
		{
			return m_szMachineGUIDRegistry;
		}
		std::string GetGUIDFromWinapi() const
		{
			return m_szMachineGUIDWinapi;
		}
		std::string GetVolumeHashFromWinapi() const
		{
			return m_szMachineVolumeHash;
		}
		std::string GetMacAddressFromNetbios() const
		{
			return m_szNetbiosMacAddr;
		}
		std::string GetProductIdFromRegistry() const
		{
			return m_szProductId;
		}
		std::string GetInstallDateFromRegistry() const
		{
			return m_szInstallDate;
		}
		std::string GetScsiIdFromRegistry() const
		{
			return m_szScsiId;
		}
		std::string GetScsiSerialFromRegistry() const
		{
			return m_szScsiSerial;
		}
		std::string GetHardwareIdFromRegistry() const
		{
			return m_szPcHwid;
		}
		std::string GetBiosDateFromRegistry() const
		{
			return m_szBiosDate;
		}
		std::string GetBiosVendorFromRegistry() const
		{
			return m_szBiosVendor;
		}
		std::string GetBiosVersionFromRegistry() const
		{
			return m_szBiosVersion;
		}
		std::string GetSusClientIdFromRegistry() const
		{
			return m_szSusCid;
		}

	protected:
		std::string GenerateCpuID();
		std::string GenerateComputerName();
		std::string GenerateUserName();
		std::string GenerateGuidFromRegistry();
		std::string GenerateGuidFromWinapi();
		std::string GenerateVolumeHashFromWinapi();
		std::string GenerateMacAddressFromNetbios();
		void GenerateRegistrySpecificInformations();

	private:
		std::string m_szMacAddress;
		std::string m_szPhysicalMacAddress;
		std::string m_szHDDSerial;
		std::string m_szHDDModel;
		std::string m_szCpuID;
		std::string m_szUserName;
		std::string m_szComputerName;
		std::string m_szBuildNumber;
		std::string m_szOSVersion;
		std::string m_szServicePackVersion;
		std::string m_szMachineGUIDRegistry;
		std::string m_szMachineGUIDWinapi;
		std::string m_szMachineVolumeHash;
		std::string m_szNetbiosMacAddr;
		std::string m_szProductId;
		std::string m_szInstallDate;
		std::string m_szScsiId;
		std::string m_szScsiSerial;
		std::string m_szPcHwid;
		std::string m_szBiosDate;
		std::string m_szBiosVendor;
		std::string m_szBiosVersion;
		std::string m_szSusCid;
};
