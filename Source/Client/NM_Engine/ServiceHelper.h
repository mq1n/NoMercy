#pragma once

class CServiceHelper
{
	public:
		CServiceHelper();
		CServiceHelper(const std::string & szServiceName, const std::string & szDisplayName = "", const std::string & szServicePath = "");
		~CServiceHelper() = default;

		DWORD GetServiceStatus();
		bool IsInstalled(LPDWORD pdwErrorCode);

		bool LoadService(DWORD dwServiceType, LPDWORD pdwErrorCode);
		bool UnloadDriver(LPDWORD pdwErrorCode);

		bool StartDriver(LPDWORD pdwErrorCode);
		bool StopDriver(LPDWORD pdwErrorCode);

		bool SetupFilterInstance(const std::string & szDriverName, const std::string & szInstanceName, const std::string & szAltitude, DWORD Flags, bool SetAsDefaultInstance);

	private:
		std::string m_szServiceName;
		std::string m_szDisplayName;
		std::string m_szServicePath;
};

