#pragma once
#include <cstdint>
#include <map>
#include <string>

static uint8_t gs_pNetworkMagicValue[] = { 0x59, 0x43, 0x4D, 0x4E }; // N M C Y

enum ENoMercyConnectionTypes
{
	NM_CONNECTION_TYPE_NULL,
	NM_CONNECTION_TYPE_CLIENT,
	NM_CONNECTION_TYPE_SERVICE,
	NM_CONNECTION_TYPE_P2P_MASTER_SERVER,
	NM_CONNECTION_TYPE_GAME_SERVER,
	NM_CONNECTION_TYPE_WEB_INTERFACE
};

enum EServerAuthenticationTypes
{
	AUTH_TYPE_NONE			= (1 << 0),
	AUTH_TYPE_UUID			= (1 << 1),
	AUTH_TYPE_STEAM_ID		= (1 << 2),
	AUTH_TYPE_UNIQUE_TOKEN	= (1 << 3)
};

enum ENetworkScanIDs
{
	NET_SCAN_ID_NULL,
	NET_SCAN_ID_TEST,
	NET_SCAN_ID_MAX
};

enum EViolationIDs
{
	ERR_VIOLATION_NULL,
	ERR_VIOLATION_TEST_SCAN,
	ERR_VIOLATION_PROCESS_BASE_SCAN,
	ERR_VIOLATION_PROCESS_ICON_HASH_SCAN,
	ERR_VIOLATION_FILE_EXIST_CHECK,
	ERR_VIOLATION_FILE_BASIC_INFORMATION_CHECK,
	ERR_VIOLATION_FILE_INFORMATION_CHECK,
	ERR_VIOLATION_FILE_CERTIFICATE_CHECK,
	ERR_VIOLATION_MAPPED_FILE_PE_CHECK,
	ERR_VIOLATION_MAPPED_FILE_SECTION_NAME_CHECK,
	ERR_VIOLATION_MAPPED_FILE_EAT_CHECK,
	ERR_VIOLATION_MAPPED_FILE_IAT_CHECK,
	ERR_VIOLATION_MAPPED_FILE_REGION_HASH_CHECK
};

enum ENotificationIDs
{
	NOTIFICATION_ID_NULL,
	NOTIFICATION_CS_BLACKLIST_PARSE_OK,
};

enum EPacketHeaders
{
	HEADER_PACKET_SINGLETION			= 1,

	HEADER_CS_CONN_INIT					= 11,
	HEADER_SC_PRE_KEY_INFO				= 12,
	HEADER_CS_RSA_INIT					= 13,
	HEADER_SC_RSA_KEY_INFO				= 14,
	HEADER_CS_RSA_KEY_FINALIZE			= 15,
	HEADER_SC_RSA_CHECK_COMPLETED		= 16,
	HEADER_CS_ENTER_INFORMATIONS		= 17,
	HEADER_SC_CHEAT_BLACKLISTS			= 18,
	HEADER_SC_ROUTINE_PACKET			= 19,
	HEADER_CS_CHEAT_REPORT				= 20,

	HEADER_NOTIFICATION_PACKET			= 96,
	HEADER_TEST_MSG						= 97,
	HEADER_TEST_MSG_LARGE				= 98,
	HEADER_CRYPTED_PACKET				= 99
};

static std::string GetPacketString(int8_t pPacketID)
{
	static std::map <int8_t, std::string> s_mPacketHeaders = 
	{
		{ HEADER_CS_CONN_INIT,				xorstr("HEADER_CS_CONN_INIT").crypt_get()			},
		{ HEADER_SC_PRE_KEY_INFO,			xorstr("HEADER_SC_PRE_KEY_INFO").crypt_get()		},
		{ HEADER_CS_RSA_INIT,				xorstr("HEADER_CS_RSA_INIT").crypt_get()			},
		{ HEADER_SC_RSA_KEY_INFO,			xorstr("HEADER_SC_RSA_KEY_INFO").crypt_get()		},
		{ HEADER_CS_RSA_KEY_FINALIZE,		xorstr("HEADER_CS_RSA_KEY_FINALIZE").crypt_get()	},
		{ HEADER_SC_RSA_CHECK_COMPLETED,	xorstr("HEADER_SC_RSA_CHECK_COMPLETED").crypt_get() },
		{ HEADER_CS_ENTER_INFORMATIONS,		xorstr("HEADER_CS_ENTER_INFORMATIONS").crypt_get()	},
		{ HEADER_SC_CHEAT_BLACKLISTS,		xorstr("HEADER_SC_CHEAT_BLACKLISTS").crypt_get()	},
		{ HEADER_SC_ROUTINE_PACKET,			xorstr("HEADER_SC_ROUTINE_PACKET").crypt_get()		},
		{ HEADER_CS_CHEAT_REPORT,			xorstr("HEADER_CS_CHEAT_REPORT").crypt_get()		},
		{ HEADER_TEST_MSG,					xorstr("HEADER_TEST_MSG").crypt_get()				},
		{ HEADER_TEST_MSG_LARGE,			xorstr("HEADER_TEST_MSG_LARGE").crypt_get()			},
		{ HEADER_CRYPTED_PACKET,			xorstr("HEADER_CRYPTED_PACKET").crypt_get()			},
	};

	if (pPacketID <= 0 || pPacketID > 255)
		return std::string(xorstr("Unallowed packet id: ").crypt_get() + pPacketID);

	auto it = s_mPacketHeaders.find(pPacketID);
	if (it == s_mPacketHeaders.end())
		return std::string(xorstr("Undefined packet: ").crypt_get() + pPacketID);

	return it->second;
}

