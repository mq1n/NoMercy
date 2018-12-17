#pragma once
#include <Windows.h>
#include <string>

class CNamedPipe
{
	public:
		CNamedPipe();
		CNamedPipe(const std::string & szPipeName);
		~CNamedPipe();

		HANDLE Get() const
		{
			return m_hPipe;
		};

		bool		Create(DWORD dwOpenMode, DWORD dwPipeMode, DWORD dwMaxInstances, DWORD dwOutBufferSize, DWORD dwInBufferSize, DWORD dwDefaultTimeOut);
		bool		Open(DWORD dwDesiredAccess, DWORD dwShareMode);

		bool		Close();
		bool		Attach(HANDLE hPipe);
		HANDLE		Detach();

		bool		ConnectClient(LPOVERLAPPED lpOverlapped = NULL);
		bool		DisconnectClient();
		bool		Flush();
		bool		Peek(LPVOID lpBuffer, DWORD dwBufferSize, DWORD& dwBytesRead,  DWORD& dwTotalBytesAvail, DWORD& dwBytesLeftThisMessage);
		bool		Transact(LPVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize, DWORD& dwBytesRead);

		template <class T> 
		bool Write(T& data) { return Write(&data, sizeof(T)); };
		bool Write(LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite);

		template <class T> 
		bool Read(T& data) { return Read(&data, sizeof(T)); };
		bool Read(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);

		bool IsOpen() const
		{
			return (m_hPipe != INVALID_HANDLE_VALUE);
		};

		bool		IsBlockingPipe(BOOL& bIsBlocking) const;
		bool		IsClientPipe(BOOL& bClientPipe) const;
		bool		IsServerPipe(BOOL& bServerPipe) const;
		bool		IsMessagePipe(BOOL& bMessagePipe) const;
		DWORD		GetCurrentInstances() const;
		DWORD		GetMaxCollectionCount() const;
		DWORD		GetCollectionTimeout() const;
		DWORD		GetOutboundBufferSize() const;
		DWORD		GetInboundBufferSize() const;
		std::string GetClientUserName() const;
		DWORD		GetMaxInstances() const;

		bool		SetMode(BOOL bByteMode, BOOL bBlockingMode);
		bool		SetMaxCollectionCount(DWORD dwCollectionCount);
		bool		SetCollectionTimeout(DWORD dwDataTimeout);

		bool		Call(LPVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize, DWORD& dwBytesRead, DWORD dwTimeOut);
		bool		IsServerAvailable(DWORD dwTimeOut);

	private:
		HANDLE		m_hPipe;
		std::string m_szPipeName;
};

