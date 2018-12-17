#include "NM_Index.h"
#include "NM_Main.h"
#include "NamedPipe.h"
#include "Defines.h"

CNamedPipe::CNamedPipe()
{
	m_hPipe			= INVALID_HANDLE_VALUE;
	m_szPipeName	= "";
}

CNamedPipe::CNamedPipe(const std::string & szPipeName)
{
    m_hPipe			= INVALID_HANDLE_VALUE;
	m_szPipeName	= szPipeName;
}

CNamedPipe::~CNamedPipe()
{
	DEBUG_LOG(LL_CRI, "CNamedPipe::~CNamedPipe");
 //   Close();
}

bool CNamedPipe::Create(DWORD dwOpenMode, DWORD dwPipeMode, DWORD dwMaxInstances, DWORD dwOutBufferSize, DWORD dwInBufferSize, DWORD dwDefaultTimeOut)
{
	if (IsOpen())
	{
		DEBUG_LOG(LL_ERR, "Target pipe is already opened");
		return false;
	}

	SECURITY_DESCRIPTOR sd = { 0 };
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, static_cast<PACL>(0), FALSE);

	SECURITY_ATTRIBUTES sa		= { 0 };
	sa.nLength					= sizeof(sa);
	sa.lpSecurityDescriptor		= &sd;
	sa.bInheritHandle			= FALSE;

	std::string szPipeName = xorstr("\\\\.\\pipe\\").crypt_get() + m_szPipeName;
    m_hPipe = ::CreateNamedPipeA(szPipeName.c_str(), dwOpenMode, dwPipeMode, dwMaxInstances, dwOutBufferSize, dwInBufferSize, dwDefaultTimeOut, &sa);

	if (!IS_VALID_HANDLE(m_hPipe))
	{
		DEBUG_LOG(LL_ERR, "CreateNamedPipeA fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::Open(DWORD dwDesiredAccess, DWORD dwShareMode)
{
	if (IsOpen())
	{
		DEBUG_LOG(LL_ERR, "Target pipe is already opened");
		return false;
	}

	auto szPipeName = xorstr("\\\\.\\pipe\\").crypt_get() + m_szPipeName;

    m_hPipe = CreateFileA(szPipeName.c_str(), dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, NULL, NULL);
	if (!IS_VALID_HANDLE(m_hPipe)) 
	{
		DEBUG_LOG(LL_ERR, "CreateFileA(Open) fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::Attach(HANDLE hPipe)
{
    if (m_hPipe != hPipe)
        Close();

    m_hPipe = hPipe;
    return true;
}

HANDLE CNamedPipe::Detach()
{
	auto hReturn = m_hPipe;
    m_hPipe = INVALID_HANDLE_VALUE;
    return hReturn;
}

bool CNamedPipe::Close()
{
    bool bSuccess = false;

	if (IsOpen() == false)
		return bSuccess;

	bSuccess = g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(m_hPipe);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "SafeCloseHandle fail! Error: %u", g_winapiApiTable->GetLastError());
    }

    return bSuccess;
}

bool CNamedPipe::ConnectClient(LPOVERLAPPED lpOverlapped)
{
	if (!m_hPipe)
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}

    BOOL bServerPipe;
	if (!IsServerPipe(bServerPipe)) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe is not a server pipe");
		return false;
	}
	if (!bServerPipe)
	{
		DEBUG_LOG(LL_ERR, "Must be called from the server side");
		return false;
	}

	auto bSuccess = ::ConnectNamedPipe(m_hPipe, lpOverlapped) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	if (!bSuccess)
	{
		DEBUG_LOG(LL_ERR, "ConnectNamedPipe fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::DisconnectClient()
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}

	BOOL bServerPipe;
	if (!IsServerPipe(bServerPipe)) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe is not a server pipe");
		return false;
	}
	if (!bServerPipe)
	{
		DEBUG_LOG(LL_ERR, "Must be called from the server side");
		return false;
	}

	auto bSuccess = ::DisconnectNamedPipe(m_hPipe);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "DisconnectNamedPipe fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::Flush()
{
	if (!m_hPipe)
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	auto bSuccess = ::FlushFileBuffers(m_hPipe);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "FlushFileBuffers fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::Write(LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite)
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	auto dwWrittenByteCount = 0UL;
	auto bSuccess = WriteFile(m_hPipe, lpBuffer, dwNumberOfBytesToWrite, &dwWrittenByteCount, NULL);
	if (!bSuccess || dwWrittenByteCount != dwNumberOfBytesToWrite)
	{
		DEBUG_LOG(LL_ERR, "WriteFile fail! Error code: %u", g_winapiApiTable->GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::Read(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead)
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	auto dwReadBytesCount = 0UL;
	auto bSuccess = ReadFile(m_hPipe, lpBuffer, dwNumberOfBytesToRead, &dwReadBytesCount, NULL);
//	if (!bSuccess || dwReadBytesCount != dwNumberOfBytesToRead)
	if (!bSuccess) 
	{
//		DEBUG_LOG(LL_ERR, "ReadFile fail! Req size: %u Read size: %u Error code: %u", dwNumberOfBytesToRead, dwReadBytesCount, g_winapiApiTable->GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::Peek(LPVOID lpBuffer, DWORD dwBufferSize, DWORD& dwBytesRead, DWORD& dwTotalBytesAvail, DWORD& dwBytesLeftThisMessage)
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	auto bSuccess = ::PeekNamedPipe(m_hPipe, lpBuffer, dwBufferSize, &dwBytesRead, &dwTotalBytesAvail, &dwBytesLeftThisMessage);
	if (!bSuccess) {
		DEBUG_LOG(LL_ERR, "Peek failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::Transact(LPVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer,  DWORD dwOutBufferSize, DWORD& dwBytesRead)
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	auto bSuccess = ::TransactNamedPipe(m_hPipe, lpInBuffer, dwInBufferSize, lpOutBuffer, dwOutBufferSize, &dwBytesRead, NULL);
	if (!bSuccess) {
		DEBUG_LOG(LL_ERR, "Transact failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::IsBlockingPipe(BOOL& bIsBlocking) const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	DWORD dwState;
	auto bSuccess = ::GetNamedPipeHandleStateA(m_hPipe, &dwState, NULL, NULL, NULL, NULL, 0);
	if (!bSuccess) {
		DEBUG_LOG(LL_ERR, "IsBlockingPipe failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}

	bIsBlocking = ((dwState & PIPE_NOWAIT) == 0);
    return true;
}

bool CNamedPipe::IsClientPipe(BOOL& bClientPipe) const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	DWORD dwFlags;
	auto bSuccess = ::GetNamedPipeInfo(m_hPipe, &dwFlags, NULL, NULL, NULL);
	if (!bSuccess) {
		DEBUG_LOG(LL_ERR, "IsClientPipe failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}

	bClientPipe = ((dwFlags & PIPE_CLIENT_END) != 0);
    return true;
}

bool CNamedPipe::IsServerPipe(BOOL& bServerPipe) const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}

    DWORD dwFlags;
	auto bSuccess = ::GetNamedPipeInfo(m_hPipe, &dwFlags, NULL, NULL, NULL);
	if (!bSuccess) {
		DEBUG_LOG(LL_ERR, "IsServerPipe failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}

	bServerPipe = ((dwFlags & PIPE_SERVER_END) != 0);
    return true;
}

bool CNamedPipe::IsMessagePipe(BOOL& bMessagePipe) const
{
	if (!m_hPipe)
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	DWORD dwState;
	auto bSuccess = ::GetNamedPipeHandleStateA(m_hPipe, &dwState, NULL, NULL, NULL, NULL, 0);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "IsMessagePipe failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}

	bMessagePipe = ((dwState & PIPE_READMODE_MESSAGE) != 0);
    return bSuccess;
}

DWORD CNamedPipe::GetCurrentInstances() const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return 0;
	}
	
	DWORD dwCurInstances = 0;
	auto bSuccess = ::GetNamedPipeHandleStateA(m_hPipe, NULL, &dwCurInstances, NULL, NULL, NULL, 0);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "GetCurrentInstances failed, GetLastError returned %d\n", ::GetLastError());
		return 0;
	}

    return dwCurInstances;
}

DWORD CNamedPipe::GetMaxCollectionCount() const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return 0;
	}

	BOOL bClientPipe;
	if (!IsClientPipe(bClientPipe))
	{
		DEBUG_LOG(LL_ERR, "Target pipe is not a client pipe");
		return 0;
	}
	if (!bClientPipe) 
	{
		DEBUG_LOG(LL_ERR, "Must be called from the client side");
		return 0;
	}

    DWORD dwMaxCollectionCount = 0;
	auto bSuccess = ::GetNamedPipeHandleStateA(m_hPipe, NULL, NULL, &dwMaxCollectionCount, NULL, NULL, 0);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "GetMaxCollectionCount failed, GetLastError returned %d\n", ::GetLastError());
		return 0;
	}

    return dwMaxCollectionCount;
}

DWORD CNamedPipe::GetCollectionTimeout() const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return 0;
	}

	BOOL bClientPipe;
	if (!IsClientPipe(bClientPipe)) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe is not a client pipe");
		return 0;
	}
	if (!bClientPipe) 
	{
		DEBUG_LOG(LL_ERR, "Must be called from the client side");
		return 0;
	}

    DWORD dwCollectDataTimeout = 0;
	auto bSuccess = ::GetNamedPipeHandleStateA(m_hPipe, NULL, NULL, NULL, &dwCollectDataTimeout, NULL, 0);
	if (!bSuccess) {
		DEBUG_LOG(LL_ERR, "GetCollectionTimeout failed, GetLastError returned %d\n", ::GetLastError());
		return 0;
	}

    return dwCollectDataTimeout;
}

DWORD CNamedPipe::GetOutboundBufferSize() const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return 0;
	}

	DWORD dwOutBufferSize = 0;
	auto bSuccess = ::GetNamedPipeInfo(m_hPipe, NULL, &dwOutBufferSize, NULL, NULL);
	if (!bSuccess) {
		DEBUG_LOG(LL_ERR, "GetOutboundBufferSize failed, GetLastError returned %d\n", ::GetLastError());
		return 0;
	}

    return dwOutBufferSize;
}