#define BEGIN_PACKET_DEF(name, type) \
 typedef struct name\
 {\
	uint8_t pMagic[4];\
	uint32_t uiPacketID;\
	static size_t  size() { return sizeof(name); }\
	name() { memset(this, 0, sizeof(name)); uiPacketID = type; }

#define END_PACKET_DEF(name)\
	uint32_t ulChecksum;\
	uint32_t ulHash;\
 } name;

#pragma pack(push, 1)

// Psuedo packet template
BEGIN_PACKET_DEF(SPacketSingleton, HEADER_PACKET_SINGLETION)
END_PACKET_DEF(TPacketSingleton)

// Connection Initilize notification packet
BEGIN_PACKET_DEF(SPacketConnectionInit, HEADER_CS_CONN_INIT)
	int8_t		pConnectionType;
END_PACKET_DEF(TPacketConnectionInit)

// Pre-encryption key notification packet
BEGIN_PACKET_DEF(SPacketPreCryptKeyInfo, HEADER_SC_PRE_KEY_INFO)
	uint8_t		dummyDataFirst[36 + 1];
	uint8_t		preCryptKey[32 + 1];
	uint8_t		dummyDataLast[48 + 1];
END_PACKET_DEF(TPacketPreCryptKeyInfo)

// RSA Initilization packet
BEGIN_PACKET_DEF(SPacketRsaInit, HEADER_CS_RSA_INIT)
	uint8_t		publicExponent[16];
	uint32_t	publicExponentSize;

	uint8_t		modulus[256];
	uint32_t	modulusSize;
END_PACKET_DEF(TPacketRsaInit)

// RSA public key transfer packet
BEGIN_PACKET_DEF(SPacketRsaKeyInfo, HEADER_SC_RSA_KEY_INFO)
	uint8_t		cryptkey[256];
	uint32_t	cryptkeysize;
END_PACKET_DEF(TPacketRsaKeyInfo)

// RSA finalization packet
BEGIN_PACKET_DEF(SPacketRsaKeyFinalize, HEADER_CS_RSA_KEY_FINALIZE)
	uint8_t		dummyDataFirst[16 + 1];
	char		rsaKeyHash[256 + 1];
	uint8_t		dummyDataLast[24 + 1];
END_PACKET_DEF(TPacketRsaKeyFinalize)

// RSA complete notification packet
BEGIN_PACKET_DEF(SPacketRsaCheckCompleted, HEADER_SC_RSA_CHECK_COMPLETED)
	bool bCompleted;
	DWORD dwServerVersion;
END_PACKET_DEF(TPacketRsaCheckCompleted)

// Simple enter packet
typedef struct _HARDWARE_INFORMATIONS
{
	char szMacAddress[255];
	char szPhysicalMacAddress[255];
	char szNetBiosMacAddr[255];

	char szHDDSerial[255];
	char szHDDModel[255];

	char szCpuID[255];

	char szUserName[255];
	char szComputerName[255];

	char szOSVersion[255];
	char szBuildNumber[255];
	char szServicePackVersion[255];

	char szMachineGUIDfromRegistry[255];
	char szMachineGUIDfromWinapi[255];
	char szMachineVolumeHash[255];

	char szRegProductId[255];
	char szRegInstallDate[255];
	char szRegScsiId[255];
	char szRegScsiSerial[255];
	char szRegHwid[255];
	char szRegBiosDate[255];
	char szRegBiosVendor[255];
	char szRegBiosVersion[255];
	char szRegSusClientID[255];
}SHwidInfos, *PHwidInfos;

typedef struct _GAME_INFORMATIONS
{
	char		szLicenseCode[255];
	DWORD		dwProcessID;
	char		szExecutable[MAX_PATH];
	char		szParentExecutable[MAX_PATH];
	char		szCommandline[4096];
	char		szTitle[255];
	char		szChecksum[255];
	bool		bIsX64;
}SGameInfos, *PGameInfos;

