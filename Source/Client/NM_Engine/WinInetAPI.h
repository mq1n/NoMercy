#pragma once

class CInternetAPI
{
	public:
		CInternetAPI();
		~CInternetAPI() = default;

		bool ExistOnHostsFile(const std::string & szEntry);
		bool CheckInternetConnection(const std::string & szTargetUrl);
		bool WebStatusCheck(const std::string & szAddress, LPDWORD pdwWebStat);
		bool IsManipulatedConnection();
		bool IsCorrectIPAddressOfWebsite(const std::string & szWebsite, const std::string & szIPAddress);

		bool ReadUrl(const std::string & szAddress, std::string * pszResult, std::size_t * puiSize);
		bool PostData(const std::string & szHost, const std::string & szUrl, const char* c_szFileName, LPBYTE pData, std::size_t ulDataSize);

	private:
		std::string m_szWebAgentName;
};
