#pragma once
#include "NM_App.h"
#include <xorstr.hpp>

enum ENoMercyStage
{
	STATE_DEV	= 0x01,
	STATE_BETA	= 0x02,
	STATE_RC	= 0x03,
	STATE_RTM	= 0x04
};

#ifndef DEBUG_LOGS_DEBUG_BUILD_ONLY
	#define DEBUG_LOGS_DEBUG_BUILD_ONLY TRUE
#endif

#ifndef DEBUG_LOG
	#if defined(_DEBUG) or (DEBUG_LOGS_DEBUG_BUILD_ONLY == FALSE)
		#define DEBUG_LOG(level, log, ...)	g_nmApp->LogHelperInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define DEBUG_LOG(level, log, ...)
	#endif
#endif

#ifndef HOOK_LOG
	#if defined(_DEBUG) or (DEBUG_LOGS_DEBUG_BUILD_ONLY == FALSE)
		#define HOOK_LOG(level, log, ...)	g_nmApp->SelfHooksInstance()->LogInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define HOOK_LOG(level, log, ...)
	#endif
#endif

#ifndef SCANNER_LOG
	#if defined(_DEBUG) or (DEBUG_LOGS_DEBUG_BUILD_ONLY == FALSE)
		#define SCANNER_LOG(level, log, ...) g_nmApp->ScannerInstance()->LogInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define SCANNER_LOG(level, log, ...)
	#endif
#endif

#ifndef SDK_LOG
	#if defined(_DEBUG) or (DEBUG_LOGS_DEBUG_BUILD_ONLY == FALSE)
		#define SDK_LOG(level, log, ...)	g_nmApp->SDKHelperInstance()->LogInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define SDK_LOG(level, log, ...)
	#endif
#endif

#ifndef NETWORK_LOG
	#if defined(_DEBUG) or (DEBUG_LOGS_DEBUG_BUILD_ONLY == FALSE)
		#define NETWORK_LOG(level, log, ...)	g_nmApp->NetworkMgrInstance()->LogInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define NETWORK_LOG(level, log, ...)
	#endif
#endif

// ---

#ifndef TEST_MODE
	#define TEST_MODE FALSE
#endif

#if defined (_DEBUG) and not defined (ALLOW_VIRTUAL_ENVIRONMENT)
	#define ALLOW_VIRTUAL_ENVIRONMENT
#endif

#ifndef BLOCK_CONSOLE_WINDOW
	#define BLOCK_CONSOLE_WINDOW FALSE
#endif

#ifndef DRIVER_TEST
	#define DRIVER_TEST
#endif

#ifndef NOMERCY_VERSION
	#define NOMERCY_VERSION "0.0.0.1"
#endif
#ifndef NOMERCY_ALLOWED_VERSION
	#define NOMERCY_ALLOWED_VERSION 1000
#endif

#ifndef NOMERCY_STAGE
	#define NOMERCY_STAGE ENoMercyStage::STATE_DEV
#endif

#ifndef NETWORK_DEFINES
	#define NETWORK_DEFINES

	#define ENABLE_SERVICE_TO_BACKEND_CONNECTION	FALSE
	#define CHECK_BACKEND_CONNECTION				FALSE
	#define BACKEND_SERVER_IP						xorstr("127.0.0.1").crypt_get()
	#define BACKEND_SERVER_PORT						1881
	#define BACKEND_SERVER_PORT_2					1882
	#define NETWORK_AES_IV							xorstr("000102030405060708090A0B0C0D0E0F").crypt_get()
	#define NETWORK_BODY_CRYPT_KEY					0x31
	#define NETWORK_CONFIG_FILENAME					xorstr("config.json").crypt_get()
	#define NET_CHECKSUM_LENGTH						(sizeof(uint32_t))
	#define NET_PACKET_HASH_MAGIC					(0x000FED0C)
#endif

#ifndef IS_X64_APP
	#ifdef _M_IX86
		#define IS_X64_APP FALSE
	#else
		#define IS_X64_APP TRUE
	#endif
#endif

#ifndef CLOSE_GAME_ON_EXIT
	#define CLOSE_GAME_ON_EXIT TRUE
#endif


