#pragma once
#include <Windows.h>
#include <FltTypes.h>
#include "FilterCommunication.h"

#ifndef FILTER_PORT_NAME
	#define FILTER_PORT_NAME L"\\NoMercy"
#endif

template <typename PacketDataType, KbFltTypes PacketType>
class CCommPortListener
{
    using TCallback = std::function<void(CCommPort& Port, CMessagePacket<PacketDataType>& Message)>;

	public:
		CCommPortListener() : 
			m_pPort(), m_hThread(NULL), m_pCallback(NULL), m_hrConnectStatus(ERROR_SUCCESS) 
		{
			m_hSubscriptionEvent = g_winapiApiTable->CreateEventA(NULL, TRUE, FALSE, NULL);
		}
		~CCommPortListener() 
		{
			Unsubscribe();
			g_winapiApiTable->CloseHandle(m_hSubscriptionEvent);
		} 

		bool Subscribe(TCallback pListener)
		{
			if (IS_VALID_HANDLE(m_hThread) || !pListener)
				return false;

			m_pCallback = pListener;

			m_hThread = g_winapiApiTable->CreateThread(NULL, 0, ListenerThread, this, 0, NULL);
			if (!m_hThread)
				return false;

			if (g_winapiApiTable->WaitForSingleObject(m_hSubscriptionEvent, 5000) == WAIT_TIMEOUT)
				return false;

			if (!SUCCEEDED(m_hrConnectStatus))
			{
				g_winapiApiTable->CloseHandle(m_hThread);
				return false;
			}
			g_winapiApiTable->ResetEvent(m_hSubscriptionEvent);

			return SUCCEEDED(m_hrConnectStatus);
		}

		void Unsubscribe()
		{
			if (!IS_VALID_HANDLE(m_hThread))
				return;

			m_pPort.Disconnect();

			if (g_winapiApiTable->WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT)
				g_winapiApiTable->TerminateThread(m_hThread, STATUS_SUCCESS);

			g_winapiApiTable->CloseHandle(m_hThread);
		}

	protected:
		static bool CallCallbackSafe(CCommPortListener* Self, CMessagePacket<PacketDataType>& Message) 
		{
			if (Self->m_pCallback)
			{
//				__try 
//				{
					Self->m_pCallback(Self->m_pPort, Message);
//				}
//				__except (EXCEPTION_EXECUTE_HANDLER) 
//				{
//					return false;
//				}
			}
			return true;
		}

		static DWORD WINAPI ListenerThread(LPVOID lpParam) 
		{
			auto Self = reinterpret_cast<CCommPortListener*>(lpParam);

			KB_FLT_CONTEXT Context = {};
			Context.Type = PacketType;
			Context.Client.ProcessId = g_winapiApiTable->GetCurrentProcessId();
			Context.Client.ThreadId = g_winapiApiTable->GetCurrentThreadId();

			Self->m_hrConnectStatus = Self->m_pPort.Connect(FILTER_PORT_NAME, &Context, sizeof(Context));
			if (!SUCCEEDED(Self->m_hrConnectStatus))
			{
				DEBUG_LOG(LL_ERR, "Can not connected to filter server!");
				g_winapiApiTable->ExitThread(0);
			}

			g_winapiApiTable->SetEvent(Self->m_hSubscriptionEvent);

			HRESULT hr;
			do 
			{
				CMessagePacket<PacketDataType> Message;
				hr = Self->m_pPort.Recv(*reinterpret_cast<CCommPortPacket*>(&Message));

				if (SUCCEEDED(hr))
				{
					CallCallbackSafe(Self, Message);
				}

				g_winapiApiTable->Sleep(1);
			} while (SUCCEEDED(hr));

			g_winapiApiTable->ExitThread(0);
			return 0;
		}

	private:
		HANDLE		m_hThread;
		HANDLE		m_hSubscriptionEvent;
		TCallback	m_pCallback;
		CCommPort	m_pPort;
		HRESULT		m_hrConnectStatus;
};

