#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Utils.h"

#include <lazy_importer.hpp>

void NNoMercyUtils::FileLog(const std::string & szFileName, const std::string & szLogData)
{
	std::ofstream f(szFileName.c_str(), std::ofstream::out | std::ofstream::app);
	f << szLogData.c_str() << std::endl;
	f.close();
}

void NNoMercyUtils::FileLogf(const std::string & szFileName, const char* c_szFormat, ...)
{
	char cTmpString[8192];
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf(cTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	FileLog(szFileName.c_str(), cTmpString);
}

bool NNoMercyUtils::IsFileExist(const std::string & szFileName)
{
	auto dwAttrib = LI_FIND(GetFileAttributesA)(szFileName.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
