#pragma once

class CCommPortPacket 
{
	public:
		CCommPortPacket() = default;
		~CCommPortPacket() = default;

		virtual PVOID GetHeader() = 0;
		virtual PVOID GetData() = 0;
		virtual ULONG GetSize() const = 0;
};

template <typename T>
class CMessagePacket : public CCommPortPacket 
{
	public:
		CMessagePacket() : 
			Packet({})
		{
		}
		~CMessagePacket()
		{
		}

		PVOID GetHeader() override		{ return static_cast<PVOID>(&Packet.Header); }
		PVOID GetData() override		{ return static_cast<PVOID>(&Packet.Data); }
		ULONG GetSize() const override	{ return sizeof(Packet); }

		ULONG GetReplyLength() const	{ return Packet.Header.ReplyLength; }
		ULONGLONG GetMessageId() const	{ return Packet.Header.MessageId; }

	private:
		struct 
		{
			FILTER_MESSAGE_HEADER Header;
			T Data;
		} Packet;
};

template <typename T>
class CReplyPacket : public CCommPortPacket 
{
	public:
		CReplyPacket() :
			CCommPortPacket(), Packet({})
		{
		}
		CReplyPacket(CCommPortPacket& Message, ULONG Status) :
			CReplyPacket() 
		{
			SetMessageId(static_cast<PFILTER_MESSAGE_HEADER>(Message.GetHeader())->MessageId);
			SetReplyStatus(Status);
		}
		CReplyPacket(CCommPortPacket& Message, ULONG Status, const T& Data) : 
			CReplyPacket(Message, Status)
		{
			SetData(Data);
		}
		~CReplyPacket() 
		{
		}
	
		PVOID GetData() override		{ return static_cast<PVOID>(&Packet.Data); }
		PVOID GetHeader() override		{ return static_cast<PVOID>(&Packet.Header); }
		ULONG GetSize() const override	{ return sizeof(Packet); }
	
		VOID SetData(const T& Data)				{ Packet.Data = Data; }
		VOID SetReplyStatus(NTSTATUS Status)	{ Packet.Header.Status = Status; }
		VOID SetMessageId(ULONGLONG MessageId)	{ Packet.Header.MessageId = MessageId; }
	private:
		struct 
		{
			FILTER_REPLY_HEADER Header;
			T Data;
		} Packet;
};

class CCommPort
{
	public:
		CCommPort();
		~CCommPort();

		HRESULT Connect(LPCWSTR PortName, PVOID Context, WORD SizeOfContext);
		VOID Disconnect();

		HRESULT Send(IN PVOID Input, DWORD InputSize, OUT PVOID Output, DWORD OutputSize, OUT OPTIONAL PULONG ReturnLength = NULL);
		HRESULT Recv(_Out_ CCommPortPacket& ReceivedMessage);
		HRESULT Reply(_In_ CCommPortPacket& ReplyMessage);

	private:
		HANDLE hPort;
		BOOL Connected;
};

