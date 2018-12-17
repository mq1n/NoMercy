#pragma once
#include <Windows.h>
#include <winioctl.h>
#include <string>

#include "../../Common/SharedTypes/WdkTypes.h"
#include "../../Common/SharedTypes/CtlTypes.h"


#define IOCTL(Code, Method) (CTL_CODE(0x8000, (Code), Method, FILE_ANY_ACCESS))
#define EXTRACT_CTL_CODE(Ioctl)   ((unsigned short)(((Ioctl) & 0b0011111111111100) >> 2))
#define EXTRACT_CTL_METHOD(Ioctl) ((unsigned short)((Ioctl) & 0b11))

#define CTL_BASE (0x800)

enum EIOMessages
{
	IO_MAP_MDL,
	IO_UNMAP_MDL,
	IO_PING_REQUEST,
	IO_GAME_LAUNCH_REQ,
	IO_PID_DATA
};

typedef struct _KERNEL_IO_PING_DATA
{
	INT iMessage;
} SKernelIOPingData, *PKernelIOPingData;

class CServiceComm
{
	public:
		CServiceComm();
		~CServiceComm();

		bool Initialize(const std::string & szName);

		bool SendIOCTL(HANDLE hDevice, DWORD Ioctl, PVOID InputBuffer, ULONG InputBufferSize, PVOID OutputBuffer, ULONG OutputBufferSize, PDWORD BytesReturned = NULL, DWORD Method = METHOD_NEITHER);
		bool SendRawIOCTL(HANDLE hDevice, DWORD Ioctl, PVOID InputBuffer, ULONG InputBufferSize, PVOID OutputBuffer, ULONG OutputBufferSize, PDWORD BytesReturned = NULL);
		bool SendRequest(DWORD dwIndex, IN PVOID pInput = NULL, ULONG ulInputSize = 0, OUT PVOID pOutput = NULL, ULONG ulOutputSize = 0);

		bool KbMapMdl(OUT WdkTypes::PVOID* MappedMemory, OPTIONAL UINT64 SrcProcessId, OPTIONAL UINT64 DestProcessId, WdkTypes::PMDL Mdl, BOOLEAN NeedLock,
			WdkTypes::KPROCESSOR_MODE AccessMode = WdkTypes::UserMode, ULONG Protect = PAGE_EXECUTE_READWRITE, WdkTypes::MEMORY_CACHING_TYPE CacheType = WdkTypes::MmNonCached, 
			OPTIONAL WdkTypes::PVOID UserRequestedAddress = NULL
		);
		bool KbUnmapMdl(IN WdkTypes::PMDL Mdl, IN WdkTypes::PVOID MappedMemory, BOOLEAN NeedUnlock);

		bool Ping();

	private:
		HANDLE m_hDriver;
};

