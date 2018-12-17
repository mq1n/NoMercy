#pragma once
#include "BasicLog.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>

enum ELogLevels
{
	LL_SYS,
	LL_ERR,
	LL_CRI,
	LL_WARN,
	LL_DEV,
	LL_TRACE
};

class CLogger
{
	public:
		CLogger() = default;
		CLogger(const std::string & szLoggerName, const std::string & szFileName);
		~CLogger() = default;

		void sys_log(const char* c_szFunction, int iLevel, const char* c_szFormat, ...);

	private:
		mutable std::recursive_mutex		m_mutex;

		std::shared_ptr <spdlog::logger>	m_logger;
		std::string							m_szLoggerName;
		std::string							m_szFileName;
};

