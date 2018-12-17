#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "SocketConnection.h"
#include "SocketClient.h"
#include "Defines.h"
#include "AES256.h"
#include "ProcessFunctions.h"
#include "NetLib.h"
#include "BasicCrypt.h"
#include "../../Common/SocketPackets.h"

#include <xxhash/xxhash.h>
#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

std::shared_ptr <CCheatDetectQueue>	g_pkUnprocessedCheatObjects = nullptr; 

CNetworkClient::CNetworkClient(NetService& service) :
	NetClient(service(), 5000), m_bRSACompleted(false), m_bPreCryptCompleted(false), m_bGotRoutinePacket(false)
{
	RegisterPacket <TPacketPreCryptKeyInfo>
	(
		HEADER_SC_PRE_KEY_INFO, 
		std::bind(&CNetworkClient::RecvPreCryptKeyInfo, this, std::placeholders::_1, std::placeholders::_2)
	);
	RegisterPacket <TPacketRsaKeyInfo>
	(
		HEADER_SC_RSA_KEY_INFO, 
		std::bind(&CNetworkClient::RecvRSAKeyInfo, this, std::placeholders::_1, std::placeholders::_2)
	);
	RegisterPacket <TPacketRsaCheckCompleted>
	(
		HEADER_SC_RSA_CHECK_COMPLETED, 
		std::bind(&CNetworkClient::RecvEncryptionReadyNotification, this, std::placeholders::_1, std::placeholders::_2)
	);
	RegisterPacket <TPacketRoutinePacket>
	(
		HEADER_SC_ROUTINE_PACKET,
		std::bind(&CNetworkClient::RecvRoutinePacket, this, std::placeholders::_1, std::placeholders::_2)
	);
	RegisterPacket <TCryptedPacket>
	(
		HEADER_CRYPTED_PACKET,
		std::bind(&CNetworkClient::RecvCryptedPacket, this, std::placeholders::_1, std::placeholders::_2)
	);
	RegisterPacket <TPacketCheatBlackLists>
	(
		HEADER_SC_CHEAT_BLACKLISTS,
		std::bind(&CNetworkClient::RecvCheatBlacklists, this, std::placeholders::_1, std::placeholders::_2)
	);
	RegisterPacket <TPacketNotification>
	(
		HEADER_NOTIFICATION_PACKET,
		std::bind(&CNetworkClient::RecvNotificationPacket, this, std::placeholders::_1, std::placeholders::_2)
	);
}


void CNetworkClient::OnConnect()
{
	NETWORK_LOG(LL_SYS, "Connected to %s:%u", GetIP().c_str(), GetPort());

	if (m_bPreCryptCompleted) // Already created connection
	{
		m_bPreCryptCompleted	= false;
		m_bRSACompleted			= false;
		m_bGotRoutinePacket		= false;
	}

	auto connInitPacket = std::make_shared<TPacketConnectionInit>();
	if (IS_VALID_SMART_PTR(connInitPacket))
	{
		if (g_nmApp->DataInstance()->GetAppType() == NM_SERVICE)
		{
			connInitPacket->pConnectionType = NM_CONNECTION_TYPE_SERVICE;
		}
		else if (g_nmApp->DataInstance()->GetAppType() == NM_CLIENT || g_nmApp->DataInstance()->GetAppType() == NM_TESTAPP)
		{
			connInitPacket->pConnectionType = NM_CONNECTION_TYPE_CLIENT;
		}
		else
		{
			NETWORK_LOG(LL_CRI, "Unknown connection type! App: %u", g_nmApp->DataInstance()->GetAppType());
			g_nmApp->OnCloseRequest(EXIT_ERR_NET_UNKNOWN_SOURCE, g_winapiApiTable->GetLastError());
			return;
		}

		SendCrypted(HEADER_CS_CONN_INIT, connInitPacket.get(), connInitPacket->size());
	}

	BeginRead();
}
void CNetworkClient::OnDisconnect(const boost::system::error_code& e)
{
	NETWORK_LOG(LL_ERR, "Disconnected from: %s Reason: %d", GetIP().c_str(), e.value());

	g_nmApp->NetworkMgrInstance()->SendDisconnectedSignal();

//	Reconnect(); // aktif edip 3ten fazla ise kapat
}


