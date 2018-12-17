#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ServiceCommunication.h"
#include "Defines.h"
#include "ProcessFunctions.h"
#include "../../Common/SharedTypes/WdkTypes.h"
#include "../../Common/SharedTypes/CtlTypes.h"

CServiceComm::CServiceComm() :
	m_hDriver(INVALID_HANDLE_VALUE)
{
}
CServiceComm::~CServiceComm()
{
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(m_hDriver);
}


bool CServiceComm::Initialize(const std::string & szName)
{
	m_hDriver = g_winapiApiTable->CreateFileA(szName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (!IS_VALID_HANDLE(m_hDriver))
		return false;

	return true;
}


bool CServiceComm::SendRawIOCTL(HANDLE hDevice, DWORD Ioctl, PVOID InputBuffer, ULONG InputBufferSize, PVOID OutputBuffer, ULONG OutputBufferSize, PDWORD BytesReturned)
{
	if (g_nmApp->DynamicWinapiInstance()->IsValidHandle(hDevice) == false)
	{
		DEBUG_LOG(LL_CRI, "Target device handle not found!!! Handle: %p Error: %u", hDevice, g_winapiApiTable->GetLastError());
		return false;
	}

	auto dwReturned = 0UL;
	auto bStatus = g_winapiApiTable->DeviceIoControl(hDevice, Ioctl, InputBuffer, InputBufferSize, OutputBuffer, OutputBufferSize, &dwReturned, NULL);

	if (BytesReturned) 
		*BytesReturned = dwReturned;
	return bStatus;
}

bool CServiceComm::SendIOCTL(HANDLE hDevice, DWORD Ioctl, PVOID InputBuffer, ULONG InputBufferSize, PVOID OutputBuffer, ULONG OutputBufferSize, PDWORD BytesReturned, DWORD Method)
{
	DWORD dwRawIoctl = CTL_CODE(0x8000, Ioctl, Method, FILE_ANY_ACCESS);

	return SendRawIOCTL(hDevice, dwRawIoctl, InputBuffer, InputBufferSize, OutputBuffer, OutputBufferSize, BytesReturned);
}

bool CServiceComm::SendRequest(DWORD dwIndex, IN PVOID pInput, ULONG ulInputSize, OUT PVOID pOutput, ULONG ulOutputSize)
{
	return SendIOCTL(m_hDriver, CTL_BASE + dwIndex, pInput, ulInputSize, pOutput, ulOutputSize);
}

// ---

bool CServiceComm::KbMapMdl(OUT WdkTypes::PVOID* MappedMemory, OPTIONAL UINT64 SrcProcessId, OPTIONAL UINT64 DestProcessId, WdkTypes::PMDL Mdl, BOOLEAN NeedLock, 
	WdkTypes::KPROCESSOR_MODE AccessMode, ULONG Protect, WdkTypes::MEMORY_CACHING_TYPE CacheType, OPTIONAL WdkTypes::PVOID UserRequestedAddress) 
{
	if (!MappedMemory) 
		return false;

	KB_MAP_MDL_OUT Output = {};

	KB_MAP_MDL_IN Input = {};
	Input.SrcProcessId = SrcProcessId;
	Input.DestProcessId = DestProcessId;
	Input.Mdl = Mdl;
	Input.NeedLock = NeedLock;
	Input.AccessMode = AccessMode;
	Input.Protect = Protect;
	Input.CacheType = CacheType;
	Input.UserRequestedAddress;

	BOOL Status = SendRequest(IO_MAP_MDL, &Input, sizeof(Input), &Output, sizeof(Output));

	if (MappedMemory)
		*MappedMemory = Output.BaseAddress;
	return Status;
}

bool CServiceComm::KbUnmapMdl(IN WdkTypes::PMDL Mdl, IN WdkTypes::PVOID MappedMemory, BOOLEAN NeedUnlock)
{
	if (!Mdl || !MappedMemory)
		return false;

	KB_UNMAP_MDL_IN Input = {};
	Input.Mdl = Mdl;
	Input.BaseAddress = MappedMemory;
	Input.NeedUnlock = NeedUnlock;
	return SendRequest(IO_UNMAP_MDL, &Input, sizeof(Input));
}

bool CServiceComm::Ping()
{
	SKernelIOPingData pData = { 0 };
	pData.iMessage = 0;

	auto bRet = SendRequest(IO_PING_REQUEST, &pData, sizeof(pData), &pData, sizeof(pData));
	DEBUG_LOG(LL_SYS, "IO completed! Result: %d Msg: %d", bRet, pData.iMessage);

	return bRet;
}

