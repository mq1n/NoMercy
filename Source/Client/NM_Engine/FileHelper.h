#pragma once

class CFileFunctions
{
	public:
		static bool File2Mem(const std::string & szFileName, std::string * pszOutput);
		static bool Mem2File(const std::string & szFileName, BYTE* pBuffer, DWORD dwSize);
};