std::size_t CNetworkClient::ProcessInput(const void* data, std::size_t maxlength)
{
	NETWORK_LOG(LL_TRACE, "Data: %p Max length: %u", data, maxlength);

	auto pData = reinterpret_cast<const TPacketSingleton*>(data);
	if (maxlength < sizeof(TPacketSingleton))
	{
		NETWORK_LOG(LL_CRI, "Packet size is so less!");
		return 0;
	}

	NETWORK_LOG(LL_SYS, "Got Packet with magic: 0x%X id %d (%02x) size %u coming size %u", pData->pMagic, pData->uiPacketID, pData->uiPacketID, GetPacketCapacity(pData->uiPacketID), maxlength);

	// Magic value check
	if (memcmp(pData->pMagic, gs_pNetworkMagicValue, sizeof(pData->pMagic)))
	{
		NETWORK_LOG(LL_CRI, "Unknown magic value: 0x%X", gs_pNetworkMagicValue);
		return GetPacketCapacity(pData->uiPacketID);
	}

	// ID check
	auto handler = m_pHandlers.find(pData->uiPacketID);
	if (handler == m_pHandlers.end())
	{
		NETWORK_LOG(LL_CRI, "Unknown Packet with id %d (%02x) received", pData->uiPacketID, pData->uiPacketID);
		return GetPacketCapacity(pData->uiPacketID);
	}

	// Size check
	auto ulCorrectSize = GetPacketCapacity(pData->uiPacketID);
	if (ulCorrectSize != maxlength)
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch! Correct: %u Current: %u Packet: %d (%02x)", ulCorrectSize, maxlength, pData->uiPacketID, pData->uiPacketID);
		return GetPacketCapacity(pData->uiPacketID);
	}

	// Hash check
	auto pCorrectHash = XXH32(data, maxlength - NET_CHECKSUM_LENGTH, NET_PACKET_HASH_MAGIC);
	auto pCurrentHash = *(uint32_t*)((uint32_t)data + maxlength - NET_CHECKSUM_LENGTH);

	NETWORK_LOG(LL_SYS, "Hash: %p Correct hash: %p Eq: %d", pCurrentHash, pCorrectHash, pCurrentHash == pCorrectHash);
	if (pCurrentHash != pCorrectHash)
	{
		NETWORK_LOG(LL_CRI, "Packet hash mismatch! Current: %p Correct: %p", pCurrentHash, pCorrectHash);
		return GetPacketCapacity(pData->uiPacketID);
	}

	// Build packet handler
	std::size_t handlerResult = handler->second.second(data, ulCorrectSize);
	NETWORK_LOG(LL_SYS, "handlerResult: %u data: %p length: %u", handlerResult, data, maxlength);
	if (!handlerResult)
	{
		NETWORK_LOG(LL_CRI, "Handler result return NULL!");
		return GetPacketCapacity(pData->uiPacketID);
	}

	return GetPacketCapacity(pData->uiPacketID);
}

std::size_t CNetworkClient::OnRead(const void* data, std::size_t length)
{
	NETWORK_LOG(LL_SYS, "OnRead triggered! %p-%u", data, length);
	if (!data || !length) 
	{
		NETWORK_LOG(LL_SYS, "Null data read!");
		return 0;
	}

	const auto packet = reinterpret_cast<const TPacketSingleton*>(data);
	NETWORK_LOG(LL_SYS, "Read data: %p - %u | %d - %s | ", data, length, packet->uiPacketID, GetPacketString(packet->uiPacketID).c_str());

	auto result = ProcessInput(data, length);
	NETWORK_LOG(LL_SYS, "ProcessInput result: %u", result);

	return result;
}

