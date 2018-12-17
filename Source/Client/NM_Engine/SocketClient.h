#pragma once
#include "NetLib.h"
#include "RSA.h"
#include "WorkQueue.h"

extern std::shared_ptr <CCheatDetectQueue> g_pkUnprocessedCheatObjects;

class CNetworkClient : public NetClient
{
	private:
		typedef std::unordered_map <uint8_t, /* header */ std::pair< /* Callback */ std::size_t, std::function<std::size_t /* Placeholders */(const void*, /* data */ std::size_t /* size */)>>> MapPacketHandlers;

	public:
		CNetworkClient(NetService& service);
		virtual ~CNetworkClient() = default;

		virtual void			OnConnect();
		virtual void			OnDisconnect(const boost::system::error_code& e);
		virtual std::size_t		OnRead(const void* data, std::size_t length);
		virtual std::size_t		OnWritePre(const void* data, std::size_t length);
		virtual void			OnWritePost(bool bCompleted, const std::string & szRequestId);
		virtual void			OnError(std::uint32_t ulErrorType, const boost::system::error_code & e);

		void SendCrypted(std::uint32_t ulPacketID, void* pData, std::size_t ulSize);

		std::size_t ProcessInput(const void* data, std::size_t maxlength);
	
		std::size_t RecvPreCryptKeyInfo(const void* data, std::size_t maxlength);
		std::size_t RecvRSAKeyInfo(const void* data, std::size_t maxlength);
		std::size_t RecvEncryptionReadyNotification(const void* data, std::size_t maxlength);
		std::size_t RecvRoutinePacket(const void* data, std::size_t maxlength);
		std::size_t RecvCryptedPacket(const void* data, std::size_t maxlength);
		std::size_t RecvCheatBlacklists(const void* data, std::size_t maxlength);
		std::size_t RecvNotificationPacket(const void* data, std::size_t maxlength);

		bool CreateQueueWatcherThread();


		template <typename PacketT, typename HandlerT>
		inline void RegisterPacket(uint8_t type, HandlerT handler)
		{
			m_pHandlers.insert(std::make_pair(type, std::make_pair(PacketT::size(), handler)));
		}

		inline void RemovePacket(uint8_t type)
		{
			auto it = m_pHandlers.find(type);
			if (it != m_pHandlers.end())
				m_pHandlers.erase(it);
		}

	protected:
		DWORD					NetQueueWorkerRoutine(void);
		static DWORD WINAPI		StartThreadRoutine(LPVOID lpParam);

	private:
		mutable std::recursive_mutex	m_Mutex;

		bool							m_bGotRoutinePacket;

		Crypt_Client					m_pRSAClient;
		uint8_t							m_pRSAKey[NET_CRYPT_KEY_LENGTH + 1];
		bool							m_bRSACompleted;
		uint8_t							m_pPreCryptKey[NET_CRYPT_KEY_LENGTH + 1];
		bool							m_bPreCryptCompleted;

		MapPacketHandlers				m_pHandlers;
};
