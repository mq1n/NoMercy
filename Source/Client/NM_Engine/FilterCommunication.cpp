#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "FilterCommunication.h"

CCommPort::CCommPort() : 
	hPort(NULL), Connected(FALSE) 
{
}
CCommPort::~CCommPort() 
{
	Disconnect();
}


HRESULT CCommPort::Connect(LPCWSTR PortName, PVOID Context, WORD SizeOfContext) 
{
	HRESULT Status = FilterConnectCommunicationPort(PortName, 0, Context, SizeOfContext, NULL, &hPort);
	Connected = Status == S_OK;
	return Status;
}

VOID CCommPort::Disconnect() 
{
	if (hPort) 
		CloseHandle(hPort);
}

HRESULT CCommPort::Send(IN PVOID Input, DWORD InputSize, OUT PVOID Output, DWORD OutputSize, OUT OPTIONAL PULONG ReturnLength)
{
	DWORD Returned = 0;
	HRESULT Status = FilterSendMessage(hPort, Input, InputSize, Output, OutputSize, &Returned);

	if (ReturnLength)
		*ReturnLength = Returned;
	return Status;
}

HRESULT CCommPort::Recv(_Out_ CCommPortPacket& ReceivedMessage)
{
	return FilterGetMessage(hPort, static_cast<PFILTER_MESSAGE_HEADER>(ReceivedMessage.GetHeader()), ReceivedMessage.GetSize(), NULL);
}

HRESULT CCommPort::Reply(_In_ CCommPortPacket& ReplyMessage)
{
	return FilterReplyMessage(hPort, static_cast<PFILTER_REPLY_HEADER>(ReplyMessage.GetHeader()), ReplyMessage.GetSize());
}