std::size_t CNetworkClient::OnWritePre(const void* pData, std::size_t ulSize)
{
	if (!pData || !ulSize)
	{
		NETWORK_LOG(LL_CRI, "Null data writed!");
		return ulSize;
	}

	NETWORK_LOG(LL_SYS, "OnWrite triggered! Data: %p(%u)", pData, ulSize);

	if (ulSize >= sizeof(TPacketSingleton))
	{
		auto pPsuedoPacket = reinterpret_cast<const TPacketSingleton*>(pData);
		NETWORK_LOG(LL_SYS, "Packet: %u(%s) Data: %p(%u)", pPsuedoPacket->uiPacketID, GetPacketString(pPsuedoPacket->uiPacketID).c_str(), pData, ulSize);

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

	return ulSize;
}

void CNetworkClient::OnWritePost(bool bCompleted, const std::string & szRequestId)
{
	NETWORK_LOG(LL_SYS, "Write completed! Result: %d Request ID: %s", bCompleted, szRequestId.c_str());
}

void CNetworkClient::OnError(std::uint32_t ulErrorType, const boost::system::error_code & e)
{
	NETWORK_LOG(LL_ERR, "Network error handled! ID: %u System error: %u(%s)", ulErrorType, e.value(), e.message().c_str());
}


void CNetworkClient::SendCrypted(std::uint32_t ulPacketID, void* pData, std::size_t ulSize)
{
	if (IsConnected() == false)
		return;

	NETWORK_LOG(LL_SYS, "Original data: %p-%u Packet: %u", pData, ulSize, ulPacketID);

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


	if (m_bRSACompleted || m_bPreCryptCompleted)
	{
		// Crypt packet content
		auto cryptedPacket = NAES256::Encrypt(reinterpret_cast<const uint8_t *>(pData), ulSize, m_bRSACompleted ? m_pRSAKey : m_pPreCryptKey);
		NETWORK_LOG(LL_SYS, "Crpyted data: %p-%u", cryptedPacket.get_data(), cryptedPacket.get_size());

		auto cryptedPacketData = std::make_unique<TCryptedPacket>();
		if (IS_VALID_SMART_PTR(cryptedPacketData))
		{
			memcpy(cryptedPacketData->pContext, cryptedPacket.get_data(), cryptedPacket.get_size());

			cryptedPacketData->uiDecryptedPacketId = ulPacketID;
			cryptedPacketData->ulCryptedSize = cryptedPacket.get_size();
			cryptedPacketData->ulDecryptedSize = ulSize;
			cryptedPacketData->ulCryptedSum = XXH32(cryptedPacket.get_data(), cryptedPacket.get_size(), NET_PACKET_HASH_MAGIC);
			cryptedPacketData->ulDecryptedSum = XXH32(pData, ulSize - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */), NET_PACKET_HASH_MAGIC);

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


std::size_t CNetworkClient::RecvCryptedPacket(const void* c_lpData, std::size_t uMaxLength)
{
	if (uMaxLength < TCryptedPacket::size())
		return 0;

	NETWORK_LOG(LL_SYS, "Got crypted packet!");

	if (uMaxLength != TCryptedPacket::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch! Size: %u Capacity: %u", uMaxLength, TCryptedPacket::size());
		return TCryptedPacket::size();
	}

	auto pCryptedPacket = reinterpret_cast<const TCryptedPacket*>(c_lpData);

	// Magic value check
	if (memcmp(pCryptedPacket->pMagic, gs_pNetworkMagicValue, sizeof(pCryptedPacket->pMagic)))
	{
		NETWORK_LOG(LL_CRI, "Unknown magic value: 0x%X", pCryptedPacket->pMagic);
		return TCryptedPacket::size();
	}

	// Packet sum check
	auto pCorrectChecksum = g_nmApp->NetworkMgrInstance()->CreateChecksum(c_lpData, uMaxLength - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
	if (pCorrectChecksum != pCryptedPacket->ulChecksum)
	{
		NETWORK_LOG(LL_CRI, "Packet checksum mismatch! Current: %p Correct: %p", pCryptedPacket->ulChecksum, pCorrectChecksum);
		return TCryptedPacket::size();
	}

	// Packet hash check
	auto pCorrectPacketHash = XXH32(c_lpData, uMaxLength - NET_CHECKSUM_LENGTH, NET_PACKET_HASH_MAGIC);
	if (pCorrectPacketHash != pCryptedPacket->ulHash)
	{
		NETWORK_LOG(LL_CRI, "Packet hash mismatch! Current: %p Correct: %p", pCryptedPacket->ulHash, pCorrectPacketHash);
		return TCryptedPacket::size();
	}

	NETWORK_LOG(LL_SYS, "Got crypted packet! Original Packet ID: %u Sub Packet ID: %u Crypted packet capacity: %u Crypted packet size: %u",
		pCryptedPacket->uiPacketID, pCryptedPacket->uiDecryptedPacketId, TCryptedPacket::size(), uMaxLength);

	// Packet ID Check
	auto pPacketHandler = m_pHandlers.find(pCryptedPacket->uiDecryptedPacketId);
	if (pPacketHandler == m_pHandlers.end())
	{
		NETWORK_LOG(LL_CRI, "Unknown target packet ID: %u", pCryptedPacket->uiDecryptedPacketId);
		return TCryptedPacket::size();
	}
	NETWORK_LOG(LL_SYS, "Handled packet decrypted id: %u", pCryptedPacket->uiDecryptedPacketId);

	// --

	NETWORK_LOG(LL_SYS, "Decryption has been started. Crpyted data: %p Crypted data size: %u", pCryptedPacket->pContext, pCryptedPacket->ulCryptedSize);

	// Size & sum param check
	if (!pCryptedPacket->ulCryptedSize || !pCryptedPacket->ulDecryptedSize || !pCryptedPacket->ulCryptedSum || !pCryptedPacket->ulDecryptedSum)
	{
		NETWORK_LOG(LL_CRI, "Corrupted crypted data!");
		return TCryptedPacket::size();
	}

	// Crypted packet hash check
	auto pCurrCryptedHash = XXH32(pCryptedPacket->pContext, pCryptedPacket->ulCryptedSize, NET_PACKET_HASH_MAGIC);
	if (pCryptedPacket->ulCryptedSum != pCurrCryptedHash)
	{
		NETWORK_LOG(LL_CRI, "Crypted sum check fail! Correct: %p Current: %p", pCryptedPacket->ulCryptedSum, pCurrCryptedHash);
		return TCryptedPacket::size();
	}

	// Decrypt routine
	auto pDecryptedDataBuf = NAES256::Decrypt(pCryptedPacket->pContext, pCryptedPacket->ulCryptedSize, m_bRSACompleted ? m_pRSAKey : m_pPreCryptKey);
	if (pDecryptedDataBuf.is_null())
	{
		NETWORK_LOG(LL_CRI, "Decrypt fail!");
		return TCryptedPacket::size();
	}

	auto pDecryptedPacketData = pDecryptedDataBuf.get_data();
	auto ulDecryptedPacketDataSize = pDecryptedDataBuf.get_size();
	NETWORK_LOG(LL_SYS, "Decryption completed. Decrypted data: %p Decrypted data size: %u", pDecryptedPacketData, ulDecryptedPacketDataSize);

	// Decrypted packet size check pre
	if (ulDecryptedPacketDataSize < sizeof(TPacketSingleton))
	{
		NETWORK_LOG(LL_CRI, "Packet size: %u is less!", ulDecryptedPacketDataSize);
		return TCryptedPacket::size();
	}

	// Decrypted packet size check
	if (pCryptedPacket->ulDecryptedSize != ulDecryptedPacketDataSize)
	{
		NETWORK_LOG(LL_CRI, "Packet size is corrupted! Context size: %u Decrypted size: %u", pCryptedPacket->ulDecryptedSize, ulDecryptedPacketDataSize);
		return TCryptedPacket::size();
	}

	auto pSubPacket = reinterpret_cast<const TPacketSingleton*>(pDecryptedPacketData);
	auto pSubPacketSize = GetPacketCapacity(pCryptedPacket->uiDecryptedPacketId);

	NETWORK_LOG(LL_SYS, "Got sub packet with magic: 0x%X ID: %u (%02x) Egg Context size: %u Sub packet capacity: %u",
		pSubPacket->pMagic, pSubPacket->uiPacketID, pSubPacket->uiPacketID, pCryptedPacket->ulDecryptedSize, pSubPacketSize);

	// Decrypted packet id check
	if (pSubPacket->uiPacketID != pCryptedPacket->uiDecryptedPacketId)
	{
		NETWORK_LOG(LL_CRI, "Packet header is corrupted! Sub id: %u Decrypted id: %u", pSubPacket->uiPacketID, pCryptedPacket->uiDecryptedPacketId);
		return TCryptedPacket::size();
	}

	// Capacity check
	if (pCryptedPacket->ulDecryptedSize != pSubPacketSize)
	{
		NETWORK_LOG(LL_CRI, "Packet size is corrupted! Context size: %u Sub packet size: %u", pCryptedPacket->ulDecryptedSize, pSubPacketSize);
		return TCryptedPacket::size();
	}

	// Magic value check
	if (memcmp(pSubPacket->pMagic, gs_pNetworkMagicValue, sizeof(pSubPacket->pMagic)))
	{
		NETWORK_LOG(LL_CRI, "Unknown magic value for sub packet: 0x%X", pSubPacket->pMagic);
		return TCryptedPacket::size();
	}

	// Build packet handler for decrypted packet
	auto ulSubHandlerResult = pPacketHandler->second.second(pDecryptedPacketData, ulDecryptedPacketDataSize);
	NETWORK_LOG(LL_SYS, "Sub packet handler result: %u data: %p length: %u", ulSubHandlerResult, pDecryptedPacketData, ulDecryptedPacketDataSize);
	if (!ulSubHandlerResult)
	{
		NETWORK_LOG(LL_CRI, "Handler result return NULL!");
		return TCryptedPacket::size();
	}

	return TCryptedPacket::size();
}

std::size_t CNetworkClient::RecvNotificationPacket(const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketNotification::size())
		return 0;

	NETWORK_LOG(LL_SYS, "Got notification packet!");

	auto packet = reinterpret_cast<const TPacketNotification*>(data);

	NETWORK_LOG(LL_SYS, "Notification packet received! ID: %u", packet->uiNotificationID);

	return TPacketNotification::size();
}

std::size_t CNetworkClient::RecvCheatBlacklists(const void* data, std::size_t maxlength) // TODO: When all blacklists are handled start scanner routine
{
	if (maxlength < TPacketCheatBlackLists::size())
		return 0;

	NETWORK_LOG(LL_SYS, "Got blacklist packet!");

	auto packet = reinterpret_cast<const TPacketCheatBlackLists*>(data);

	auto szTestBlackList = std::string(packet->ssBlackList1);
	NETWORK_LOG(LL_CRI, "Test blacklist:\n%s", szTestBlackList.c_str());

	try
	{
		ptree root;
		std::istringstream iss(szTestBlackList);
		read_json(iss, root);

//		for (ptree::const_iterator v = root.begin(); v != root.end(); ++v)
//		{
//			std::string key = v->first;
//			auto value = v->second.get_value<std::string>();
//			NETWORK_LOG(LL_CRI, "key: %s value %s", key.c_str(), value.c_str());
//		}

		auto headerTree = root.get_child("Header");
		if (headerTree.empty())
		{
			NETWORK_LOG(LL_ERR, "Blacklist header not found!");
			return TPacketCheatBlackLists::size();
		}

		auto scanId = headerTree.get<std::string>("ScanID");
		auto rowCount = headerTree.get<std::string>("RowCount");
		auto fieldCount = headerTree.get<std::string>("FieldCount");

		if (scanId.empty() || rowCount.empty() || fieldCount.empty())
		{
			NETWORK_LOG(LL_ERR, "Blacklist header context not found! ID: %s Row: %s Field: %s", scanId.c_str(), rowCount.c_str(), fieldCount.c_str());
			return TPacketCheatBlackLists::size();
		}

		NETWORK_LOG(LL_SYS, "Blacklist process started! ScanID: %s Row Count: %s Field Count: %s", scanId.c_str(), rowCount.c_str(), fieldCount.c_str());

		auto iScanID = std::stoi(scanId);
		if (!(iScanID > NET_SCAN_ID_NULL && iScanID < NET_SCAN_ID_MAX))
		{
			NETWORK_LOG(LL_ERR, "Unallowed scan ID: %d", iScanID);
			return TPacketCheatBlackLists::size();
		}

		auto nRowCount = std::stoi(rowCount);
		for (auto i = 1; i <= nRowCount; ++i)
		{
			NETWORK_LOG(LL_SYS, "Current row idx: %d", i);

			auto szCurrTree = std::to_string(i);
			auto currTree	= root.get_child(szCurrTree);
			if (currTree.empty())
			{
				NETWORK_LOG(LL_ERR, "Current tree: %s is empty!", szCurrTree.c_str());
				continue;
			}

			switch (iScanID)
			{
				case NET_SCAN_ID_TEST: // window scan(test)
				{
					auto szWndName		= currTree.get<std::string>("WindowName");
					auto szClassName	= currTree.get<std::string>("ClassName");

					NETWORK_LOG(LL_SYS, "Node process started! ScanID: %d Row: %u WndName: %s ClassName: %s", iScanID, i, szWndName.c_str(), szClassName.c_str());

					g_nmApp->QuarentineInstance()->AppendTestScanData(i, szWndName, szClassName);

					NETWORK_LOG(LL_SYS, "Current node appended to quarentine!");

				} break;

				default:
					NETWORK_LOG(LL_ERR, "Unknown scan ID: %d", iScanID);
					break;
			}
		}
	}
	catch (const std::exception & e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_ERR, "Blacklist json parse exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif
	}

	// Send parse complete notification
	auto notificationPacket = std::make_shared<TPacketNotification>();
	if (IS_VALID_SMART_PTR(notificationPacket))
	{
		notificationPacket->uiNotificationID = NOTIFICATION_CS_BLACKLIST_PARSE_OK;

		SendCrypted(HEADER_NOTIFICATION_PACKET, notificationPacket.get(), notificationPacket->size());
	}

	return TPacketCheatBlackLists::size();
}

std::size_t CNetworkClient::RecvPreCryptKeyInfo(const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketPreCryptKeyInfo::size())
		return 0;

	NETWORK_LOG(LL_SYS, "Got pre crypt key packet!");

	auto packet = reinterpret_cast<const TPacketPreCryptKeyInfo*>(data);

	// Register crypt key
	memcpy(&m_pPreCryptKey, packet->preCryptKey, NET_CRYPT_KEY_LENGTH);
//	DecryptBuffer(m_pPreCryptKey, NET_CRYPT_KEY_LENGTH, NETWORK_BODY_CRYPT_KEY);
	m_bPreCryptCompleted = true;

	// Start rsa initilziation
	auto rsaInitPacket = std::make_shared<TPacketRsaInit>();
	if (IS_VALID_SMART_PTR(rsaInitPacket))
	{
		m_pRSAClient.create_keypair();

		memcpy(rsaInitPacket->publicExponent, m_pRSAClient.get_public_exponent().get_data(), m_pRSAClient.get_public_exponent().get_size());
		rsaInitPacket->publicExponentSize = m_pRSAClient.get_public_exponent().get_size();
		memcpy(rsaInitPacket->modulus, m_pRSAClient.get_modulus().get_data(), m_pRSAClient.get_modulus().get_size());
		rsaInitPacket->modulusSize = m_pRSAClient.get_modulus().get_size();

		SendCrypted(HEADER_CS_RSA_INIT, rsaInitPacket.get(), rsaInitPacket->size());
	}

	return TPacketPreCryptKeyInfo::size();
}

std::size_t CNetworkClient::RecvRSAKeyInfo(const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketRsaKeyInfo::size())
		return 0;

	NETWORK_LOG(LL_SYS, "Got rsa info packet!");

	// Read
	auto packet = reinterpret_cast<const TPacketRsaKeyInfo*>(data);
	auto pPublicRSAKey = DataBuffer(packet->cryptkey, packet->cryptkeysize);

	m_pRSAClient.set_crypt_key(pPublicRSAKey);
	DataBuffer pRsaData(m_pRSAClient.get_crypt_key().get_data(), m_pRSAClient.get_crypt_key().get_size());
	memcpy(&m_pRSAKey, pRsaData.get_data(), NET_CRYPT_KEY_LENGTH);

	// Answer
	auto finalizePacket = std::make_shared<TPacketRsaKeyFinalize>();

	// First arg | Dummy
	auto ulRandomDataSize	= g_nmApp->FunctionsInstance()->GetRandomInt(8, 16);
	auto pFirstDummyData	= g_nmApp->FunctionsInstance()->GetRandomData(ulRandomDataSize);
	memcpy(finalizePacket->dummyDataFirst, pFirstDummyData, ulRandomDataSize);

	// Second Arg | RSA key
	auto szKeyHash = g_nmApp->DirFunctionsInstance()->GetSHA256(pRsaData.get_data(), pRsaData.get_size());
	strcpy(finalizePacket->rsaKeyHash, szKeyHash.c_str());

	// Third arg | Dummy
	auto ulRandomDataSize2	= g_nmApp->FunctionsInstance()->GetRandomInt(12, 24);
	auto pLastDummyData		= g_nmApp->FunctionsInstance()->GetRandomData(ulRandomDataSize2);
	memcpy(finalizePacket->dummyDataLast, pLastDummyData, ulRandomDataSize2);

	// Magic
	memcpy(finalizePacket->pMagic, gs_pNetworkMagicValue, sizeof(finalizePacket->pMagic));

	// Send
	SendCrypted(HEADER_CS_RSA_KEY_FINALIZE, finalizePacket.get(), finalizePacket->size());

	return TPacketRsaKeyInfo::size();
}

std::size_t CNetworkClient::RecvEncryptionReadyNotification(const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketRsaCheckCompleted::size())
		return 0;

	NETWORK_LOG(LL_SYS, "Got encryption complete notification!");

	auto packet = reinterpret_cast<const TPacketRsaCheckCompleted*>(data);
	if (packet->bCompleted == false)
	{
		NETWORK_LOG(LL_CRI, "Fatal error!!! RSA can NOT completed!");
		
		boost::system::error_code e;
		Disconnect(e);

		g_nmApp->OnCloseRequest(EXIT_ERR_NET_RSA_INIT_FAIL, g_winapiApiTable->GetLastError());
		return 0;
	}

	m_bRSACompleted = true;

	// ---

	// Generate temporary UUID instead of target user's steam id or unique token
	auto szUuid = std::string("");

	{
		HKEY hKey = NULL;
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\NoMercy", NULL, KEY_READ | KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			auto dwDataType = REG_SZ;
			char pBuffer[128] = { 0 };
			auto uiBufSize = 128UL;
			int val = RegQueryValueExA(hKey, "uuid", NULL, &dwDataType, (LPBYTE)&pBuffer, &uiBufSize);
			if (val == ERROR_SUCCESS)
			{
				szUuid = pBuffer;
				NETWORK_LOG(LL_SYS, "Registry uuid: %s", szUuid.c_str());
			}
			RegCloseKey(hKey);
		}

		if (szUuid.empty())
		{
			auto uuid = boost::uuids::random_generator()();
			szUuid = boost::uuids::to_string(uuid);
			NETWORK_LOG(LL_SYS, "New generated uuid: %s", szUuid.c_str());

			HKEY hKey = NULL;
			if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, "Software\\NoMercy", 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, 0) == ERROR_SUCCESS)
			{
				RegSetValueExA(hKey, "uuid", 0, REG_SZ, (const BYTE *)szUuid.c_str(), szUuid.size());
				RegCloseKey(hKey);
			}
		}
	}

	// ---

	//todo Send all of hardware informations as sha based hash'd

	// Build packet
	auto pEnterPacket = std::make_shared<TPacketEnterPacket>();
	if (IS_VALID_SMART_PTR(pEnterPacket))
	{
		pEnterPacket->dwTimestamp	= g_nmApp->FunctionsInstance()->GetCurrentTimestamp();
		pEnterPacket->dwTickCount	= g_winapiApiTable->GetTickCount();
		
		pEnterPacket->dwGameID				= g_nmApp->DataInstance()->GetGameCode();
		pEnterPacket->gameInfos.bIsX64		= g_nmApp->FunctionsInstance()->IsX64System();
		pEnterPacket->gameInfos.dwProcessID	= g_winapiApiTable->GetCurrentProcessId();
		strcpy(pEnterPacket->gameInfos.szExecutable,		g_nmApp->DirFunctionsInstance()->ExeNameWithPath().c_str());
		strcpy(pEnterPacket->gameInfos.szChecksum,			g_nmApp->DirFunctionsInstance()->GetFileMd5(pEnterPacket->gameInfos.szExecutable).c_str());
		strcpy(pEnterPacket->gameInfos.szCommandline,		g_nmApp->FunctionsInstance()->GetProcessCommandLine().c_str());
		strcpy(pEnterPacket->gameInfos.szLicenseCode,		g_nmApp->DataInstance()->GetLicenseCode().c_str());
		strcpy(pEnterPacket->gameInfos.szParentExecutable,	CProcessFunctions::GetParentProcessName(pEnterPacket->gameInfos.dwProcessID).c_str());
		strcpy(pEnterPacket->gameInfos.szTitle,				g_nmApp->FunctionsInstance()->GetMainWindowTitle(pEnterPacket->gameInfos.dwProcessID).c_str());

		SET_BIT(pEnterPacket->pAuthType,						AUTH_TYPE_UUID);
		strcpy(pEnterPacket->szNmUuid,							szUuid.c_str());
		strcpy(pEnterPacket->hwInfos.szBuildNumber,				g_nmApp->HwidManagerInstance()->GetWinBuildNumber().c_str());
		strcpy(pEnterPacket->hwInfos.szComputerName,			g_nmApp->HwidManagerInstance()->GetComputerName().c_str());
		strcpy(pEnterPacket->hwInfos.szCpuID,					g_nmApp->HwidManagerInstance()->GetCpuID().c_str());
		strcpy(pEnterPacket->hwInfos.szHDDModel,				g_nmApp->HwidManagerInstance()->GetHDDModelID().c_str());
		strcpy(pEnterPacket->hwInfos.szHDDSerial,				g_nmApp->HwidManagerInstance()->GetHDDSerialNumber().c_str());
		strcpy(pEnterPacket->hwInfos.szMacAddress,				g_nmApp->HwidManagerInstance()->GetMacAddress().c_str());
		strcpy(pEnterPacket->hwInfos.szMachineGUIDfromRegistry, g_nmApp->HwidManagerInstance()->GetGUIDFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szMachineGUIDfromWinapi,	g_nmApp->HwidManagerInstance()->GetGUIDFromWinapi().c_str());
		strcpy(pEnterPacket->hwInfos.szMachineVolumeHash,		g_nmApp->HwidManagerInstance()->GetVolumeHashFromWinapi().c_str());
		strcpy(pEnterPacket->hwInfos.szNetBiosMacAddr,			g_nmApp->HwidManagerInstance()->GetMacAddressFromNetbios().c_str());
		strcpy(pEnterPacket->hwInfos.szOSVersion,				g_nmApp->HwidManagerInstance()->GetWinVersion().c_str());
		strcpy(pEnterPacket->hwInfos.szPhysicalMacAddress,		g_nmApp->HwidManagerInstance()->GetPhysicalMacAddress().c_str());
		strcpy(pEnterPacket->hwInfos.szRegBiosDate,				g_nmApp->HwidManagerInstance()->GetBiosDateFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegBiosVendor,			g_nmApp->HwidManagerInstance()->GetBiosVendorFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegBiosVersion,			g_nmApp->HwidManagerInstance()->GetBiosVersionFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegHwid,					g_nmApp->HwidManagerInstance()->GetHardwareIdFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegInstallDate,			g_nmApp->HwidManagerInstance()->GetInstallDateFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegProductId,			g_nmApp->HwidManagerInstance()->GetProductIdFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegScsiId,				g_nmApp->HwidManagerInstance()->GetScsiIdFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegScsiSerial,			g_nmApp->HwidManagerInstance()->GetScsiSerialFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szRegSusClientID,			g_nmApp->HwidManagerInstance()->GetSusClientIdFromRegistry().c_str());
		strcpy(pEnterPacket->hwInfos.szServicePackVersion,		g_nmApp->HwidManagerInstance()->GetWinSPNumber().c_str());
		strcpy(pEnterPacket->hwInfos.szUserName,				g_nmApp->HwidManagerInstance()->GetUserName().c_str());

		pEnterPacket->dwAppType			= g_nmApp->DataInstance()->GetAppType();
		pEnterPacket->nmInfos.dwNMStage = NOMERCY_STAGE;
		strcpy(pEnterPacket->nmInfos.szNMModuleName,	g_nmApp->DataInstance()->GetAntiFullName().c_str());
		strcpy(pEnterPacket->nmInfos.szNMHash,			g_nmApp->DirFunctionsInstance()->GetFileMd5(pEnterPacket->nmInfos.szNMModuleName).c_str());
		strcpy(pEnterPacket->nmInfos.szNMVersion,		NOMERCY_VERSION);
		strcpy(pEnterPacket->nmInfos.szNMBuildDate,		g_nmApp->FunctionsInstance()->FixBuildDate().c_str());

		NETWORK_LOG(LL_SYS, "Orignal data: %p-%u Checksum: %p Checksum size: %p", pEnterPacket.get(), pEnterPacket->size(), pEnterPacket->ulChecksum, pEnterPacket->size() - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));

		// Send
		SendCrypted(HEADER_CS_ENTER_INFORMATIONS, pEnterPacket.get(), pEnterPacket->size());
	}

	// Ready for safe data transfer
	g_nmApp->NetworkMgrInstance()->SendConnectedSignal();

	return TPacketRsaCheckCompleted::size();
}