typedef struct _NOMERCY_INFORMATIONS
{
	char		szNMVersion[255];
	DWORD		dwNMStage;
	char		szNMHash[255];
	char		szNMModuleName[255];
	char		szNMBuildDate[255];
}SNMInfos, *PNMInfos;

BEGIN_PACKET_DEF(SPacketEnterPacket, HEADER_CS_ENTER_INFORMATIONS)
	DWORD		dwTimestamp;
	DWORD		dwTickCount;

	DWORD		dwGameID;
	SGameInfos	gameInfos;

	BYTE		pAuthType;
	char		szNmUuid[255];
	char		szNmPlatformId[255];
	char		szUniqueToken[255];
	SHwidInfos	hwInfos;

	DWORD		dwAppType;
	SNMInfos	nmInfos;
END_PACKET_DEF(TPacketEnterPacket)

// Cheat blacklists
BEGIN_PACKET_DEF(SPacketCheatBlacklists, HEADER_SC_CHEAT_BLACKLISTS)
	char ssBlackList1[8192];
	char ssBlackList2[8192];
	char ssBlackList3[8192];
	char ssBlackList4[8192];
	char ssBlackList5[8192];
END_PACKET_DEF(TPacketCheatBlackLists)

// Routine packet
BEGIN_PACKET_DEF(SPacketRoutinePacket, HEADER_SC_ROUTINE_PACKET)
END_PACKET_DEF(TPacketRoutinePacket)

// Cheat report packet
BEGIN_PACKET_DEF(SPacketDetectionInfo, HEADER_CS_CHEAT_REPORT)
	uint32_t	dwViolation;
	uint32_t	dwErrorCode;
	char		szMessage[4096];
END_PACKET_DEF(TPacketDetectionInfo)

// Notification routine with dynamic id
BEGIN_PACKET_DEF(SPacketNotification, HEADER_NOTIFICATION_PACKET)
	uint32_t uiNotificationID;
END_PACKET_DEF(TPacketNotification)

// Test
BEGIN_PACKET_DEF(SPacketTestMsg, HEADER_TEST_MSG)
	char szMessage[1024 * 4];
END_PACKET_DEF(TPacketTestMsg)

BEGIN_PACKET_DEF(SPacketTestMsgX, HEADER_TEST_MSG_LARGE)
	char szMessage[1024 * 1024];
END_PACKET_DEF(TPacketTestMsgX)

// Crypted packet body
BEGIN_PACKET_DEF(SCryptedPacket, HEADER_CRYPTED_PACKET)
	uint32_t	uiDecryptedPacketId;

	uint8_t		pContext[NETWORK_MAX_ALLOWED_SIZE - 0x40 /* current packet size */];
	uint32_t	ulCryptedSize;
	uint32_t	ulDecryptedSize;
	uint32_t	ulCryptedSum;
	uint32_t	ulDecryptedSum;
END_PACKET_DEF(TCryptedPacket)

// Packet capacity helper func
static std::size_t GetPacketCapacity(uint32_t header)
{
	switch (header)
	{
		case HEADER_PACKET_SINGLETION:
			return TPacketSingleton::size();
		case HEADER_CS_CONN_INIT:
			return TPacketConnectionInit::size();
		case HEADER_SC_PRE_KEY_INFO:
			return TPacketPreCryptKeyInfo::size();
		case HEADER_CS_RSA_INIT:
			return TPacketRsaInit::size();
		case HEADER_SC_RSA_KEY_INFO:
			return TPacketRsaKeyInfo::size();
		case HEADER_CS_RSA_KEY_FINALIZE:
			return TPacketRsaKeyFinalize::size();
		case HEADER_SC_RSA_CHECK_COMPLETED:
			return TPacketRsaCheckCompleted::size();
		case HEADER_CS_ENTER_INFORMATIONS:
			return TPacketEnterPacket::size();
		case HEADER_SC_CHEAT_BLACKLISTS:
			return TPacketCheatBlackLists::size();
		case HEADER_SC_ROUTINE_PACKET:
			return TPacketRoutinePacket::size();
		case HEADER_CS_CHEAT_REPORT:
			return TPacketDetectionInfo::size();
		case HEADER_NOTIFICATION_PACKET:
			return TPacketNotification::size();
		case HEADER_CRYPTED_PACKET:
			return TCryptedPacket::size();
		case HEADER_TEST_MSG:
			return TPacketTestMsg::size();
		case HEADER_TEST_MSG_LARGE:
			return TPacketTestMsgX::size();
	}

	return 0UL;
}

#pragma pack(pop)

