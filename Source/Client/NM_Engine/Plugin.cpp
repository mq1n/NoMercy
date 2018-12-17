#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Defines.h"
#include "Plugin.h"

bool CPlugin::OnPluginInfoReceived(const std::shared_ptr <SPluginInfoData> & pPluginInfo) // callback
{
	if (!IS_VALID_SMART_PTR(pPluginInfo))
		return false;

	if (!strlen(pPluginInfo->szPluginName))
		return false;

	auto szPluginPath = g_nmApp->DirFunctionsInstance()->ExePath() + xorstr("\\").crypt_get() + pPluginInfo->szPluginName;
	if (!g_nmApp->DirFunctionsInstance()->IsFileExist(szPluginPath))
		return false;

	if (g_nmApp->DirFunctionsInstance()->GetFileSize(szPluginPath) != pPluginInfo->dwFileSize)
		return false;

	if (g_nmApp->DirFunctionsInstance()->GetFileSHA256(szPluginPath) != pPluginInfo->szEggHash)
		return false;

	return true;
}

bool CPlugin::RequestPluginList()
{
	// TODO: Get plugin informations from master server and store on vPlugins
	return true;
}

bool CPlugin::LoadPlugins()
{
	char* szCurDir = (char*)malloc(MAX_PATH);
	GetCurrentDirectoryA(MAX_PATH,szCurDir);
	strcat_s(szCurDir,MAX_PATH / sizeof(char),"\\*");
	if (!strlen(szCurDir))
		return false;

	WIN32_FIND_DATAA FindDataw32;
	auto hFind = g_winapiApiTable->FindFirstFileA(szCurDir,&FindDataw32);
	if (!hFind || hFind == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if (!(FindDataw32.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if(strstr(FindDataw32.cFileName,".nmp") != NULL)
			{
				// IsCompressed & Decompress via LZ4

				// Serverside hash == Current hash check

				// Decrypt XOR'd aes key

				// Decrypt via AES256

				// Serverside hash check

				// Load with serverside selected type(LoadLibrary or Manual map)

				auto hPlugin = g_winapiApiTable->LoadLibraryA(FindDataw32.cFileName);
				if (!hPlugin)
				{
					printf("Plugin: %s can NOT loaded! Error: %u\n", FindDataw32.cFileName, GetLastError());
					continue;
				}

				auto pPlugin = std::make_shared<SPluginData>();
				if (!IS_VALID_SMART_PTR(pPlugin))
				{
					printf("Plugin data struct can not allocated for: %s. Error: %u\n", FindDataw32.cFileName, GetLastError());
					continue;
				}

				pPlugin->hPlugin		= hPlugin;
				pPlugin->dwName			= (DWORD_PTR)g_winapiApiTable->GetProcAddress(hPlugin, "PluginName");
				pPlugin->dwVersion		= (DWORD_PTR)g_winapiApiTable->GetProcAddress(hPlugin, "PluginVersion");
				pPlugin->dwErrorMessage = (DWORD_PTR)g_winapiApiTable->GetProcAddress(hPlugin, "PluginErrorMessage");
				pPlugin->dwInitRoutine	= (DWORD_PTR)g_winapiApiTable->GetProcAddress(hPlugin, "PluginOnInit");
				pPlugin->dwFinalRoutine = (DWORD_PTR)g_winapiApiTable->GetProcAddress(hPlugin, "PluginOnFinal");

				if (pPlugin->dwName && pPlugin->dwVersion && pPlugin->dwErrorMessage && pPlugin->dwInitRoutine && pPlugin->dwFinalRoutine)
				{
					printf("Plugin: %s succesfully loaded!\n", FindDataw32.cFileName);

					m_vLoadedPlugins.push_back(pPlugin);
					continue;
				}

				printf("An error occured on check exported functions! Data: %p-%p-%p-%p-%p\n",
					(void*)pPlugin->dwName, (void*)pPlugin->dwVersion, (void*)pPlugin->dwErrorMessage, (void*)pPlugin->dwInitRoutine, (void*)pPlugin->dwFinalRoutine);
				g_winapiApiTable->FreeLibrary(hPlugin);
			}
		}
	}
	while (g_winapiApiTable->FindNextFileA(hFind,&FindDataw32) != 0);

	free(szCurDir);
	return m_vLoadedPlugins.size() > 0;
}

bool CPlugin::ExecutePlugins()
{
	auto bRet = false;

	if (m_vLoadedPlugins.empty())
		return bRet;

	for (const auto & pCurrPlugin : m_vLoadedPlugins)
	{
		if (!IS_VALID_SMART_PTR(pCurrPlugin))
			continue;

		auto pPluginName		 = (TPluginName)pCurrPlugin->dwName;
		auto pPluginVersion		 = (TPluginVersion)pCurrPlugin->dwVersion;
		auto pPluginErrorMsg	 = (TPluginErrorMessage)pCurrPlugin->dwErrorMessage;
		auto pPluginInitRoutine  = (TPluginOnInit)pCurrPlugin->dwInitRoutine;
		auto pPluginFinalRoutine = (TPluginOnFinal)pCurrPlugin->dwFinalRoutine;

		printf("Current plugin: %s(%s) %p-%p\n", pPluginName(), pPluginVersion(), pPluginInitRoutine, pPluginFinalRoutine);

		bRet = pPluginInitRoutine(g_nmApp.get());
		printf("Plugin executed! Ret: %d Error: %s\n", bRet, pPluginErrorMsg());

		if (bRet == false)
			break;
	}
	return bRet;
}

bool CPlugin::FinalizePlugins()
{
	auto bRet = false;

	if (m_vLoadedPlugins.empty())
		return bRet;

	for (const auto & pCurrPlugin : m_vLoadedPlugins)
	{
		if (!IS_VALID_SMART_PTR(pCurrPlugin))
			continue;

		auto pPluginName		 = (TPluginName)pCurrPlugin->dwName;
		auto pPluginVersion		 = (TPluginVersion)pCurrPlugin->dwVersion;
		auto pPluginErrorMsg	 = (TPluginErrorMessage)pCurrPlugin->dwErrorMessage;
		auto pPluginInitRoutine  = (TPluginOnInit)pCurrPlugin->dwInitRoutine;
		auto pPluginFinalRoutine = (TPluginOnFinal)pCurrPlugin->dwFinalRoutine;

		printf("Current plugin: %s(%s) %p-%p\n", pPluginName(), pPluginVersion(), pPluginInitRoutine, pPluginFinalRoutine);

		bRet = pPluginFinalRoutine();
		printf("Plugin finalized! Ret: %d Error: %s\n", bRet, pPluginErrorMsg());

		if (bRet == false)
			break;
	}
	return bRet;
}

bool CPlugin::UnloadPlugins()
{
	auto bRet = false;

	if (m_vLoadedPlugins.empty())
		return bRet;

	for (const auto & pCurrPlugin : m_vLoadedPlugins)
	{
		if (!IS_VALID_SMART_PTR(pCurrPlugin))
			continue;

		auto pPluginName		 = (TPluginName)pCurrPlugin->dwName;
		auto pPluginVersion		 = (TPluginVersion)pCurrPlugin->dwVersion;
		auto pPluginErrorMsg	 = (TPluginErrorMessage)pCurrPlugin->dwErrorMessage;
		auto pPluginInitRoutine  = (TPluginOnInit)pCurrPlugin->dwInitRoutine;
		auto pPluginFinalRoutine = (TPluginOnFinal)pCurrPlugin->dwFinalRoutine;

		printf("Current plugin: %s(%s) %p-%p\n", pPluginName(), pPluginVersion(), pPluginInitRoutine, pPluginFinalRoutine);

		bRet = g_winapiApiTable->FreeLibrary(pCurrPlugin->hPlugin);
		printf("Plugin unload! Ret: %d Error: %u\n", bRet, GetLastError());

		if (bRet == false)
			break;
	}
	return bRet;
}

