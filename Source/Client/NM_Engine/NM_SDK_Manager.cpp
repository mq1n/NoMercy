#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_SDK_Manager.h"
#include "Defines.h"

#include <lazy_importer.hpp>

CNM_SDK_Helper::CNM_SDK_Helper() :
	m_pMessageHandler(nullptr)
{
	auto szPID					= std::to_string(LI_FIND(GetCurrentProcessId)());
	std::string szLoggerName	= xorstr("NoMercy_ClientSDK").crypt_get();
	auto szLoggerFileName		= xorstr("NoMercy/").crypt_get() + szLoggerName + xorstr("_").crypt_get() + szPID + xorstr(".log").crypt_get();

#ifdef _DEBUG
	m_logHelper = std::make_shared<CLogger>(szLoggerName, szLoggerFileName.c_str());
	if (!IS_VALID_SMART_PTR(m_logHelper)) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SDK_LOGGER_INIT_FAIL, LI_FIND(GetLastError)());
		return;
	}
#endif

	m_pMetin2Mgr = std::make_unique<CMetin2SDKMgr>();
	if (!IS_VALID_SMART_PTR(m_pMetin2Mgr))
	{
		SDK_LOG(LL_ERR, "m_pMetin2Mgr allocation failed! Last error: %u", LI_FIND(GetLastError)());
		return;
	}
}

void CNM_SDK_Helper::OnGameTick()
{
	switch (g_nmApp->DataInstance()->GetGameCode())
	{
		case METIN2_GAME:
		{
			m_pMetin2Mgr->OnGameTick();
		} break;

		default:
			break;
	}
}

bool CNM_SDK_Helper::ProcessClientMessage(int Code, void* lpMessage)
{
	SDK_LOG(LL_SYS, "Client message handled! Code: %d Msg: %p", Code, lpMessage);

#if 0
	auto pMessageBody = (SGDClientMsgBody*)c_lpMessage; // TODO

	if (memcmp(pMessageBody->pMagic, gs_pNetworkMagicValue, sizeof(pMessageBody->pMagic)))
	{
		// ...
	}

	auto pMessageContext = g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(pMessageBody->ulContextSize, __FUNCTION__);
	if (!pMessageContext)
	{
		// ...
	}

	auto dwChecksum = 0UL;
	if (false == GetChecksumNoMercyMessage(pMessageContext, pMessageBody->ulContextSize, &dwChecksum))
	{
		// ...
	}

	if (dwChecksum != pMessageBody->ulContextSum)
	{
		// ...
	}

	if (pMessageBody->bCrypted)
	{
		if (false == SecureNoMercyMessage(pMessageContext, pMessageBody->ulContextSize, pMessageBody->pKey, false)) // decrypt
		{
			// ...
		}

		auto dwPureChecksum = 0UL;
		if (false == GetChecksumNoMercyMessage(pMessageContext, pMessageBody->ulContextSize, &dwPureChecksum))
		{
			// ...
		}

		if (dwChecksum != pMessageBody->ulContextPureSum)
		{
			// ...
		}
	}
	// use pMessageContext instead of lpMessage
#endif

	switch (Code)
	{
		case NM_DATA_SEND_GAME_CODE:
		{
			g_nmApp->DataInstance()->SetGameCode(reinterpret_cast<int>(lpMessage));
		} break;

		case NM_DATA_SEND_POINTER_REDIRECTION_COMPLETED_SIGNAL:
		{
			auto pVersion = reinterpret_cast<void*>(NOMERCY_ALLOWED_VERSION);
			return SendMessageToClient(NM_DATA_RECV_VERSION, nullptr, pVersion);
		} break;

		default:
			break;
	}
	
	switch (g_nmApp->DataInstance()->GetGameCode())
	{
		case METIN2_GAME:
		{
			m_pMetin2Mgr->OnClientMessage(Code, lpMessage);
		} break;

		default:
			SDK_LOG(LL_ERR, "Game code is undefined for process client messages!");
			break;
	}

#if 0
	g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pMessageContext);
	if (pMessageBody->pbCompleted) *pMessageBody->pbCompleted = true;
#endif
	return true;
}


bool CNM_SDK_Helper::CreateMessageHandler(TNMCallback lpMessageHandler)
{
	m_pMessageHandler = lpMessageHandler;
	return true;
}

bool CNM_SDK_Helper::SendMessageToClient(int Code, const char* c_szMessage, void* lpParam)
{
	if (!m_pMessageHandler)
	{
		SDK_LOG(LL_ERR, "Message handler not yet created!");
		return false;
	}

	m_pMessageHandler(static_cast<NM_MESSAGE_CODES>(Code), c_szMessage, lpParam);
	return true;
}

