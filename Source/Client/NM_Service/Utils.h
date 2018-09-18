#pragma once
#include <string>

namespace NNoMercyUtils
{
	void FileLog(const std::string & szFileName, const std::string & szLogData);
	void FileLogf(const std::string & szFileName, const char* c_szFormat, ...);

	void DebugLog(const std::string & szLogData);
	void DebugLogf(const char* c_szFormat, ...);

	void WriteErrorLogEntry(const std::string & szFunction, uint32_t dwError);

	bool IsFileExist(const std::string & szFileName);
	std::string ExePath();
};

