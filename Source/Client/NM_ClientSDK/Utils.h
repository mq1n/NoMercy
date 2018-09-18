#pragma once
#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>

namespace NNoMercyUtils
{
	void FileLog(const std::string & szFileName, const std::string & szLogData);
	void FileLogf(const std::string & szFileName, const char* c_szFormat, ...);
	bool IsFileExist(const std::string & szFileName);
};