DWORD CNamedPipe::GetInboundBufferSize() const
{
	if (!m_hPipe)
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return 0;
	}

	DWORD dwInBufferSize = 0;
	auto bSuccess = ::GetNamedPipeInfo(m_hPipe, NULL, NULL, &dwInBufferSize, NULL);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "GetInboundBufferSize failed, GetLastError returned %d\n", ::GetLastError());
		return 0;
	}

    return dwInBufferSize;
}

std::string CNamedPipe::GetClientUserName() const
{
	if (!m_hPipe)
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return std::string("");
	}

	BOOL bServerPipe;
	if (!IsServerPipe(bServerPipe)) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe is not a server pipe");
		return std::string("");
	}
	if (!bServerPipe)
	{
		DEBUG_LOG(LL_ERR, "Must be called from the server side");
		return std::string("");
	}

    char pszUserName[_MAX_PATH];
	auto bSuccess = ::GetNamedPipeHandleStateA(m_hPipe, NULL, NULL, NULL, NULL, pszUserName, _MAX_PATH);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "GetClientUserName failed, GetLastError returned %d\n", ::GetLastError());
		return std::string("");
	}

    return pszUserName;
}

DWORD CNamedPipe::GetMaxInstances() const
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return 0;
	}
	
	DWORD dwMaxInstances = 0;
    auto bSuccess = ::GetNamedPipeInfo(m_hPipe, NULL, NULL, NULL, &dwMaxInstances);
	if (!bSuccess)
	{
		DEBUG_LOG(LL_ERR, "GetMaxInstances failed, GetLastError returned %d\n", ::GetLastError());
		return 0;
	}
    return dwMaxInstances;
}

