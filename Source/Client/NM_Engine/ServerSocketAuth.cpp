#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "SocketConnection.h"
#include "SocketServer.h"
#include "ServerSocketPeer.h"
#include "Defines.h"
#include "SocketUtils.h"
#include "AES256.h"
#include "NetLib.h"
#include "../../Common/SocketPackets.h"
#include "RSA.h"

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <xxhash/xxhash.h>
#include <amy.hpp>

// TODO: Connection limit for each IP address

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

static void HandleGenericUpdateResult(const boost::system::error_code & e, uint64_t affected_rows, amy::connector &connector, std::shared_ptr<CNetworkPeer> peer, const std::string & what)
{
	if (affected_rows > 0 && peer)
	{
		NETWORK_LOG(LL_SYS, "Updated %s of account %s", what.c_str(), peer->GetIP().c_str());
	}

	return;
}


CNetworkServer::CNetworkServer(NetService& netService) :
	NetServer(netService()), m_pNetService(netService), m_pSQLConnector(netService()), m_szPortOrderID("0")
{
}
CNetworkServer::~CNetworkServer()
{
}


NetService& CNetworkServer::GetServiceInstance() const
{
	return m_pNetService;
}



void CNetworkServer::Init()
{
	if (!ParseJsonConfig(NETWORK_CONFIG_FILENAME))
	{
		NETWORK_LOG(LL_CRI, "Json config file: %s can NOT parsed!", NETWORK_CONFIG_FILENAME);
		g_nmApp->OnCloseRequest(EXIT_ERR_NET_MSERVER_CONFIG_FAIL, g_winapiApiTable->GetLastError());
		return;
	}

	SetupPacketDispatchers();

	NETWORK_LOG(LL_SYS, "Connecting to account sql %s@%s:%u", m_pNomercyCfgContext.szUser.c_str(), m_pNomercyCfgContext.szAddress.c_str(), m_pNomercyCfgContext.usPort);

	std::shared_ptr<CNetworkServer> self(std::dynamic_pointer_cast<CNetworkServer>(shared_from_this()));
	m_pSQLConnector.async_connect(
		boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(m_pNomercyCfgContext.szAddress), m_pNomercyCfgContext.usPort),
		amy::auth_info(m_pNomercyCfgContext.szUser, m_pNomercyCfgContext.szPassword),
		m_pNomercyCfgContext.szDatabase,
//		amy::client_multi_statements,
		amy::default_flags,
		boost::bind(&CNetworkServer::OnMySQLConnect, self, _1)
	);
}
void CNetworkServer::Run()
{
	m_pNetService.Run();
}
void CNetworkServer::Shutdown()
{
	m_pNetService.Stop();
}
bool CNetworkServer::IsShuttingDown() const
{
	return m_pNetService.HasStopped();
}

std::shared_ptr< CNetworkPeer> CNetworkServer::FindPeer(int32_t id) const
{
	auto iter = m_umPeerMap.find(id);
	if (iter != m_umPeerMap.end())
		return iter->second;

	return nullptr;
}

std::shared_ptr <NetPeer> CNetworkServer::NewPeer()
{
	std::shared_ptr<CNetworkPeer> peer = std::make_shared<CNetworkPeer>(std::static_pointer_cast<CNetworkServer>(shared_from_this()));
	m_umPeerMap.insert(std::unordered_map<int32_t, std::shared_ptr<CNetworkPeer> >::value_type(peer->GetId(), peer));
	return peer;
}

void CNetworkServer::RemovePeer(int32_t id)
{
	auto iter = m_umPeerMap.find(id);
	if (iter != m_umPeerMap.end())
		m_umPeerMap.erase(iter);
}


std::size_t CNetworkServer::ProcessInput(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	NETWORK_LOG(LL_SYS, "Peer: %s(%d) Data: %p Max length: %u", peer->GetIP().c_str(), peer->GetId(), data, maxlength);

	auto pData = reinterpret_cast<const TPacketSingleton*>(data);
	if (maxlength < sizeof(TPacketSingleton))
	{
		NETWORK_LOG(LL_CRI, "Packet size is so less!");
		return 0;
	}

	NETWORK_LOG(LL_SYS, "Got Packet with magic: 0x%X id %d (%02x) size %u coming size %u received from Client %s (%d) at Phase: %d", 
		pData->pMagic, pData->uiPacketID, pData->uiPacketID, GetPacketCapacity(pData->uiPacketID), maxlength, peer->GetIP().c_str(), peer->GetId(), peer->GetPeerPhase());
	
	// Magic value check
	if (memcmp(pData->pMagic, gs_pNetworkMagicValue, sizeof(pData->pMagic)))
	{
		NETWORK_LOG(LL_CRI, "Unknown magic value: 0x%X", pData->pMagic);
		return 0;
	}

	// ID check
	auto handler = m_pPacketHandlers.find(pData->uiPacketID);
	if (handler == m_pPacketHandlers.end())
	{
		NETWORK_LOG(LL_CRI, "Unknown Packet with id %d (%02x) received from client %s (%d)", pData->uiPacketID, pData->uiPacketID, peer->GetIP().c_str(), peer->GetId());
		return 0;
	}

	// Size check
	auto ulCorrectSize = GetPacketCapacity(pData->uiPacketID);
	if (ulCorrectSize != maxlength)
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch! Correct: %u Current: %u Packet: %d (%02x)", ulCorrectSize, maxlength, pData->uiPacketID, pData->uiPacketID);
		return 0;
	}

	// Hash check
	auto pCorrectHash = XXH32(data, maxlength - NET_CHECKSUM_LENGTH, NET_PACKET_HASH_MAGIC);
	auto pCurrentHash = *(uint32_t*)((std::size_t)data + maxlength - NET_CHECKSUM_LENGTH);

	NETWORK_LOG(LL_SYS, "Hash: %p Correct hash: %p Eq: %d", pCurrentHash, pCorrectHash, pCurrentHash == pCorrectHash);
	if (pCurrentHash != pCorrectHash)
	{
		NETWORK_LOG(LL_CRI, "Packet hash mismatch! Current: %p Correct: %p", pCurrentHash, pCorrectHash);
		return GetPacketCapacity(pData->uiPacketID);
	}

	// Phase check
	auto newPhase = PHASE_NULL;
	auto peerPhase = peer->GetPeerPhase();
	if (peerPhase != PHASE_CHECK_COMPLETED)
	{
		auto bDestroyPeer = false;
		if (peerPhase == PHASE_CONN_REQUEST)
		{
			if (pData->uiPacketID != HEADER_CS_CONN_INIT)
				bDestroyPeer = true;
				
			newPhase = PHASE_ENCRYPTED_COMM_READY;
		}
		else if (peerPhase == PHASE_ENCRYPTED_COMM_READY)
		{
			if (pData->uiPacketID != HEADER_CRYPTED_PACKET)
				bDestroyPeer = true;

			peer->SetPeerPhase(PHASE_CHECK_COMPLETED);
		}
		else
		{
			bDestroyPeer = true;
		}

		if (bDestroyPeer)
		{
			NETWORK_LOG(LL_CRI, "Unknown packet request for current phase! Packet: %u Phase: %d", pData->uiPacketID, peerPhase);

			boost::system::error_code e;
			peer->Disconnect(e);

			return GetPacketCapacity(pData->uiPacketID);
		}
	}

	// Build packet handler
	auto handlerResult = handler->second.second(peer, data, ulCorrectSize);
	NETWORK_LOG(LL_SYS, "handlerResult: %u data: %p length: %u", handlerResult, data, maxlength);
	if (!handlerResult)
	{
		NETWORK_LOG(LL_CRI, "Handler result return NULL!");
		return GetPacketCapacity(pData->uiPacketID);
	}

	// Register new phase
	if (newPhase != PHASE_NULL)
		peer->SetPeerPhase(newPhase);

	return ulCorrectSize;
}

