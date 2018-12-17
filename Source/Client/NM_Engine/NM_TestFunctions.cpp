#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "PEHelper.h"
#include "AntiDebug.h"
#include "ThreadExitCallback.h"
#include "ScannerInterface.h"

#include <rewolf-wow64ext/src/wow64ext.h>

#include "SocketUtils.h"
#include "../../Common/SocketPackets.h"
 /// ---

#include "WindowUI.h"
#include "DbgHelpWrapper.h"

DWORD WINAPI CheckQuarentine(LPVOID)
{
	while (1)
	{
		g_nmApp->QuarentineInstance()->CheckTestScan("Cheat Engine 6.7", "");
		Sleep(1000);
	}

	return 0;
}

void CNM_App::InitTestFunctions()
{
//	CreateThread(0, 0, CheckQuarentine, 0, 0, 0);

#if 0
	m_nmNetworkMgr->RegisterOnConnectedCallback(std::bind(&CNM_App::OnBackEndConnected, this));
	m_nmNetworkMgr->RegisterOnDisconnectedCallback(std::bind(&CNM_App::OnBackEndDisconnected, this));


	m_nmHwidManager->Initilize();
	m_nmNetworkMgr->InitSocketConnection();
#endif

#if 0
	if (m_nmAccessHelper->BlockAccess(NtCurrentProcess) == false)
	{
		DEBUG_LOG(LL_CRI, "ACCESS RULE 1 FAIL!!!");
	}
	DEBUG_LOG(LL_SYS, "Access rule 1 adjust step completed!");

	if (m_nmAccessHelper->BlockAccess(NtCurrentThread) == false)
	{
		DEBUG_LOG(LL_CRI, "ACCESS RULE 2 FAIL!!!");
	}
	DEBUG_LOG(LL_SYS, "Access rule 2 adjust step completed!");

	if (m_nmAccessHelper->DenyProcessAccess() == false)
	{
		DEBUG_LOG(LL_CRI, "ACCESS RULE 3 FAIL!!!");
	}
	DEBUG_LOG(LL_SYS, "Access rule 3 adjust step completed!");

	if (m_nmAccessHelper->DenyThreadAccess(NtCurrentThread) == false)
	{
		DEBUG_LOG(LL_CRI, "ACCESS RULE 4 FAIL!!!");
	}
	DEBUG_LOG(LL_SYS, "Access rule 4 adjust step completed!");

	if (IsWindowsVistaOrGreater())
	{
		m_nmAccessHelper->EnablePermanentDep();
	}
	DEBUG_LOG(LL_SYS, "Access rule 7 adjust step completed!");
#endif

#if 0
	if (g_nmApp->SelfHooksInstance()->InitializeSelfAPIHooks() == false)
	{
		DEBUG_LOG(LL_CRI, "Api hook init fail!!!");
	}
#endif#

#if 0
	uint64_t EPNameOffset = 0;
	uint64_t EPPidOffset = 0;
	uint64_t EPDirBaseOffset = 0;
	uint64_t EPBaseOffset = 0;
	uint64_t EPLinkOffset = 0;
	uint64_t EPObjectTable = 0;

	char szCurrentDirectory[MAX_PATH] = { 0 };
	if (!GetCurrentDirectoryA(MAX_PATH, szCurrentDirectory))
		return;

	char szSystemDirectory[MAX_PATH] = { 0 };
	if (!GetSystemDirectoryA(szSystemDirectory, MAX_PATH))
		return;

	auto szKernelSysName = szSystemDirectory + std::string("\\ntoskrnl.exe");
	auto szKernelLocalName = szCurrentDirectory + std::string("\\ntos.exe");

	if (FALSE == CopyFileA(szKernelSysName.c_str(), szKernelLocalName.c_str(), FALSE)) // todo copy to temp foler
		return;

	auto pDbgHelpMgr = std::make_unique<CDbgHelpWrapper>();
	if (!pDbgHelpMgr || !pDbgHelpMgr.get())
	{
		printf("Dbghel wrapper alloc fail! Error: %u\n", GetLastError());
		return;
	}
	printf("Dbghelper wrapper allocated!\n");

	if (!pDbgHelpMgr->InitializeDbgHelp())
	{
		printf("InitializeDbgHelp fail! Error: %u\n", GetLastError());
		return ;
	}
	printf("Dbghelper wrapper initialized!\n");

	if (!pDbgHelpMgr->LoadSymbols((char*)szKernelLocalName.c_str()))
	{
		printf("LoadSymbols fail! Error: %u\n", GetLastError());
		return ;
	}
	printf("Dbghelper wrapper symbols loaded!\n");

	auto ulEprocessIdx = 0UL;
	if (!pDbgHelpMgr->GetRootSymbol((char*)"_EPROCESS", &ulEprocessIdx))
	{
		printf("GetRootSymbol _EPROCESS fail! Error: %u\n", GetLastError());
		return ;
	}
	printf("EPROCESS Index: %u\n", ulEprocessIdx);

#define MAX_CHILD 512
	ULONG ulEprocessChilds[MAX_CHILD];
	ULONG ulEprocessChildCount = 0UL;
	if (!pDbgHelpMgr->GetChildrenSymbols(ulEprocessIdx, ulEprocessChilds, MAX_CHILD, ulEprocessChildCount))
	{
		printf("GetChildrenSymbols fail! Error: %u\n", GetLastError());
		return;
	}
	printf("Eprocess Child count: %u\n", ulEprocessChildCount);

	for (ULONG i = 0; i < ulEprocessChildCount; i++)
	{
		auto ulCurrChild = ulEprocessChilds[i];
		printf("Current index: %u Current child: %u\n", i, ulCurrChild);

		LPWSTR wszName = { L'\0' };
		if (!pDbgHelpMgr->GetSymbolName(ulCurrChild, &wszName))
		{
			printf("GetSymbolName fail! Error: %u\n", GetLastError());
			continue;
		}
		printf("Child: %u-%ls\n", ulCurrChild, wszName);

		auto ulCurrOffset = 0UL;
		if (!pDbgHelpMgr->GetSymbolOffset(ulCurrChild, &ulCurrOffset))
		{
			printf("GetSymbolOffset fail! Error: %u\n", GetLastError());
			continue;
		}
		printf("Offset: %ls -> %u\n", wszName, ulCurrOffset);

		if (!wcscmp(wszName, L"ImageFileName"))
		{
			EPNameOffset = ulCurrOffset;
		}
		else if (!wcscmp(wszName, L"UniqueProcessId"))
		{
			EPPidOffset = ulCurrOffset;
		}
		else if (!wcscmp(wszName, L"ActiveProcessLinks"))
		{
			EPLinkOffset = ulCurrOffset;
		}
		else if (!wcscmp(wszName, L"SectionBaseAddress"))
		{
			EPBaseOffset = ulCurrOffset;
		}
		else if (!wcscmp(wszName, L"ObjectTable"))
		{
			EPObjectTable = ulCurrOffset;
		}

		pDbgHelpMgr->FreeSymbolName(wszName);
	}
#endif

#if 0
	DWORD dwErr = 0;
	auto bRet = CAntiDebug::InitAntiDebug(&dwErr);
	DEBUG_LOG(LL_SYS, "ret: %d err: %u", bRet, dwErr);
#endif

//	m_nmManualMapHelper->RefreshModules();

//	g_nmApp->InitCheckModuleModifications();
}


