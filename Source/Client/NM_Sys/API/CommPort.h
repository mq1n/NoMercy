#pragma once

class CommPort 
{
	public:
		using SERVER_COOKIE = struct
		{
			CommPort* ServerInstance;
			PVOID UserCookie;
		};

		using FILTER_CLIENT_INFO = struct
		{
			CommPort* ServerInstance;
			PFLT_PORT ClientPort;
			PVOID ConnectionContext;
			ULONG SizeOfContext;
		};

		class FilterClientsList : public EResource, public LinkedList <FILTER_CLIENT_INFO>
		{
			public:
				FilterClientsList() :
					EResource(), LinkedList<FILTER_CLIENT_INFO>()
				{
				}
				~FilterClientsList() = default;
		};
    
		using FILTER_CLIENT_REQUEST = struct 
		{
			IN PVOID InputBuffer;
			ULONG InputSize;
			OUT PVOID OutputBuffer;
			ULONG OutputSize;
		};

		using _OnMessage = NTSTATUS(NTAPI*)(FILTER_CLIENT_INFO& Client, FILTER_CLIENT_REQUEST& Request, OUT PULONG ReturnLength);

	public:
		CommPort();
		~CommPort();

		CommPort(const CommPort&) = delete;
		CommPort(CommPort&&) = delete;
		CommPort& operator = (const CommPort&) = delete;
		CommPort& operator = (CommPort&&) = delete;

		NTSTATUS StartServer(PFLT_FILTER Filter,  LPCWSTR PortName,  _OnMessage OnMessage, LONG MaxConnections = 128, OPTIONAL PVOID Cookie = NULL);

		VOID StopServer();

		FilterClientsList & GetFilterClients() { return FilterClients; };

		NTSTATUS Send(PFLT_PORT Client, IN PVOID Buffer, ULONG Size, OUT PVOID Response = NULL, ULONG ResponseSize = 0, ULONG Timeout = 0);

	protected:
		static NTSTATUS FLTAPI OnConnectInternal(IN PFLT_PORT ClientPort, IN PVOID ServerPortCookie, IN PVOID ConnectionContext, IN ULONG SizeOfContext, OUT PVOID *ConnectionPortCookie);

		static VOID FLTAPI OnDisconnectInternal(IN PVOID ConnectionCookie);

		static NTSTATUS FLTAPI OnMessageInternal(IN PVOID PortCookie, IN PVOID InputBuffer OPTIONAL, IN ULONG InputBufferLength, OUT PVOID OutputBuffer OPTIONAL,
			IN ULONG OutputBufferLength, OUT PULONG ReturnOutputBufferLength);

	private:
		PFLT_FILTER ParentFilter;

		SERVER_COOKIE ServerCookie;

		PFLT_PORT ServerPort;

		FilterClientsList FilterClients;

		_OnMessage OnMessageCallback;
};

