#pragma once
#include "NetLib.h"
#include "RSA.h"

#include <amy.hpp>

class CNetworkPeer;

class CNetworkServer : public NetServer
{
	private:
		typedef std::unordered_map <uint8_t, std::pair <std::size_t, std::function <std::size_t(std::shared_ptr <CNetworkPeer>, const void*, std::size_t)> > > MapPacketHandlers;
		struct DBConfig
		{
			std::string szAddress;
			std::string szUser;
			std::string szPassword;
			std::string szDatabase;
			uint16_t usPort = { 0 };
		};
	
	public:
		CNetworkServer(NetService & netService);
		virtual ~CNetworkServer();	

		NetService & GetServiceInstance() const;
	
		virtual void Init();
		virtual void Run();
		virtual void Shutdown();
		virtual bool IsShuttingDown() const;
	
		virtual std::shared_ptr <CNetworkPeer> FindPeer(int32_t id) const;
		virtual void RemovePeer(int32_t id);
	
		virtual std::size_t ProcessInput(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength);
	
		template <typename PacketT, typename HandlerT>
		inline void RegisterPacket(uint8_t type, HandlerT handler) 
		{ 
			m_pPacketHandlers.insert(std::make_pair(type, std::make_pair(PacketT::size(), handler)));
		}
		inline void RemovePacket(uint8_t type)
		{
			auto iter = m_pPacketHandlers.find(type);
			if (iter != m_pPacketHandlers.end())
				m_pPacketHandlers.erase(iter);
		}

		inline std::string GetBlackList(int32_t id)
		{
			auto iter = m_umSerializedBlacklists.find(id);
			if (iter != m_umSerializedBlacklists.end())
				return iter->second;
			return std::string("");
		}

		std::string GetServerVersion() { return m_szCfgMasterServerVersion; };
	
	protected:
		virtual std::shared_ptr <NetPeer> NewPeer();
	
		virtual bool ParseJsonConfig(const std::string & szConfigFileName);
		virtual void SetupPacketDispatchers();

		static void OnMySQLConnect(std::weak_ptr <CNetworkServer> self, const boost::system::error_code& e);
		static void ParseCheatBlackList(std::weak_ptr <CNetworkServer> self);
	
		std::size_t RecvConnectionInit(std::shared_ptr <CNetworkPeer> peer, const void * data, std::size_t maxlength);
		std::size_t RecvRSAInit(std::shared_ptr <CNetworkPeer> peer, const void * data, std::size_t maxlength);
		std::size_t RecvRSAFinalize(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength);
		std::size_t RecvEnterRequest(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength);
		std::size_t RecvCheatReport(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength);
		std::size_t RecvCryptedPacket(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength);
		std::size_t RecvLargeTestMsg(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength);
		std::size_t RecvNotificationPacket(std::shared_ptr <CNetworkPeer> peer, const void* data, std::size_t maxlength);

	private:
		NetService &		m_pNetService;
		amy::connector		m_pSQLConnector;
		MapPacketHandlers	m_pPacketHandlers;

		std::unordered_map <int32_t, std::string>						m_umSerializedBlacklists;
		std::unordered_map <int32_t, std::shared_ptr <CNetworkPeer> >	m_umPeerMap;
	
		// config
		std::string		m_szPortOrderID;
		std::string		m_szCfgMasterServerVersion;
		struct
		{
			std::string szHostName;
			uint16_t	usPort = { 0 };
			std::string szBindIP;
		} m_pServerCfg;
		DBConfig m_pNomercyCfgContext;
};

