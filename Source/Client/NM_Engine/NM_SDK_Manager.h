#pragma once
#include "../NM_ClientSDK/NoMercy.h"
#include "Metin2_SDK_Manager.h"

class CNM_SDK_Helper
{
	public:
		CNM_SDK_Helper();
		~CNM_SDK_Helper() = default;

#ifdef _DEBUG
		auto LogInstance() { return m_logHelper; };
#endif

		void OnGameTick();
		bool ProcessClientMessage(int Code, void* lpMessage);

		bool CreateMessageHandler(TNMCallback lpMessageHandler);
		bool SendMessageToClient(int Code, const char* c_szMessage, void* lpParam);

	private:
#ifdef _DEBUG
		std::shared_ptr <CLogger>		m_logHelper;
#endif

		TNMCallback						m_pMessageHandler;

		std::unique_ptr <CMetin2SDKMgr>	m_pMetin2Mgr;
};