std::size_t CNetworkClient::RecvRoutinePacket(const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketRoutinePacket::size())
		return 0;

	NETWORK_LOG(LL_SYS, "Got routine packet!");
	m_bGotRoutinePacket = true;

	return TPacketRoutinePacket::size();
}


DWORD CNetworkClient::NetQueueWorkerRoutine(void)
{
	NETWORK_LOG(LL_SYS, "Network queue worker thread event has been started");

	auto pNetQueueThreadTimer	= CTimer<std::chrono::milliseconds>();

	auto currentThread			= std::shared_ptr<CThread>();
	do
	{
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_NET_QUEUEWORKER);

		if (pNetQueueThreadTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_NET_QUEUE_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);

	} while (!IS_VALID_SMART_PTR(currentThread));

	for (;;)
	{
		// Process cheat detect queue
		if (g_pkUnprocessedCheatObjects->HasWorkObject())
		{
			// Just send if routine packet(server answer) handled
			if (m_bGotRoutinePacket == true)
			{
				// Should be ready for new routine
				m_bGotRoutinePacket = false;

				// Get first queue object
				auto pFirstObject = g_pkUnprocessedCheatObjects->GetFirstObject();
				if (IS_VALID_SMART_PTR(pFirstObject))
				{
					// Process queue object
					auto detectionInfoPacket = std::make_shared<TPacketDetectionInfo>();
					if (IS_VALID_SMART_PTR(detectionInfoPacket))
					{
						detectionInfoPacket->dwErrorCode = pFirstObject->dwErrorCode;
						detectionInfoPacket->dwViolation = pFirstObject->dwViolation;
						strcpy(detectionInfoPacket->szMessage, pFirstObject->szMessage);

						SendCrypted(HEADER_CS_CHEAT_REPORT, detectionInfoPacket.get(), detectionInfoPacket->size());
					}

					// Remove processed object from queue
					g_pkUnprocessedCheatObjects->RemoveFirstObject();
				}
			}
		}

		// Reduce cpu usage
		g_winapiApiTable->Sleep(100);
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc6()
{
	__asm
	{
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
	}
}
#endif

DWORD WINAPI CNetworkClient::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CNetworkClient*)lpParam;
	return This->NetQueueWorkerRoutine();
}

bool CNetworkClient::CreateQueueWatcherThread()
{
	NETWORK_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)StartThreadRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		NETWORK_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_NET_QUEUEWORKER, StartThreadRoutine, ulFuncSize, (void*)this, 5000 + 1000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		NETWORK_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	NETWORK_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}