bool CNetworkServer::ParseJsonConfig(const std::string& szConfigFileName)
{
	ptree root;
	read_json(szConfigFileName, root);

	std::string currentStage = root.get<std::string>("stage", "dev");
	auto stageTree = root.get_child(currentStage);

	m_szCfgMasterServerVersion = stageTree.get<std::string>("version", "1");

	auto dbCache = stageTree.find("dbcache");
	if (dbCache == stageTree.not_found()) {
		NETWORK_LOG(LL_ERR, "cannot find dbcache stage tree");
		return false;
	}

	if (!stageTree.count("nm_servers"))
	{
		NETWORK_LOG(LL_ERR, "cannot find nm_servers tree");
		return false;
	}

	int portorder = std::stoi(m_szPortOrderID);
	size_t i = 0;
	uint16_t port = 0;

	auto authTree = stageTree.get_child("nm_servers");
	if (!authTree.count("ports"))
	{
		NETWORK_LOG(LL_ERR, "cannot find ports in tree. (%d)", portorder);
		return false;
	}

	for (auto const& item : authTree.get_child("ports"))
	{
		if (portorder == i++)
			port = item.second.get_value<uint16_t>();
	}

	m_pServerCfg.szHostName = str(boost::format("PORT_ORDER_ID%s") % m_szPortOrderID.c_str());
	m_pServerCfg.usPort = port;
	m_pServerCfg.szBindIP = "0.0.0.0";

	if (!stageTree.count("mysql"))
	{
		NETWORK_LOG(LL_ERR, "cannot find mysql in tree.");
		return false;
	}

	ptree databasesTree = stageTree.get_child("mysql.databases");

	for (const auto & kv : databasesTree)
	{
		int8_t retries = 5;
		NETWORK_LOG(0, "connecting to MySQL server (%s)", kv.second.get<std::string>("ident").c_str());

		if (kv.second.get<std::string>("ident") == "NOMERCY_DB")
		{
			m_pNomercyCfgContext.szAddress = kv.second.get<std::string>("address");
			m_pNomercyCfgContext.szUser = kv.second.get<std::string>("user");
			m_pNomercyCfgContext.szPassword = kv.second.get<std::string>("password");
			m_pNomercyCfgContext.szDatabase = kv.second.get<std::string>("database");
			m_pNomercyCfgContext.usPort = 3306;
			if (kv.second.find("port") != kv.second.not_found())
				m_pNomercyCfgContext.usPort = kv.second.get<uint16_t>("port");

			break;
		}
	}

	return true;
}


