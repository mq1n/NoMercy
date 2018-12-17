#include <fltKernel.h>

#include "MemoryUtils.h"
#include "Locks.h"
#include "LinkedList.h"
#include "CommPort.h"
#include "WdkTypes.h"

CommPort::CommPort() : 
	ParentFilter(NULL), ServerCookie({}), ServerPort(NULL), FilterClients(), OnMessageCallback(NULL)
{
}
CommPort::~CommPort()
{
	StopServer();
}


NTSTATUS CommPort::StartServer(PFLT_FILTER Filter, LPCWSTR PortName,_OnMessage OnMessage,LONG MaxConnections, OPTIONAL PVOID Cookie) 
{
	if (ServerPort)
		StopServer();

	ParentFilter = Filter;
	OnMessageCallback = OnMessage;

	ServerCookie.ServerInstance = this;
	ServerCookie.UserCookie = Cookie;

	UNICODE_STRING Name = {};
	RtlInitUnicodeString(&Name, PortName);

	PSECURITY_DESCRIPTOR SecurityDescriptor = NULL;
	NTSTATUS Status = FltBuildDefaultSecurityDescriptor(&SecurityDescriptor, FLT_PORT_ALL_ACCESS);
	if (!NT_SUCCESS(Status)) 
	{
		KdPrint(("[Kernel-Bridge]: Create security descriptor failure (0x%X)\r\n", Status));
		return Status;
	}

	OBJECT_ATTRIBUTES ObjectAttributes = {};
	InitializeObjectAttributes(&ObjectAttributes, &Name, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, SecurityDescriptor);

	Status = FltCreateCommunicationPort(ParentFilter, &ServerPort, &ObjectAttributes, &ServerCookie, OnConnectInternal, OnDisconnectInternal,OnMessageInternal, MaxConnections);

	FltFreeSecurityDescriptor(SecurityDescriptor);

	if (NT_SUCCESS(Status))
		KdPrint(("[Kernel-Bridge]: Comm.Port created!\r\n"));
	else
		KdPrint(("[Kernel-Bridge]: Comm.Port failure: 0x%X\r\n", Status));
	
	return Status;
}

VOID CommPort::StopServer() 
{
	if (ServerPort) 
		FltCloseCommunicationPort(ServerPort);

	// Disconnecting all connected clients:
	FilterClients.LockExclusive();

	for (auto& Client : FilterClients)
	{
		FltCloseClientPort(ParentFilter, &Client.ClientPort);
	}

	FilterClients.Unlock();
}

NTSTATUS CommPort::OnConnectInternal(IN PFLT_PORT ClientPort, IN PVOID ServerPortCookie, IN PVOID ConnectionContext, IN ULONG SizeOfContext, OUT PVOID *ConnectionPortCookie)
{
	SysInfoLog("[NoMercy]: Comm.Port OnConnect from: %lld\n", reinterpret_cast<UINT64>(PsGetCurrentProcessId()));

	auto ServerCookie = static_cast<SERVER_COOKIE*>(ServerPortCookie);

	FILTER_CLIENT_INFO FilterClient = {};
	FilterClient.ServerInstance = ServerCookie->ServerInstance;
	FilterClient.ClientPort = ClientPort;
	
	if (ConnectionContext && SizeOfContext) 
	{ 
		PVOID ContextBuffer = VirtualMemory::AllocFromPool(SizeOfContext);
		RtlCopyMemory(ContextBuffer, ConnectionContext, SizeOfContext);

		FilterClient.ConnectionContext = ContextBuffer;
		FilterClient.SizeOfContext = SizeOfContext;
	}

	auto ServerInstance = static_cast<CommPort*>(ServerCookie->ServerInstance);
	// Add 'Client' to filter clients list:

	ServerInstance->FilterClients.LockExclusive();

	*ConnectionPortCookie = static_cast<PVOID>(ServerInstance->FilterClients.InsertTail(FilterClient));

	ServerInstance->FilterClients.Unlock();

	return STATUS_SUCCESS;
}

VOID CommPort::OnDisconnectInternal(IN PVOID ConnectionContext)
{
	SysInfoLog("[NoMercy]: Comm.Port OnDisconnect from: %lld\n", reinterpret_cast<UINT64>(PsGetCurrentProcessId()));

	// Free client-specific info:
	auto ClientEntry = static_cast<FilterClientsList::ListEntry*>(ConnectionContext);
	if (ClientEntry->GetValue()->ClientPort) 
		FltCloseClientPort(ClientEntry->GetValue()->ServerInstance->ParentFilter, &ClientEntry->GetValue()->ClientPort);

	if (ClientEntry->GetValue()->ConnectionContext && ClientEntry->GetValue()->SizeOfContext) 
	{
		VirtualMemory::FreePoolMemory(ClientEntry->GetValue()->ConnectionContext);
	}

	CommPort* ServerInstance = ClientEntry->GetValue()->ServerInstance;

	// Unlink client from filter clients list:
	ServerInstance->FilterClients.LockExclusive();
	ServerInstance->FilterClients.Remove(ClientEntry);
	ServerInstance->FilterClients.Unlock();
}

NTSTATUS CommPort::OnMessageInternal(IN PVOID PortCookie, IN PVOID InputBuffer OPTIONAL, IN ULONG InputBufferLength, OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength, OUT PULONG ReturnOutputBufferLength)
{
	auto ClientEntry = static_cast<FilterClientsList::ListEntry*>(PortCookie);

	_OnMessage Handler = ClientEntry->GetValue()->ServerInstance->OnMessageCallback;
	if (Handler)
	{ 
		FILTER_CLIENT_REQUEST Request = {};
		Request.InputBuffer = InputBuffer;
		Request.InputSize = InputBufferLength;
		Request.OutputBuffer = OutputBuffer;
		Request.OutputSize = OutputBufferLength;
		return Handler(*ClientEntry->GetValue(), Request, ReturnOutputBufferLength);
	}

	return STATUS_SUCCESS;
}

NTSTATUS CommPort::Send(PFLT_PORT Client,  IN PVOID Buffer,  ULONG Size,  OUT PVOID Response,  ULONG ResponseSize, ULONG MsecTimeout)
{
	if (MsecTimeout == 0xFFFFFFFF) // Infinite wait:
		return FltSendMessage(ParentFilter, &Client, Buffer, Size, Response, &ResponseSize, NULL);

	LARGE_INTEGER _Timeout; // In 100-ns units
	_Timeout.QuadPart = - static_cast<INT64>(MsecTimeout) * 10 * 1000; // Relative time is negative!

	return FltSendMessage(ParentFilter, &Client, Buffer, Size, Response, &ResponseSize, &_Timeout);
}

