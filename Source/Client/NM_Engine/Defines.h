#pragma once
#include "NM_App.h"

static enum ENoMercyStage
{
	STATE_DEV	= 0x01,
	STATE_BETA	= 0x02,
	STATE_RC	= 0x03,
	STATE_RTM	= 0x04
};

#ifndef DEBUG_LOG
	#ifdef _DEBUG
		#define DEBUG_LOG(level, log, ...)	g_nmApp->LogHelperInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define DEBUG_LOG(level, log, ...)
	#endif
#endif

#if defined (_DEBUG) and not defined (ALLOW_VIRTUAL_ENVIRONMENT)
	#define ALLOW_VIRTUAL_ENVIRONMENT
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

	#define CHECK_BACKEND_CONNECTION	TRUE
	#define BACKEND_SERVER_IP			XOR("127.0.0.1")
	#define BACKEND_SERVER_PORT			1881
	#define BACKEND_SERVER_PORT_2		1882
	#define NETWORK_AES_IV				XOR("000102030405060708090A0B0C0D0E0F")
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


