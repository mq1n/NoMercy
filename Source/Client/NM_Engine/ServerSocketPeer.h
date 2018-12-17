#pragma once
#include "TIdManager.h"
#include "NetLib.h"
#include "RSA.h"

enum EPeerPhaseList
{
	PHASE_NULL,
	PHASE_CONN_REQUEST,
	PHASE_ENCRYPTED_COMM_READY,
	PHASE_CHECK_COMPLETED,
	PHASE_MAX
};

class CNetworkServer;

class CNetworkPeer : public NetPeer, public IUniqueID <CNetworkPeer, int32_t>
{
	public:
		CNetworkPeer(std::shared_ptr <CNetworkServer> server);
		virtual ~CNetworkPeer();
	
		std::shared_ptr<CNetworkServer> GetServer() const;
		int32_t GetId() const;
		
		virtual void SetPeerPhase(const int8_t phase);
		virtual int8_t GetPeerPhase() const;

		virtual void SetRSAKey(const uint8_t* key);
		virtual const uint8_t* GetRSAKey() const;
		virtual bool IsRSAKeyInitialized() const;
		virtual DataBuffer BuildRSAKey(const uint8_t* pPublicExponent, uint32_t ulPublicExponentSize, const uint8_t* pModulus, uint32_t ulModulusSize);
		virtual Secret GetRSACtx();

		virtual void SetPreCryptKey(const uint8_t* key);
		virtual const uint8_t* GetPreCryptKey() const;
		virtual bool IsPreCryptKeyInitialized() const;

		virtual void SetConnectionType(int8_t connType);
		virtual int8_t GetConnectionType() const;


		// net class callbacks
		virtual void OnConnect();
		virtual void OnDisconnect(const boost::system::error_code& e);
		virtual std::size_t OnRead(const void* data, std::size_t length);
		virtual std::size_t OnWritePre(const void* data, std::size_t length);
		virtual void OnWritePost(bool bCompleted, const std::string & szRequestId);
		virtual void OnError(std::uint32_t ulErrorType, const boost::system::error_code & e);

		// packet crypter & sender
		virtual void SendCrypted(std::uint32_t ulPacketID, void* pData, std::size_t ulSize);
		
		// finalization routine for a peer
		virtual void Destroy();
		
		// packet funcs
		virtual void SendPreCryptKey();
		virtual void SendRSAKeyInfo(DataBuffer cryptKey);
		virtual	void SendRSACompleteNotification(bool bCompleted);
		virtual void SendRoutinePacket();
		virtual void SendBlackLists();
		
	private:
		std::weak_ptr <CNetworkServer>	m_pServer;
		Crypt_Server					m_pRSAServer;
		uint32_t						m_uiLoginKey;
		uint8_t							m_pPreCryptKey[NET_CRYPT_KEY_LENGTH + 1];
		bool							m_bPreCryptCompleted;
		uint8_t							m_pRSACryptKey[NET_CRYPT_KEY_LENGTH + 1];
		bool							m_bRSACryptKeyCompleted;
		int8_t							m_pPhase;
		int8_t							m_pConnType;
};
