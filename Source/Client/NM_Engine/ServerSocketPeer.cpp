#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "SocketConnection.h"
#include "SocketServer.h"
#include "ServerSocketPeer.h"
#include "Defines.h"
#include "RSA.h"
#include "AES256.h"
#include "NetLib.h"
#include "BasicCrypt.h"
#include "../../Common/SocketPackets.h"

#include <xxhash\xxhash.h>

CNetworkPeer::CNetworkPeer(std::shared_ptr <CNetworkServer> server) :
	NetPeer(server->GetServiceInstance()()), m_pServer(server), m_uiLoginKey(0), m_bRSACryptKeyCompleted(false), m_bPreCryptCompleted(false), 
	m_pPhase(EPeerPhaseList::PHASE_NULL), m_pConnType(ENoMercyConnectionTypes::NM_CONNECTION_TYPE_NULL)
{
	memset(&m_pRSACryptKey, 0, NET_CRYPT_KEY_LENGTH + 1);
	memset(&m_pPreCryptKey, 0, NET_CRYPT_KEY_LENGTH + 1);
}
CNetworkPeer::~CNetworkPeer()
{
}


std::shared_ptr<CNetworkServer> CNetworkPeer::GetServer() const
{
	return m_pServer.lock();
}
int32_t CNetworkPeer::GetId() const
{
	return UniqueId();
}

void CNetworkPeer::SetPeerPhase(const int8_t phase)
{
	m_pPhase = phase;
}
int8_t CNetworkPeer::GetPeerPhase() const
{
	return m_pPhase;
}

void CNetworkPeer::SetRSAKey(const uint8_t * key)
{
	memcpy(&m_pRSACryptKey, key, NET_CRYPT_KEY_LENGTH);
	m_bRSACryptKeyCompleted = true;
}
const uint8_t * CNetworkPeer::GetRSAKey() const
{
	return &m_pRSACryptKey[0];
}
bool CNetworkPeer::IsRSAKeyInitialized() const
{
	return m_bRSACryptKeyCompleted;
}

void CNetworkPeer::SetPreCryptKey(const uint8_t * key)
{
	memcpy(&m_pPreCryptKey, key, NET_CRYPT_KEY_LENGTH);
	m_bPreCryptCompleted = true;
}
const uint8_t * CNetworkPeer::GetPreCryptKey() const
{
	return &m_pPreCryptKey[0];
}
bool CNetworkPeer::IsPreCryptKeyInitialized() const
{
	return m_bPreCryptCompleted;
}

void CNetworkPeer::SetConnectionType(int8_t connType)
{
	m_pConnType = connType;
}
int8_t CNetworkPeer::GetConnectionType() const
{
	return m_pConnType;
}