void CNetworkServer::SetupPacketDispatchers()
{
	RegisterPacket<TPacketConnectionInit>(HEADER_CS_CONN_INIT, std::bind(&CNetworkServer::RecvConnectionInit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	RegisterPacket<TPacketRsaInit>(HEADER_CS_RSA_INIT, std::bind(&CNetworkServer::RecvRSAInit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	RegisterPacket<TPacketRsaKeyFinalize>(HEADER_CS_RSA_KEY_FINALIZE, std::bind(&CNetworkServer::RecvRSAFinalize, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	RegisterPacket<TPacketEnterPacket>(HEADER_CS_ENTER_INFORMATIONS, std::bind(&CNetworkServer::RecvEnterRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	RegisterPacket<TPacketDetectionInfo>(HEADER_CS_CHEAT_REPORT, std::bind(&CNetworkServer::RecvCheatReport, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	RegisterPacket<TCryptedPacket>(HEADER_CRYPTED_PACKET, std::bind(&CNetworkServer::RecvCryptedPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	RegisterPacket<TPacketTestMsgX>(HEADER_TEST_MSG_LARGE, std::bind(&CNetworkServer::RecvLargeTestMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	RegisterPacket<TPacketNotification>(HEADER_NOTIFICATION_PACKET, std::bind(&CNetworkServer::RecvNotificationPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void CNetworkServer::OnMySQLConnect(std::weak_ptr <CNetworkServer> self, AMY_SYSTEM_NS::error_code const& e)
{
	std::shared_ptr <CNetworkServer> _this(self.lock());
	if (_this)
	{
		if (!e)
		{
			auto reconnect = true;

			if (0 != mysql_options(_this->m_pSQLConnector.native(), MYSQL_OPT_RECONNECT, &reconnect))
			{
				NETWORK_LOG(LL_SYS, "mysql_option: %s", mysql_error(_this->m_pSQLConnector.native()));
			}
			mysql_set_character_set(_this->m_pSQLConnector.native(), "latin1");

			NETWORK_LOG(LL_SYS, "Binding server on %s:%u", _this->m_pServerCfg.szBindIP.c_str(), _this->m_pServerCfg.usPort);
			_this->Bind(_this->m_pServerCfg.szBindIP, _this->m_pServerCfg.usPort);

			_this->ParseCheatBlackList(self);
		}
		else
		{
			NETWORK_LOG(LL_CRI, "Cannot connect to MySQL server! Error: %s(%d)", e.message().c_str(), e.value());
		}
	}
}

void CNetworkServer::ParseCheatBlackList(std::weak_ptr <CNetworkServer> self)
{
	std::shared_ptr <CNetworkServer> _this(self.lock());
	if (_this)
	{
		auto ProcessBlacklistRoutine = [&](uint32_t uiScanID, const std::string & szTableName)
		{
			// sql

			std::string szQueryDB = "SELECT * FROM " + szTableName;

			try
			{
				_this->m_pSQLConnector.query(szQueryDB);
			}
			catch (std::exception & e)
			{
#ifdef _DEBUG
				NETWORK_LOG(LL_ERR, "Exception throwed! Data: %s", e.what());
#else
				UNREFERENCED_PARAMETER(e);
#endif
				return;
			}

			auto setDBResult = _this->m_pSQLConnector.store_result();
			if (setDBResult.empty())
			{
				NETWORK_LOG(LL_ERR, "Result set is empty!");
				return;
			}

			// json

			ptree root;

			ptree pHeaderNode;
			pHeaderNode.put("ScanID", std::to_string(uiScanID));
			pHeaderNode.put("RowCount", std::to_string(setDBResult.affected_rows()));
			pHeaderNode.put("FieldCount", std::to_string(setDBResult.field_count()));
			root.add_child("Header", pHeaderNode);

			// parse sql

			switch (uiScanID)
			{
				case NET_SCAN_ID_TEST: // window scan(test)
				{
					uint32_t idx = 0;
					for (const auto & row : setDBResult)
					{
						const amy::field& fieldID			= row[0];
						const amy::field& fieldWndName		= row[1];
						const amy::field& fieldClassName	= row[2];

						auto uiID			= fieldID.is_null()			? 0U : fieldID.as<amy::sql_int_unsigned>();
						auto szWndName		= fieldWndName.is_null()	? "" : fieldWndName.as<amy::sql_text>();
						auto szClassName	= fieldClassName.is_null()	? "" : fieldClassName.as<amy::sql_text>();

						NETWORK_LOG(LL_SYS, "Idx: %u | ID: %u Wnd: %s Class: %s", idx++, uiID, szWndName.c_str(), szClassName.c_str());

						ptree pNode;

						pNode.put("WindowName", szWndName);
						pNode.put("ClassName", szClassName);

						root.add_child(std::to_string(uiID), pNode);
					}

				} break;

				default:
					NETWORK_LOG(LL_ERR, "Unknown scan ID: %u", uiScanID);
					break;
			}

			std::ostringstream ossBuf;
			write_json(ossBuf, root, false);

			_this->m_umSerializedBlacklists.insert(std::unordered_map<int32_t, std::string >::value_type(uiScanID, ossBuf.str()));

//			NETWORK_LOG(LL_SYS, "%s", ossBuf.str().c_str());
		};


		std::map <uint32_t, std::string> mBlacklistContext =
		{
			/* scan id, table name */
			{ NET_SCAN_ID_TEST, xorstr("cheat_blacklist_1").crypt_get() }
		};

		for (const auto & it : mBlacklistContext)
		{
			ProcessBlacklistRoutine(it.first, it.second);
		}
	}
}


std::size_t CNetworkServer::RecvConnectionInit(std::shared_ptr<CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketConnectionInit::size())
		return 0;

	if (maxlength != TPacketConnectionInit::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch it can be combined packet!");
		return TPacketConnectionInit::size();
	}

	auto packet = reinterpret_cast<const TPacketConnectionInit*>(data);

	// Packet sum check
	auto pCorrectChecksum = g_nmApp->NetworkMgrInstance()->CreateChecksum(data, maxlength - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
	if (pCorrectChecksum != packet->ulChecksum)
	{
		NETWORK_LOG(LL_CRI, "Packet checksum mismatch! Current: %p Correct: %p", packet->ulChecksum, pCorrectChecksum);
		return TPacketConnectionInit::size();
	}

	switch (packet->pConnectionType)
	{
		case NM_CONNECTION_TYPE_SERVICE:
		case NM_CONNECTION_TYPE_CLIENT:
		{
			// Register connection type of peer
			peer->SetConnectionType(packet->pConnectionType);

			// Create & Send pre crypt dynamic key
			peer->SendPreCryptKey();

		} break;

//		case NM_CONNECTION_TYPE_P2P_MASTER_SERVER:
//		case NM_CONNECTION_TYPE_GAME_SERVER:
//		case NM_CONNECTION_TYPE_WEB_INTERFACE:
//		{
//			TODO_MSG;
//		} break;

		default:
			NETWORK_LOG(LL_ERR, "Unknown connection type: %X", packet->pConnectionType);
			boost::system::error_code e;
			peer->Disconnect(e);
			break;
	}

	NETWORK_LOG(LL_SYS, "Connection type info packet received! Type: %X", packet->pConnectionType);

	return TPacketConnectionInit::size();
}

std::size_t CNetworkServer::RecvNotificationPacket(std::shared_ptr<CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketNotification::size())
		return 0;

	if (maxlength != TPacketNotification::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch it can be combined packet!");
		return TPacketNotification::size();
	}

	auto packet = reinterpret_cast<const TPacketNotification*>(data);

	NETWORK_LOG(LL_SYS, "Notification packet received! ID: %u", packet->uiNotificationID);

	switch (packet->uiNotificationID)
	{
		case NOTIFICATION_CS_BLACKLIST_PARSE_OK:
		{
			peer->SendRoutinePacket();
		} break;

		default:
			NETWORK_LOG(LL_ERR, "Unknown notificaiton id: %u", packet->uiNotificationID);
			boost::system::error_code e;
			peer->Disconnect(e);
			break;
	}

	return TPacketNotification::size();
}

std::size_t CNetworkServer::RecvLargeTestMsg(std::shared_ptr<CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketTestMsgX::size())
		return 0;

	if (maxlength != TPacketTestMsgX::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch it can be combined packet!");
		return TPacketTestMsgX::size();
	}

	auto packet = reinterpret_cast<const TPacketTestMsgX*>(data);

	auto begin = (*(uint32_t*)((uint32_t)packet->szMessage));
	auto end = (*(uint32_t*)((uint32_t)packet->szMessage + sizeof(packet->szMessage) - sizeof(uint32_t)));

	NETWORK_LOG(LL_CRI, "large msg ctx: %X - %X", begin, end);

	return TPacketTestMsgX::size();
}

std::size_t CNetworkServer::RecvCryptedPacket(std::shared_ptr <CNetworkPeer> pPeer, const void * c_lpData, std::size_t uMaxLength)
{
	NETWORK_LOG(LL_SYS, "Got packet! Peer: %s(%u) Data: %p Size: %u", pPeer->GetIP().c_str(), pPeer->GetId(), c_lpData, uMaxLength);

	if (uMaxLength < TCryptedPacket::size())
		return 0;

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
	auto pPacketHandler = m_pPacketHandlers.find(pCryptedPacket->uiDecryptedPacketId);
	if (pPacketHandler == m_pPacketHandlers.end())
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
	auto pDecryptedDataBuf = NAES256::Decrypt(pCryptedPacket->pContext, pCryptedPacket->ulCryptedSize, pPeer->IsRSAKeyInitialized() ? pPeer->GetRSAKey() : pPeer->GetPreCryptKey());
	if (pDecryptedDataBuf.is_null())
	{
		NETWORK_LOG(LL_CRI, "Decrypt fail!");
		return TCryptedPacket::size();
	}

	auto pDecryptedPacketData		= pDecryptedDataBuf.get_data();
	auto ulDecryptedPacketDataSize	= pDecryptedDataBuf.get_size();
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

	auto pSubPacket		= reinterpret_cast<const TPacketSingleton*>(pDecryptedPacketData);
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
	auto ulSubHandlerResult = pPacketHandler->second.second(pPeer, pDecryptedPacketData, ulDecryptedPacketDataSize);
	NETWORK_LOG(LL_SYS, "Sub packet handler result: %u data: %p length: %u", ulSubHandlerResult, pDecryptedPacketData, ulDecryptedPacketDataSize);
	if (!ulSubHandlerResult)
	{
		NETWORK_LOG(LL_CRI, "Handler result return NULL!");
		return TCryptedPacket::size();
	}

	return TCryptedPacket::size();
}

std::size_t CNetworkServer::RecvCheatReport(std::shared_ptr<CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketDetectionInfo::size())
		return 0;

	if (maxlength != TPacketDetectionInfo::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch it can be combined packet!");
		return TPacketDetectionInfo::size();
	}
	
	auto packet = reinterpret_cast<const TPacketDetectionInfo*>(data);

	auto szCorrectSum = g_nmApp->NetworkMgrInstance()->CreateChecksum(data, maxlength - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
	if (szCorrectSum != packet->ulChecksum)
	{
		NETWORK_LOG(LL_CRI, "Packet checksum mismatch! Current: %p Correct: %p Connection closed!", packet->ulChecksum, szCorrectSum);
		return TPacketDetectionInfo::size();
	}

	NETWORK_LOG(LL_SYS, "Got report packet! Violation: %u Msg: %s", packet->dwViolation, packet->szMessage);

	boost::system::error_code ec;
	char szMessage[sizeof(packet->szMessage) * 2 + 1];

	try
	{
		std::string message(packet->szMessage, std::min<std::size_t>(strlen(packet->szMessage), sizeof(packet->szMessage)));
		mysql_real_escape_string(m_pSQLConnector.native(), szMessage, message.c_str(), message.size());
	}
	catch (std::exception & e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "DetectionLog escape string exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	std::string szDetectLogQuery;
	try
	{
		szDetectLogQuery = (boost::format("INSERT INTO detect_logs (ServerTime, PeerID, IpAddress, ViolationID, ErrorCode, Message) VALUES ('%1%', %2%, '%3%', %4%, %5%, '%6%')") % \
			std::to_string(g_nmApp->FunctionsInstance()->GetCurrentTimestamp()) % peer->GetId() % peer->GetIP() % packet->dwViolation % packet->dwErrorCode % szMessage).str();
	}
	catch (std::exception & e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "DetectionLog format exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

//	NETWORK_LOG(LL_ERR, "DetectionLog query info, size: %u str: %s", szDetectLogQuery.size(), szDetectLogQuery.c_str());

	try
	{
		amy::execute(m_pSQLConnector, szDetectLogQuery, ec);
	}
	catch (std::exception& e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "DetectionLog sql exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	// Send routine packet
	peer->SendRoutinePacket();

	return TPacketDetectionInfo::size();
}


std::size_t CNetworkServer::RecvRSAInit(std::shared_ptr<CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketRsaInit::size())
		return 0;

	if (maxlength != TPacketRsaInit::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch it can be combined packet!");
		return TPacketRsaInit::size();
	}

	auto packet = reinterpret_cast<const TPacketRsaInit*>(data);

	auto szCorrectSum = g_nmApp->NetworkMgrInstance()->CreateChecksum(data, maxlength - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
	if (szCorrectSum != packet->ulChecksum)
	{
		NETWORK_LOG(LL_CRI, "Packet checksum mismatch! Current: %p Correct: %p Connection closed!", packet->ulChecksum, szCorrectSum);
		return TPacketRsaInit::size();
	}

	// Create rsa key
	auto pCryptKey = peer->BuildRSAKey(packet->publicExponent, packet->publicExponentSize, packet->modulus, packet->modulusSize);
	if (pCryptKey.is_null())
	{
		NETWORK_LOG(LL_CRI, "RSA Key build fail!");

		boost::system::error_code e;
		peer->Disconnect(e);

		return TPacketRsaInit::size();
	}

	// Send to client
	peer->SendRSAKeyInfo(pCryptKey);

	NETWORK_LOG(LL_SYS, "Sent rsa packet data: %s-%u", pCryptKey.get_data(), pCryptKey.get_size());

	return TPacketRsaInit::size();
}


std::size_t CNetworkServer::RecvRSAFinalize(std::shared_ptr<CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	NETWORK_LOG(LL_SYS, "Peer: %p-%p %s(%d) Data: %p Max length: %p Packet size: %p",
		peer, peer.get(), peer->GetIP().c_str(), peer->GetId(), data, maxlength, TPacketRsaKeyFinalize::size());

	if (maxlength < TPacketRsaKeyFinalize::size())
		return 0;

	if (maxlength != TPacketRsaKeyFinalize::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch it can be combined packet!");
		return TPacketRsaKeyFinalize::size();
	}

	auto packet = reinterpret_cast<const TPacketRsaKeyFinalize*>(data);

	auto szCorrectSum = g_nmApp->NetworkMgrInstance()->CreateChecksum(data, maxlength - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
	if (szCorrectSum != packet->ulChecksum)
	{
		NETWORK_LOG(LL_CRI, "Packet checksum mismatch! Current: %p Correct: %p Connection closed!", packet->ulChecksum, szCorrectSum);
		return TPacketRsaKeyFinalize::size();
	}

	NETWORK_LOG(LL_SYS, "Got RSA finalize packet!");

	auto szCorrectKeyHash	= g_nmApp->DirFunctionsInstance()->GetSHA256(peer->GetRSACtx().get_data(), peer->GetRSACtx().get_size());
	auto szCurrentKeyHash	= packet->rsaKeyHash;
	auto bKeysIsEqual		= (!strcmp(szCorrectKeyHash.c_str(), szCurrentKeyHash));

	NETWORK_LOG(LL_SYS, "Client key hash: %s Server key hash: %s Eq: %d", szCurrentKeyHash, szCorrectKeyHash, bKeysIsEqual);

	// Check rsa key
	if (bKeysIsEqual == false)
	{
		NETWORK_LOG(LL_CRI, "RSA key hash mismatch with clientside!"); // TODO: Mark as failed, If client still try send request reject requests
		peer->DelayedDisconnect(5000);
	}

	// Send notification to client
	peer->SendRSACompleteNotification(bKeysIsEqual);

	// Set key data
	peer->SetRSAKey(peer->GetRSACtx().get_data());
	NETWORK_LOG(LL_SYS, "Server RSA key succesfully created! Size: %u", peer->GetRSACtx().get_size());

	return TPacketRsaKeyFinalize::size();
}

std::size_t CNetworkServer::RecvEnterRequest(std::shared_ptr<CNetworkPeer> peer, const void* data, std::size_t maxlength)
{
	if (maxlength < TPacketEnterPacket::size())
		return 0;

	if (!peer || !peer.get())
		return TPacketEnterPacket::size();

	if (maxlength != TPacketEnterPacket::size())
	{
		NETWORK_LOG(LL_CRI, "Packet size mismatch it can be combined packet!");
		return TPacketEnterPacket::size();
	}

	boost::system::error_code ec;

	auto packet = reinterpret_cast<const TPacketEnterPacket*>(data);

	auto szCorrectSum = g_nmApp->NetworkMgrInstance()->CreateChecksum(data, maxlength - (NET_CHECKSUM_LENGTH /* hash */ + NET_CHECKSUM_LENGTH /* sum */));
	if (szCorrectSum != packet->ulChecksum)
	{
		NETWORK_LOG(LL_CRI, "Packet checksum mismatch! Current: %p Correct: %p Connection closed!", packet->ulChecksum, szCorrectSum);
		return TPacketEnterPacket::size();
	}

	auto pCurrentHash = XXH32(data, maxlength - NET_CHECKSUM_LENGTH, NET_PACKET_HASH_MAGIC);

	NETWORK_LOG(LL_SYS, "Hash: %p Correct hash: %p Eq: %d", pCurrentHash, packet->ulHash, pCurrentHash == packet->ulHash);
	if (packet->ulHash != pCurrentHash)
	{
		NETWORK_LOG(LL_CRI, "Packet hash mismatch! Packet hash: %p Current: %p", packet->ulHash, pCurrentHash);
		return TPacketEnterPacket::size();
	}


	NETWORK_LOG(LL_SYS, "Got enter packet from: Peer ID: %u Conn Type: %X Phase: %X IP: %s",
		peer->GetId(), peer->GetConnectionType(), peer->GetPeerPhase(), peer->GetIP().c_str());


	char szUuid[sizeof(packet->szNmUuid) * 2 + 1];
	char szPlatformID[sizeof(packet->szNmPlatformId) * 2 + 1];
	char szUniqueToken[sizeof(packet->szUniqueToken) * 2 + 1];
	char szExecutable[sizeof(packet->gameInfos.szExecutable) * 2 + 1];
	char szExecutableChecksum[sizeof(packet->gameInfos.szChecksum) * 2 + 1];
	char szCommandline[sizeof(packet->gameInfos.szCommandline) * 2 + 1];
	char szTitle[sizeof(packet->gameInfos.szTitle) * 2 + 1];
	char szParentExecutable[sizeof(packet->gameInfos.szParentExecutable) * 2 + 1];
	char szLicenseCode[sizeof(packet->gameInfos.szParentExecutable) * 2 + 1];

	try
	{
		std::string uuid(packet->szNmUuid, std::min<std::size_t>(strlen(packet->szNmUuid), sizeof(packet->szNmUuid)));
		mysql_real_escape_string(m_pSQLConnector.native(), szUuid, uuid.c_str(), uuid.size());

		std::string platformid(packet->szNmPlatformId, std::min<std::size_t>(strlen(packet->szNmPlatformId), sizeof(packet->szNmPlatformId)));
		mysql_real_escape_string(m_pSQLConnector.native(), szPlatformID, platformid.c_str(), platformid.size());

		std::string uniquetoken(packet->szUniqueToken, std::min<std::size_t>(strlen(packet->szUniqueToken), sizeof(packet->szUniqueToken)));
		mysql_real_escape_string(m_pSQLConnector.native(), szUniqueToken, uniquetoken.c_str(), uniquetoken.size());

		std::string exename(packet->gameInfos.szExecutable, std::min<std::size_t>(strlen(packet->gameInfos.szExecutable), sizeof(packet->gameInfos.szExecutable)));
		mysql_real_escape_string(m_pSQLConnector.native(), szExecutable, exename.c_str(), exename.size());

		std::string exesum(packet->gameInfos.szChecksum, std::min<std::size_t>(strlen(packet->gameInfos.szChecksum), sizeof(packet->gameInfos.szChecksum)));
		mysql_real_escape_string(m_pSQLConnector.native(), szExecutableChecksum, exesum.c_str(), exesum.size());

		std::string execmd(packet->gameInfos.szCommandline, std::min<std::size_t>(strlen(packet->gameInfos.szCommandline), sizeof(packet->gameInfos.szCommandline)));
		mysql_real_escape_string(m_pSQLConnector.native(), szCommandline, execmd.c_str(), execmd.size());

		std::string exetitle(packet->gameInfos.szTitle, std::min<std::size_t>(strlen(packet->gameInfos.szTitle), sizeof(packet->gameInfos.szTitle)));
		mysql_real_escape_string(m_pSQLConnector.native(), szTitle, exetitle.c_str(), exetitle.size());

		std::string parentname(packet->gameInfos.szParentExecutable, std::min<std::size_t>(strlen(packet->gameInfos.szParentExecutable), sizeof(packet->gameInfos.szParentExecutable)));
		mysql_real_escape_string(m_pSQLConnector.native(), szParentExecutable, parentname.c_str(), parentname.size());

		std::string licensecode(packet->gameInfos.szLicenseCode, std::min<std::size_t>(strlen(packet->gameInfos.szLicenseCode), sizeof(packet->gameInfos.szLicenseCode)));
		mysql_real_escape_string(m_pSQLConnector.native(), szLicenseCode, licensecode.c_str(), licensecode.size());
	}
	catch (std::exception & e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "UserTable escape string exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	std::string szUserTableQuery;
	try
	{
		szUserTableQuery = (boost::format("INSERT INTO user_table (ServerTime, PeerID, PeerConnType, IpAddress, ClientTime, TickCount, AuthType, UUID, PlatformID, "
			"UniqueToken, GameID, IsX64Client, ProcessID, ExecutableName, ExecutableChecksum, ExecutableCommandLine, ExecutableTitle, ParentExecutable, LicenseCode, "
			"AppType, NoMercyStage) VALUES ('%1%', '%2%', '%3%', '%4%', %5%, %6%, '%7%', '%8%', '%9%', '%10%', %11%, %12%, %13%, '%14%', '%15%', '%16%', '%17%', '%18%', '%19%', %20%, %21%)") % \
			std::to_string(g_nmApp->FunctionsInstance()->GetCurrentTimestamp()) % std::to_string(peer->GetId()) % std::to_string(peer->GetConnectionType()) % \
			peer->GetIP() % packet->dwTimestamp % packet->dwTickCount % std::to_string(packet->pAuthType) % szUuid % szPlatformID % szUniqueToken % packet->dwGameID % packet->gameInfos.bIsX64 % \
			packet->gameInfos.dwProcessID % szExecutable % szExecutableChecksum % szCommandline % szTitle % szParentExecutable % szLicenseCode % packet->dwAppType % packet->nmInfos.dwNMStage
			).str();
	}
	catch (std::exception & e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "UserTable format exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

//	NETWORK_LOG(LL_ERR, "UserTable query info, size: %u str: %s", szUserTableQuery.size(), szUserTableQuery.c_str());

	try
	{
		amy::execute(m_pSQLConnector, szUserTableQuery, ec);
	}
	catch (std::exception& e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "UserTable sql exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	if (ec)
	{
		NETWORK_LOG(LL_CRI, "User table insert query fail! Error: %u(%s)", ec.value(), ec.message().c_str());

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	// ---

	char szNMModuleName[sizeof(packet->nmInfos.szNMModuleName) * 2 + 1];
	char szNMModuleHash[sizeof(packet->nmInfos.szNMHash) * 2 + 1];
	char szNMVersion[sizeof(packet->nmInfos.szNMVersion) * 2 + 1];
	char szNMBuildDate[sizeof(packet->nmInfos.szNMBuildDate) * 2 + 1];
	char szUserName[sizeof(packet->hwInfos.szUserName) * 2 + 1];
	char szComputerName[sizeof(packet->hwInfos.szComputerName) * 2 + 1];
	char szOSVersion[sizeof(packet->hwInfos.szOSVersion) * 2 + 1];
	char szBuildNumber[sizeof(packet->hwInfos.szBuildNumber) * 2 + 1];
	char szServicePackVersion[sizeof(packet->hwInfos.szServicePackVersion) * 2 + 1];
	char szMacAddress[sizeof(packet->hwInfos.szMacAddress) * 2 + 1];
	char szPhysicalMacAddress[sizeof(packet->hwInfos.szPhysicalMacAddress) * 2 + 1];
	char szNetBiosMacAddr[sizeof(packet->hwInfos.szNetBiosMacAddr) * 2 + 1];
	char szHDDSerial[sizeof(packet->hwInfos.szHDDSerial) * 2 + 1];
	char szHDDModel[sizeof(packet->hwInfos.szHDDModel) * 2 + 1];
	char szMachineGUIDfromRegistry[sizeof(packet->hwInfos.szMachineGUIDfromRegistry) * 2 + 1];
	char szMachineGUIDfromWinapi[sizeof(packet->hwInfos.szMachineGUIDfromWinapi) * 2 + 1];
	char szMachineVolumeHash[sizeof(packet->hwInfos.szMachineVolumeHash) * 2 + 1];
	char szCpuID[sizeof(packet->hwInfos.szCpuID) * 2 + 1];
	char szRegProductId[sizeof(packet->hwInfos.szRegProductId) * 2 + 1];
	char szRegInstallDate[sizeof(packet->hwInfos.szRegInstallDate) * 2 + 1];
	char szRegScsiId[sizeof(packet->hwInfos.szRegScsiId) * 2 + 1];
	char szRegScsiSerial[sizeof(packet->hwInfos.szRegScsiSerial) * 2 + 1];
	char szRegHwid[sizeof(packet->hwInfos.szRegHwid) * 2 + 1];
	char szRegBiosDate[sizeof(packet->hwInfos.szRegBiosDate) * 2 + 1];
	char szRegBiosVendor[sizeof(packet->hwInfos.szRegBiosVendor) * 2 + 1];
	char szRegBiosVersion[sizeof(packet->hwInfos.szRegBiosVersion) * 2 + 1];
	char szRegSusClientID[sizeof(packet->hwInfos.szRegSusClientID) * 2 + 1];

	try
	{
		std::string nmmodule(packet->nmInfos.szNMModuleName, std::min<std::size_t>(strlen(packet->nmInfos.szNMModuleName), sizeof(packet->nmInfos.szNMModuleName)));
		mysql_real_escape_string(m_pSQLConnector.native(), szNMModuleName, nmmodule.c_str(), nmmodule.size());

		std::string nmhash(packet->nmInfos.szNMHash, std::min<std::size_t>(strlen(packet->nmInfos.szNMHash), sizeof(packet->nmInfos.szNMHash)));
		mysql_real_escape_string(m_pSQLConnector.native(), szNMModuleHash, nmhash.c_str(), nmhash.size());

		std::string nmversion(packet->nmInfos.szNMVersion, std::min<std::size_t>(strlen(packet->nmInfos.szNMVersion), sizeof(packet->nmInfos.szNMVersion)));
		mysql_real_escape_string(m_pSQLConnector.native(), szNMVersion, nmversion.c_str(), nmversion.size());

		std::string nmbuilddate(packet->nmInfos.szNMBuildDate, std::min<std::size_t>(strlen(packet->nmInfos.szNMBuildDate), sizeof(packet->nmInfos.szNMBuildDate)));
		mysql_real_escape_string(m_pSQLConnector.native(), szNMBuildDate, nmbuilddate.c_str(), nmbuilddate.size());

		std::string hwusername(packet->hwInfos.szUserName, std::min<std::size_t>(strlen(packet->hwInfos.szUserName), sizeof(packet->hwInfos.szUserName)));
		mysql_real_escape_string(m_pSQLConnector.native(), szUserName, hwusername.c_str(), hwusername.size());

		std::string hwcompname(packet->hwInfos.szComputerName, std::min<std::size_t>(strlen(packet->hwInfos.szComputerName), sizeof(packet->hwInfos.szComputerName)));
		mysql_real_escape_string(m_pSQLConnector.native(), szComputerName, hwcompname.c_str(), hwcompname.size());

		std::string hwosver(packet->hwInfos.szOSVersion, std::min<std::size_t>(strlen(packet->hwInfos.szOSVersion), sizeof(packet->hwInfos.szOSVersion)));
		mysql_real_escape_string(m_pSQLConnector.native(), szOSVersion, hwosver.c_str(), hwosver.size());

		std::string hwosbuild(packet->hwInfos.szBuildNumber, std::min<std::size_t>(strlen(packet->hwInfos.szBuildNumber), sizeof(packet->hwInfos.szBuildNumber)));
		mysql_real_escape_string(m_pSQLConnector.native(), szBuildNumber, hwosbuild.c_str(), hwosbuild.size());

		std::string hwossp(packet->hwInfos.szServicePackVersion, std::min<std::size_t>(strlen(packet->hwInfos.szServicePackVersion), sizeof(packet->hwInfos.szServicePackVersion)));
		mysql_real_escape_string(m_pSQLConnector.native(), szServicePackVersion, hwossp.c_str(), hwossp.size());

		std::string hwmac(packet->hwInfos.szMacAddress, std::min<std::size_t>(strlen(packet->hwInfos.szMacAddress), sizeof(packet->hwInfos.szMacAddress)));
		mysql_real_escape_string(m_pSQLConnector.native(), szMacAddress, hwmac.c_str(), hwmac.size());

		std::string hwphysmac(packet->hwInfos.szPhysicalMacAddress, std::min<std::size_t>(strlen(packet->hwInfos.szPhysicalMacAddress), sizeof(packet->hwInfos.szPhysicalMacAddress)));
		mysql_real_escape_string(m_pSQLConnector.native(), szPhysicalMacAddress, hwphysmac.c_str(), hwphysmac.size());

		std::string hwnetbiosmac(packet->hwInfos.szNetBiosMacAddr, std::min<std::size_t>(strlen(packet->hwInfos.szNetBiosMacAddr), sizeof(packet->hwInfos.szNetBiosMacAddr)));
		mysql_real_escape_string(m_pSQLConnector.native(), szNetBiosMacAddr, hwnetbiosmac.c_str(), hwnetbiosmac.size());

		std::string hwhddserial(packet->hwInfos.szHDDSerial, std::min<std::size_t>(strlen(packet->hwInfos.szHDDSerial), sizeof(packet->hwInfos.szHDDSerial)));
		mysql_real_escape_string(m_pSQLConnector.native(), szHDDSerial, hwhddserial.c_str(), hwhddserial.size());

		std::string hwhddmodel(packet->hwInfos.szHDDModel, std::min<std::size_t>(strlen(packet->hwInfos.szHDDModel), sizeof(packet->hwInfos.szHDDModel)));
		mysql_real_escape_string(m_pSQLConnector.native(), szHDDModel, hwhddmodel.c_str(), hwhddmodel.size());

		std::string hwguidreg(packet->hwInfos.szMachineGUIDfromRegistry, std::min<std::size_t>(strlen(packet->hwInfos.szMachineGUIDfromRegistry), sizeof(packet->hwInfos.szMachineGUIDfromRegistry)));
		mysql_real_escape_string(m_pSQLConnector.native(), szMachineGUIDfromRegistry, hwguidreg.c_str(), hwguidreg.size());

		std::string hwguidapi(packet->hwInfos.szMachineGUIDfromWinapi, std::min<std::size_t>(strlen(packet->hwInfos.szMachineGUIDfromWinapi), sizeof(packet->hwInfos.szMachineGUIDfromWinapi)));
		mysql_real_escape_string(m_pSQLConnector.native(), szMachineGUIDfromWinapi, hwguidapi.c_str(), hwguidapi.size());

		std::string hwvolhash(packet->hwInfos.szMachineVolumeHash, std::min<std::size_t>(strlen(packet->hwInfos.szMachineVolumeHash), sizeof(packet->hwInfos.szMachineVolumeHash)));
		mysql_real_escape_string(m_pSQLConnector.native(), szMachineVolumeHash, hwvolhash.c_str(), hwvolhash.size());

		std::string hwcpuid(packet->hwInfos.szCpuID, std::min<std::size_t>(strlen(packet->hwInfos.szCpuID), sizeof(packet->hwInfos.szCpuID)));
		mysql_real_escape_string(m_pSQLConnector.native(), szCpuID, hwcpuid.c_str(), hwcpuid.size());

		std::string regproductit(packet->hwInfos.szRegProductId, std::min<std::size_t>(strlen(packet->hwInfos.szRegProductId), sizeof(packet->hwInfos.szRegProductId)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegProductId, regproductit.c_str(), regproductit.size());

		std::string reginstalldate(packet->hwInfos.szRegInstallDate, std::min<std::size_t>(strlen(packet->hwInfos.szRegInstallDate), sizeof(packet->hwInfos.szRegInstallDate)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegInstallDate, reginstalldate.c_str(), reginstalldate.size());

		std::string regscsiid(packet->hwInfos.szRegScsiId, std::min<std::size_t>(strlen(packet->hwInfos.szRegScsiId), sizeof(packet->hwInfos.szRegScsiId)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegScsiId, regscsiid.c_str(), regscsiid.size());

		std::string regscsiser(packet->hwInfos.szRegScsiSerial, std::min<std::size_t>(strlen(packet->hwInfos.szRegScsiSerial), sizeof(packet->hwInfos.szRegScsiSerial)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegScsiSerial, regscsiser.c_str(), regscsiser.size());

		std::string reghwid(packet->hwInfos.szRegHwid, std::min<std::size_t>(strlen(packet->hwInfos.szRegHwid), sizeof(packet->hwInfos.szRegHwid)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegHwid, reghwid.c_str(), reghwid.size());

		std::string regbiosdate(packet->hwInfos.szRegBiosDate, std::min<std::size_t>(strlen(packet->hwInfos.szRegBiosDate), sizeof(packet->hwInfos.szRegBiosDate)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegBiosDate, regbiosdate.c_str(), regbiosdate.size());

		std::string regbiosvendor(packet->hwInfos.szRegBiosVendor, std::min<std::size_t>(strlen(packet->hwInfos.szRegBiosVendor), sizeof(packet->hwInfos.szRegBiosVendor)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegBiosVendor, regbiosvendor.c_str(), regbiosvendor.size());

		std::string regbiosver(packet->hwInfos.szRegBiosVersion, std::min<std::size_t>(strlen(packet->hwInfos.szRegBiosVersion), sizeof(packet->hwInfos.szRegBiosVersion)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegBiosVersion, regbiosver.c_str(), regbiosver.size());

		std::string regsuscliid(packet->hwInfos.szRegSusClientID, std::min<std::size_t>(strlen(packet->hwInfos.szRegSusClientID), sizeof(packet->hwInfos.szRegSusClientID)));
		mysql_real_escape_string(m_pSQLConnector.native(), szRegSusClientID, regsuscliid.c_str(), regsuscliid.size());
	}
	catch (std::exception & e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "HwidTable escape string exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	std::string szHwidTableQuery;
	try
	{
		szHwidTableQuery = (boost::format("INSERT INTO hwid_table (ServerTime, PeerID, IpAddress, UUID, PlatformID, UniqueToken, NoMercyModuleName, NoMercyModuleHash, NoMercyModuleVersion, "
			"NoMercyBuildDate, HW_UserName, HW_ComputerName, HW_OS_Version, HW_OS_BuildVersion, HW_OS_ServicePackVersion, HW_MacAddress, HW_PhysicalMacAddress, HW_NetBiosMacAddress, "
			"HW_HddSerial, HW_HddModel, HW_MachineGUID_Registry, HW_MachineGUID_WinAPI, HW_CpuID, HW_VolumeHash, HW_Registry_BiosDate, HW_Registry_BiosVendor, HW_Registry_BiosVersion, "
			"HW_Registry_HWID, HW_Registry_InstallDate, HW_Registry_ProductID, HW_Registry_SCSI_ID, HW_Registry_SCSI_Serial, HW_Registry_SUS_ClientID) "
			"VALUES ('%1%', %2%, '%3%', '%4%', '%5%', '%6%', '%7%', '%8%', '%9%', '%10%', '%11%', '%12%', '%13%', '%14%', '%15%', '%16%', '%17%', '%18%', '%19%', '%20%', '%21%', "
			"'%22%', '%23%', '%24%', '%25%', '%26%', '%27%', '%28%', '%29%', '%30%', '%31%', '%32%', '%33%')") % \
			std::to_string(g_nmApp->FunctionsInstance()->GetCurrentTimestamp()) % peer->GetId() % peer->GetIP() % szUuid % szPlatformID % szUniqueToken % szNMModuleName % szNMModuleHash % szNMVersion % szNMBuildDate % szUserName % szComputerName % szOSVersion % \
			szBuildNumber % szServicePackVersion % szMacAddress % szPhysicalMacAddress % szNetBiosMacAddr % szHDDSerial % szHDDModel % szMachineGUIDfromRegistry % \
			szMachineGUIDfromWinapi % szCpuID % szMachineVolumeHash % szRegBiosDate % szRegBiosVendor % szRegBiosVersion % szRegHwid % szRegInstallDate % szRegProductId % \
			szRegScsiId % szRegScsiSerial % szRegSusClientID
		).str();
	}
	catch (std::exception & e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "HwidTable format exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

//	NETWORK_LOG(LL_ERR, "HwidTable query info, size: %u str: %s", szHwidTableQuery.size(), szHwidTableQuery.c_str());

	try
	{
		amy::execute(m_pSQLConnector, szHwidTableQuery, ec);
	}
	catch (std::exception& e)
	{
#ifdef _DEBUG
		NETWORK_LOG(LL_CRI, "HwidTable sql exception: %s", e.what());
#else
		UNREFERENCED_PARAMETER(e);
#endif

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	if (ec)
	{
		NETWORK_LOG(LL_CRI, "Hwid table insert query fail! Error: %u(%s)", ec.value(), ec.message().c_str());

		peer->Disconnect(ec);

		return TPacketEnterPacket::size();
	}

	peer->SendBlackLists();

	return TPacketEnterPacket::size();
}
