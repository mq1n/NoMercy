#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Utils.h"

#include <lazy_importer.hpp>

#define SERVICE_NAME "NoMercySvc"

inline void WriteEventLogEntry(const std::string & szMessage, WORD wType)
{
	HANDLE EventSource = NULL;
	LPCSTR Strings[2] = { NULL, NULL };

	EventSource = RegisterEventSourceA(NULL, SERVICE_NAME);
	if (EventSource)
	{
		Strings[0] = SERVICE_NAME;
		Strings[1] = szMessage.c_str();

		ReportEventA(EventSource, wType, 0, 0, NULL, 2, 0, Strings, NULL);

		DeregisterEventSource(EventSource);
	}
}

void NNoMercyUtils::WriteErrorLogEntry(const std::string & szFunction, uint32_t dwError)
{
	char szMessage[1024];
	sprintf(szMessage, "NoMercy | %s Failed: 0x%08lx", szFunction.c_str(), dwError);

	WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE);
}


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

void NNoMercyUtils::DebugLog(const std::string & szLogData)
{
	OutputDebugStringA(szLogData.c_str());
}

void NNoMercyUtils::DebugLogf(const char* c_szFormat, ...)
{
	char cTmpString[8192];
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf(cTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	DebugLog(cTmpString);
}

bool NNoMercyUtils::IsFileExist(const std::string & szFileName)
{
	auto dwAttrib = LI_FIND(GetFileAttributesA)(szFileName.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string NNoMercyUtils::ExePath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	auto szBuffer = std::string(buffer);
	auto pos = szBuffer.find_last_of("\\/");
	return szBuffer.substr(0, pos);
}