bool CNamedPipe::SetMode(BOOL bByteMode, BOOL bBlockingMode)
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}
	
	DWORD dwMode;
    if (bByteMode)
    {
        if (bBlockingMode)
            dwMode = PIPE_READMODE_BYTE | PIPE_WAIT;
        else
            dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    }
    else
    {
        if (bBlockingMode)
            dwMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
        else
            dwMode = PIPE_READMODE_MESSAGE | PIPE_NOWAIT;
    }

	auto bSuccess = ::SetNamedPipeHandleState(m_hPipe, &dwMode, NULL, NULL);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "SetMode failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::SetMaxCollectionCount(DWORD dwCollectionCount)
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}

	BOOL bClientPipe;
	if (!IsClientPipe(bClientPipe)) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe is not a client pipe");
		return false;
	}
	if (!bClientPipe)
	{
		DEBUG_LOG(LL_ERR, "Must be called from the client side");
		return false;
	}

	auto bSuccess = ::SetNamedPipeHandleState(m_hPipe, NULL, &dwCollectionCount, NULL);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "SetMaxCollectionCount failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}
    return true;
}

bool CNamedPipe::SetCollectionTimeout(DWORD dwDataTimeout)
{
	if (!m_hPipe) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe must be open");
		return false;
	}

    BOOL bClientPipe;
	if (!IsClientPipe(bClientPipe)) 
	{
		DEBUG_LOG(LL_ERR, "Target pipe is not a client pipe");
		return false;
	}
	if (!bClientPipe) 
	{
		DEBUG_LOG(LL_ERR, "Must be called from the client side");
		return false;
	}

	auto bSuccess = ::SetNamedPipeHandleState(m_hPipe, NULL, NULL, &dwDataTimeout);
	if (!bSuccess) 
	{
		DEBUG_LOG(LL_ERR, "SetCollectionTimeout failed, GetLastError returned %d\n", ::GetLastError());
		return false;
	}

    return true;
}

bool CNamedPipe::Call(LPVOID lpInBuffer,  DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize,  DWORD& dwBytesRead, DWORD dwTimeOut)
{
	auto szPipeName = "\\\\.\\pipe\\" + m_szPipeName;

	auto bSuccess = ::CallNamedPipeA(szPipeName.c_str(), lpInBuffer, dwInBufferSize, lpOutBuffer, dwOutBufferSize, &dwBytesRead, dwTimeOut);
	if (!bSuccess)
	{
		DEBUG_LOG(LL_ERR, "CallNamedPipeA fail!, Errpr code: %u", g_winapiApiTable->GetLastError());
	}

    return bSuccess;
}

bool CNamedPipe::IsServerAvailable(DWORD dwTimeOut)
{
	auto szPipeName = "\\\\.\\pipe\\" + m_szPipeName;

	auto bSuccess = ::WaitNamedPipeA(szPipeName.c_str(), dwTimeOut);
	if (!bSuccess)
	{
		DEBUG_LOG(LL_ERR, "WaitNamedPipeA fail!, Error code: %u", g_winapiApiTable->GetLastError());
	}

    return bSuccess;
}