DataBuffer CNetworkPeer::BuildRSAKey(const uint8_t* pPublicExponent, uint32_t ulPublicExponentSize, const uint8_t* pModulus, uint32_t ulModulusSize)
{
	DataBuffer pCryptKey;
	try
	{
		auto _pPublicExponent = DataBuffer(pPublicExponent, ulPublicExponentSize);
		auto _pModulus = DataBuffer(pModulus, ulModulusSize);

		// Create rsa key
		m_pRSAServer.create_keypair();
		m_pRSAServer.create_crypt_key();
		pCryptKey = m_pRSAServer.export_wrap_crypt_key(_pPublicExponent, _pModulus);
	}
	catch (std::exception& e)
	{
#ifdef _DEBUG
		DEBUG_LOG(LL_CRI, "Build RSA key exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif
	}
	return pCryptKey;
}
Secret CNetworkPeer::GetRSACtx()
{
	Secret pCryptKey;
	try
	{
		pCryptKey = m_pRSAServer.get_crypt_key();
	}
	catch (std::exception& e)
	{
#ifdef _DEBUG
		DEBUG_LOG(LL_CRI, "Get RSA key exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif
	}
	return pCryptKey;
}

void CNetworkPeer::OnConnect()
{
	NETWORK_LOG(LL_SYS, "Client connected %s (%d)", GetIP().c_str(), GetId());

	SetPeerPhase(PHASE_CONN_REQUEST);
	BeginRead();
}
void CNetworkPeer::OnDisconnect(const boost::system::error_code& e)
{
	NETWORK_LOG(LL_SYS, "Client disconnected %s (%d) Reason: %d", GetIP().c_str(), GetId(), e.value());

	auto server = GetServer();
	if (server)
	{
		server->RemovePeer(GetId());
	}
}


std::size_t CNetworkPeer::OnRead(const void* data, std::size_t length)
{
	std::size_t ulResult = 0;

	auto pServer = GetServer();
	if (!IS_VALID_SMART_PTR(pServer))
	{
		NETWORK_LOG(LL_CRI, "Null server ptr!");

		boost::system::error_code e;
		Disconnect(e);

		return ulResult;
	}

	if (!data || !length) 
	{
		NETWORK_LOG(LL_CRI, "Null data read!");
		return ulResult;
	}

	const auto packet = reinterpret_cast<const TPacketSingleton*>(data);
	NETWORK_LOG(LL_SYS, "Data: %p - %u Packet: %d - %s", data, length, packet->uiPacketID, GetPacketString(packet->uiPacketID).c_str());

	ulResult = pServer->ProcessInput(std::static_pointer_cast<CNetworkPeer>(shared_from_this()), data, length);

	return ulResult;
}

std::size_t CNetworkPeer::OnWritePre(const void* pData, std::size_t ulSize)
{
	if (!pData || !ulSize)
	{
		NETWORK_LOG(LL_CRI, "Null data writed!");
		return ulSize;
	}

	if (pData)
	{
		if (ulSize >= sizeof(TPacketSingleton))
		{
			auto pPsuedoPacket = reinterpret_cast<const TPacketSingleton*>(pData);
			NETWORK_LOG(LL_SYS, "%p - %u | %d - %s", pData, ulSize, pPsuedoPacket->uiPacketID, GetPacketString(pPsuedoPacket->uiPacketID).c_str());

			if (ulSize == GetPacketCapacity(pPsuedoPacket->uiPacketID))
			{
				auto pPacketMagic = (*(uint32_t*)gs_pNetworkMagicValue);
				(*(uint32_t*)((uint32_t)pData)) = pPacketMagic;

				NETWORK_LOG(LL_SYS, "Magic: %p", pPacketMagic);

				auto pPacketSum = g_nmApp->NetworkMgrInstance()->CreateChecksum(pData, ulSize - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
				(*(uint32_t*)((uint32_t)pData + ulSize - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */))) = pPacketSum;

				NETWORK_LOG(LL_SYS, "Sum: %p", pPacketSum);

				auto pPacketHash = XXH32(pData, ulSize - NET_CHECKSUM_LENGTH, NET_PACKET_HASH_MAGIC);
				(*(uint32_t*)((uint32_t)pData + ulSize - NET_CHECKSUM_LENGTH)) = pPacketHash;

				NETWORK_LOG(LL_SYS, "Hash: %p", pPacketHash);
			}
		}
	}
	return ulSize;
}

void CNetworkPeer::OnWritePost(bool bCompleted, const std::string & szRequestId)
{
	NETWORK_LOG(LL_SYS, "Write completed! Result: %d Request ID: %s", bCompleted, szRequestId.c_str());
}
void CNetworkPeer::OnError(std::uint32_t ulErrorType, const boost::system::error_code & e)
{
	NETWORK_LOG(LL_ERR, "Network error handled! ID: %u System error: %u(%s)", ulErrorType, e.value(), e.message().c_str());
}


void CNetworkPeer::SendCrypted(std::uint32_t ulPacketID, void* pData, std::size_t ulSize)
{
	NETWORK_LOG(LL_SYS, "Original data: %p-%u", pData, ulSize);

	// Process packet security contents
	auto pPacketMagic = (*(uint32_t*)gs_pNetworkMagicValue);
	(*(uint32_t*)((uint32_t)pData)) = pPacketMagic;

	NETWORK_LOG(LL_SYS, "Magic: %p", pPacketMagic);

	auto pPacketSum = g_nmApp->NetworkMgrInstance()->CreateChecksum(pData, ulSize - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
	(*(uint32_t*)((uint32_t)pData + ulSize - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */))) = pPacketSum;

	NETWORK_LOG(LL_SYS, "Sum: %p", pPacketSum);

	auto pPacketHash = XXH32(pData, ulSize - NET_CHECKSUM_LENGTH, NET_PACKET_HASH_MAGIC);
	(*(uint32_t*)((uint32_t)pData + ulSize - NET_CHECKSUM_LENGTH)) = pPacketHash;

	NETWORK_LOG(LL_SYS, "Hash: %p", pPacketHash);


	if (m_bRSACryptKeyCompleted || m_bPreCryptCompleted)
	{
		// Crypt packet content
		auto cryptedPacket = NAES256::Encrypt(reinterpret_cast<const uint8_t *>(pData), ulSize, m_bRSACryptKeyCompleted ? m_pRSACryptKey : m_pPreCryptKey);
		NETWORK_LOG(LL_SYS, "Crpyted data: %p-%u", cryptedPacket.get_data(), cryptedPacket.get_size());

		auto cryptedPacketData = std::make_unique<TCryptedPacket>();
		if (IS_VALID_SMART_PTR(cryptedPacketData))
		{
			memcpy(cryptedPacketData->pContext, cryptedPacket.get_data(), cryptedPacket.get_size());

			cryptedPacketData->uiDecryptedPacketId	= ulPacketID;
			cryptedPacketData->ulCryptedSize		= cryptedPacket.get_size();
			cryptedPacketData->ulDecryptedSize		= ulSize;
			cryptedPacketData->ulCryptedSum			= XXH32(cryptedPacket.get_data(), cryptedPacket.get_size(), NET_PACKET_HASH_MAGIC);
			cryptedPacketData->ulDecryptedSum		= XXH32(pData, ulSize - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */), NET_PACKET_HASH_MAGIC);

			NETWORK_LOG(LL_SYS, "Built crpyted packet! Data: %p Size: %u Container size: %u", cryptedPacketData.get(), cryptedPacket.get_size(), cryptedPacketData->size());

			Send(cryptedPacketData.get(), cryptedPacketData->size());
		}
	}
	else
	{
		Send(pData, ulSize);
		return;
	}
}


void CNetworkPeer::Destroy()
{
	GetServer()->RemovePeer(GetId());
}


void CNetworkPeer::SendPreCryptKey()
{
	auto pDynamicKey = g_nmApp->FunctionsInstance()->GetRandomData(NET_CRYPT_KEY_LENGTH);
//	EncryptBuffer(pDynamicKey, NET_CRYPT_KEY_LENGTH, NETWORK_BODY_CRYPT_KEY);

	auto preCryptKeyPacket = std::make_shared<TPacketPreCryptKeyInfo>();
	if (IS_VALID_SMART_PTR(preCryptKeyPacket))
	{
		memcpy(preCryptKeyPacket->pMagic, gs_pNetworkMagicValue, sizeof(preCryptKeyPacket->pMagic));
		preCryptKeyPacket->uiPacketID = HEADER_SC_PRE_KEY_INFO;

		// First arg | Dummy
		auto ulRandomDataSize = 36; // g_nmApp->FunctionsInstance()->GetRandomInt(24, 36);
		auto pFirstDummyData = g_nmApp->FunctionsInstance()->GetRandomData(ulRandomDataSize);
		memcpy(preCryptKeyPacket->dummyDataFirst, pFirstDummyData, ulRandomDataSize);

		// Second Arg | Pre crypt key
		memcpy(preCryptKeyPacket->preCryptKey, pDynamicKey, NET_CRYPT_KEY_LENGTH);

		// Third arg | Dummy
		auto ulRandomDataSize2 = g_nmApp->FunctionsInstance()->GetRandomInt(36, 48);
		auto pLastDummyData = g_nmApp->FunctionsInstance()->GetRandomData(ulRandomDataSize2);
		memcpy(preCryptKeyPacket->dummyDataLast, pLastDummyData, ulRandomDataSize2);

		SendCrypted(HEADER_SC_PRE_KEY_INFO, preCryptKeyPacket.get(), preCryptKeyPacket->size());

		// Register key
		this->SetPreCryptKey(pDynamicKey);

		NETWORK_LOG(LL_SYS, "Sent pre crypt key!");
	}
}
void CNetworkPeer::SendRSAKeyInfo(DataBuffer cryptKey)
{
	auto keyInfoPacket = std::make_shared<TPacketRsaKeyInfo>();
	if (IS_VALID_SMART_PTR(keyInfoPacket))
	{
		memcpy(keyInfoPacket->pMagic, gs_pNetworkMagicValue, sizeof(keyInfoPacket->pMagic));
		keyInfoPacket->uiPacketID = HEADER_SC_RSA_KEY_INFO;
		memcpy(keyInfoPacket->cryptkey, cryptKey.get_data(), cryptKey.get_size());
		keyInfoPacket->cryptkeysize = cryptKey.get_size();

		SendCrypted(HEADER_SC_RSA_KEY_INFO, keyInfoPacket.get(), keyInfoPacket->size());

		NETWORK_LOG(LL_SYS, "Sent rsa key info packet size: %u", cryptKey.get_size());
	}
}
void CNetworkPeer::SendRSACompleteNotification(bool bCompleted)
{
	auto completeNotificationPacket = std::make_shared<TPacketRsaCheckCompleted>();
	if (IS_VALID_SMART_PTR(completeNotificationPacket))
	{
		memcpy(completeNotificationPacket->pMagic, gs_pNetworkMagicValue, sizeof(completeNotificationPacket->pMagic));
		completeNotificationPacket->uiPacketID = HEADER_SC_RSA_CHECK_COMPLETED;
		completeNotificationPacket->bCompleted = bCompleted;
		completeNotificationPacket->dwServerVersion = std::atol(GetServer()->GetServerVersion().c_str());

		SendCrypted(HEADER_SC_RSA_CHECK_COMPLETED, completeNotificationPacket.get(), completeNotificationPacket->size());

		NETWORK_LOG(LL_SYS, "Sent rsa init complete notification");
	}
}
void CNetworkPeer::SendRoutinePacket()
{
	auto routinePacket = std::make_shared<TPacketRoutinePacket>();
	if (IS_VALID_SMART_PTR(routinePacket))
	{
		memcpy(routinePacket->pMagic, gs_pNetworkMagicValue, sizeof(routinePacket->pMagic));
		routinePacket->uiPacketID = HEADER_SC_ROUTINE_PACKET;

		SendCrypted(HEADER_SC_ROUTINE_PACKET, routinePacket.get(), routinePacket->size());

		NETWORK_LOG(LL_SYS, "Sent routine packet");
	}
}
void CNetworkPeer::SendBlackLists()
{
	auto blacklistPacket = std::make_shared<TPacketCheatBlackLists>();
	if (IS_VALID_SMART_PTR(blacklistPacket))
	{
		memcpy(blacklistPacket->pMagic, gs_pNetworkMagicValue, sizeof(blacklistPacket->pMagic));
		blacklistPacket->uiPacketID = HEADER_SC_CHEAT_BLACKLISTS;
		strcpy(blacklistPacket->ssBlackList1, GetServer()->GetBlackList(1).c_str());
		strcpy(blacklistPacket->ssBlackList2, GetServer()->GetBlackList(2).c_str());
		strcpy(blacklistPacket->ssBlackList3, GetServer()->GetBlackList(3).c_str());
		strcpy(blacklistPacket->ssBlackList4, GetServer()->GetBlackList(4).c_str());
		strcpy(blacklistPacket->ssBlackList5, GetServer()->GetBlackList(5).c_str());

		SendCrypted(HEADER_SC_CHEAT_BLACKLISTS, blacklistPacket.get(), blacklistPacket->size());

		NETWORK_LOG(LL_SYS, "Sent blacklist packet");
	}
}

