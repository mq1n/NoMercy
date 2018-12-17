#pragma once
#include <iostream>
#include <fstream>

#include <lazy_importer.hpp>

static void FileLog(const std::string & szFileName, const std::string & szLogData)
{
	std::ofstream f(szFileName.c_str(), std::ofstream::out | std::ofstream::app);
	f << szLogData.c_str() << std::endl;
	f.close();
}

static void FileLogf(const std::string & szFileName, const char* c_szFormat, ...)
{
	char cTmpString[8192] = { 0 };
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf_s(cTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	FileLog(szFileName.c_str(), cTmpString);
}

static void DebugLog(const char * c_szLogData)
{
	LI_FIND(OutputDebugStringA)(c_szLogData);
}

static void DebugLogf(const char* c_szFormat, ...)
{
	char cTmpString[8192];
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf_s(cTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	DebugLog(cTmpString);
}

static void ConsoleLog(const char * c_szLogData)
{
	fputs(c_szLogData, stdout);
}

static void ConsoleLogf(const char* c_szFormat, ...)
{
	char cTmpString[8192];
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf_s(cTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	ConsoleLog(cTmpString);
}

static void Logf(const std::string & szFileName, const char* c_szFormat, ...)
{
	char cTmpString[8192];
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf_s(cTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

#ifdef _DEBUG
	DebugLog(cTmpString);
#endif
	ConsoleLog(cTmpString);
	FileLog(szFileName.c_str(), cTmpString);
}
