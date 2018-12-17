
#include "NM_Main.h"
#include "NM_Index.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "ProcessFunctions.h"

#include <xorstr.hpp>

typedef PTEB_ACTIVE_FRAME(NTAPI* lpRtlGetFrame)(VOID);
typedef VOID(NTAPI* lpRtlPushFrame)(PTEB_ACTIVE_FRAME Frame);
typedef VOID(NTAPI* lpRtlPopFrame)(PTEB_ACTIVE_FRAME Frame);

#define _TAG4(s) ( \
	(((s) >> 24) & 0xFF)       | \
	(((s) >> 8 ) & 0xFF00)     | \
	(((s) << 24) & 0xFF000000) | \
	(((s) << 8 ) & 0x00FF0000) \
	)
#define TAG4(s) _TAG4((UINT32)(s))
#define LOAD_MEM_DLL_INFO_MAGIC  TAG4('LDFM')

PTEB_ACTIVE_FRAME FindThreadFrameByContext(ULONG_PTR Context)
{
	auto pRtlGetFrame = reinterpret_cast<lpRtlGetFrame>(g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlGetFrame").crypt_get()));
	auto Frame = pRtlGetFrame();

	while (Frame != NULL && Frame->Context != Context)
		Frame = Frame->Previous;

	return Frame;
}

LOAD_MEM_DLL_INFO* GetLoadMemDllInfo()
{
	return reinterpret_cast<LOAD_MEM_DLL_INFO*>(FindThreadFrameByContext(LOAD_MEM_DLL_INFO_MAGIC));
}

