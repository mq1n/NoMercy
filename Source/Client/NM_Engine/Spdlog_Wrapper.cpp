#include "NM_Index.h"
#include "NM_Main.h"
#include "spdlog_wrapper.h"

#include <lazy_importer.hpp>

inline std::string GetCurrentPath()
{
	char buffer[MAX_PATH];
	LI_FIND(GetModuleFileNameA)(NULL, buffer, MAX_PATH);

	auto szBuffer = std::string(buffer);
	auto pos = szBuffer.find_last_of("\\/");
	return szBuffer.substr(0, pos);
}

static void LogErrorHandler(const std::string & szMessage)
{
	Logf(CUSTOM_LOG_FILENAME, xorstr("Log error handled: %s\n").crypt_get(), szMessage.c_str());
}


CLogger::CLogger(const std::string & szLoggerName, const std::string & szFileName) :
	m_szLoggerName(szLoggerName), m_szFileName(szFileName)
{
	try
	{
		auto sinks = std::vector<spdlog::sink_ptr>();

		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(m_szFileName.c_str()));

		m_logger = std::make_shared<spdlog::logger>(m_szFileName.c_str(), sinks.begin(), sinks.end());
		m_logger->set_error_handler(LogErrorHandler);

		spdlog::register_logger(m_logger);
	}
	catch (const spdlog::spdlog_ex & ex)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Exception throw on InitLogger (spdlog::spdlog_ex): %s\n").crypt_get(), ex.what());
		abort();
	}
	catch (DWORD dwNumber)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Exception throw on InitLogger (w/ number): %p\n").crypt_get(), dwNumber);
		abort();
	}
	catch (...)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Exception throw on InitLogger (w/o information!)\n").crypt_get());
		abort();
	}
}

void CLogger::sys_log(const char* c_szFunction, int iLevel, const char* c_szFormat, ...)
{
	std::lock_guard <std::recursive_mutex> __lock(m_mutex);

	char pTmpString[8192] = { 0 };
	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf(pTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	char pFinalString[9000] = { 0 };
	if (strlen(c_szFunction))
		sprintf(pFinalString, xorstr("%s | %s").crypt_get(), c_szFunction, pTmpString);
	else
		strcpy(pFinalString, pTmpString);

#if 0
	if (IS_VALID_SMART_PTR(g_nmApp->DataInstance()) && g_nmApp->DataInstance()->GetAppType() == NM_SERVICE && IS_VALID_SMART_PTR(g_nmApp->ServiceToHelperPipeMgr()))
	{
		g_nmApp->ServiceToHelperPipeMgr()->SendServiceLogInformation(iLevel, pFinalString);
		return;
	}
#endif

	try
	{
		switch (iLevel)
		{
			case LL_SYS: m_logger->info(pFinalString); break;
			case LL_CRI: m_logger->critical(pFinalString); break;
			case LL_ERR: m_logger->error(pFinalString); break;
			case LL_DEV: m_logger->debug(pFinalString); break;
			case LL_TRACE: m_logger->trace(pFinalString); break;
			case LL_WARN: m_logger->warn(pFinalString); break;
		}
		m_logger->flush();
	}
	catch (const spdlog::spdlog_ex & ex)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Exception throw on sys_log (spdlog::spdlog_ex %s\n").crypt_get(), ex.what());
		abort();
	}
	catch (DWORD dwNumber)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Exception throw on sys_log (w/ number): %p\n").crypt_get(), dwNumber);
		abort();
	}
	catch (...)
	{
		Logf(CUSTOM_LOG_FILENAME, xorstr("Exception throw on sys_log (w/o information!)\n").crypt_get());
		abort();
	}
}

