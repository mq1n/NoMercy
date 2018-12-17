#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Analyser.h"
#include "ProcessFunctions.h"
#include "Defines.h"

void CAnalyser::OnWMITriggered(int analyseType, std::map <std::string /* szType */, std::string /* szValue */> mDataMap, bool bProcessTerminated)
{
#if 0
	DEBUG_LOG(LL_SYS, "Analyse type: %d", analyseType);

	for (const auto & currentElement : mDataMap)
	{
		auto szType = std::string(currentElement.first.begin(), currentElement.first.end());
		auto szValue = std::string(currentElement.second.begin(), currentElement.second.end());

		DEBUG_LOG(LL_SYS, "Type: %s Value: %s", szType.c_str(), szValue.c_str());
	}
#endif

	switch (analyseType)
	{
		case EAnalyseTypes::ANALYSE_PROCESS:
		case EAnalyseTypes::ANALYSE_PROCESS_2:
		{
			if (!bProcessTerminated)
			{
				auto procInfos = std::make_shared<SProcessCallbackCtx>();
				if (!IS_VALID_SMART_PTR(procInfos))
					return;
				
				auto it_dwPID = mDataMap.find(xorstr("ProcessId").crypt_get());
				if (it_dwPID != mDataMap.end())
					procInfos->dwProcessId = std::stol(it_dwPID->second);

				auto it_dwSID = mDataMap.find(xorstr("SessionId").crypt_get());
				if (it_dwSID != mDataMap.end())
					procInfos->iSID = std::stoi(it_dwSID->second);

				auto it_iThreadCount = mDataMap.find(xorstr("ThreadCount").crypt_get());
				if (it_iThreadCount != mDataMap.end())
					procInfos->iThreadCount = std::stoi(it_iThreadCount->second);

				auto it_szClassName = mDataMap.find(xorstr("CreationClassName").crypt_get());
				if (it_szClassName != mDataMap.end())
					strcpy(procInfos->szClassname, it_szClassName->second.c_str());

				auto it_szCommandLine = mDataMap.find(xorstr("CommandLine").crypt_get());
				if (it_szCommandLine != mDataMap.end())
					strcpy(procInfos->szCommandline, it_szCommandLine->second.c_str());

				auto it_szExePathAndName = mDataMap.find(xorstr("ExecutablePath").crypt_get());
				if (it_szExePathAndName != mDataMap.end())
					strcpy(procInfos->szFileAndPathname, it_szExePathAndName->second.c_str());

				auto it_szName = mDataMap.find(xorstr("Name").crypt_get());
				if (it_szName != mDataMap.end())
					strcpy(procInfos->szFilename, it_szName->second.c_str());

//				g_nmApp->ScannerInstance()->OnWatcherProcessScan(procInfos, bProcessTerminated == true ? false : true);
			}
		} break;

		case EAnalyseTypes::ANALYSE_THREAD:
		{
			auto threadInfos = std::make_shared<SThreadCallbackCtx>();
			if (!IS_VALID_SMART_PTR(threadInfos))
				return;

			auto it_dwTID = mDataMap.find(xorstr("ThreadID").crypt_get());
			if (it_dwTID != mDataMap.end())
				threadInfos->dwTID = std::stol(it_dwTID->second);

			auto it_dwPID = mDataMap.find(xorstr("ProcessID").crypt_get());
			if (it_dwPID != mDataMap.end())
				threadInfos->dwProcessId = std::stol(it_dwPID->second);

			auto it_dwWaitMode = mDataMap.find(xorstr("WaitMode").crypt_get());
			if (it_dwWaitMode != mDataMap.end())
				threadInfos->dwWaitMode = std::stol(it_dwWaitMode->second);

			auto it_dwStartAddress = mDataMap.find(xorstr("Win32StartAddr").crypt_get());
			if (it_dwStartAddress != mDataMap.end())
				threadInfos->dwStartAddress = std::stol(it_dwStartAddress->second);

//			g_nmApp->ScannerInstance()->OnWatcherThreadScan(threadInfos);
		} break;

		case EAnalyseTypes::ANALYSE_MODULE:
		{
			auto moduleInfos = std::make_shared<SModuleCallbackCtx>();
			if (!IS_VALID_SMART_PTR(moduleInfos))
				return;

			auto it_dwPID = mDataMap.find(xorstr("ProcessID").crypt_get());
			if (it_dwPID != mDataMap.end())
				moduleInfos->dwProcessId = std::stol(it_dwPID->second);

			auto it_dwTID = mDataMap.find(xorstr("ImageBase").crypt_get());
			if (it_dwTID != mDataMap.end())
				moduleInfos->dwBaseAddress = std::stol(it_dwTID->second);

			auto it_dwWaitMode = mDataMap.find(xorstr("ImageSize").crypt_get());
			if (it_dwWaitMode != mDataMap.end())
				moduleInfos->dwImageSize = std::stol(it_dwWaitMode->second);

			auto it_szName = mDataMap.find(xorstr("FileName").crypt_get());
			if (it_szName != mDataMap.end())
				strcpy(moduleInfos->szFilename, it_szName->second.c_str());

//			g_nmApp->ScannerInstance()->OnWatcherModuleScan(moduleInfos);
		} break;

		case EAnalyseTypes::ANALYSE_DRIVER:
		{
			auto driverInfos = std::make_shared<SDriverCallbackCtx>();
			if (!IS_VALID_SMART_PTR(driverInfos))
				return;

			auto it_szName = mDataMap.find(xorstr("Name").crypt_get());
			if (it_szName != mDataMap.end())
				strcpy(driverInfos->szName, it_szName->second.c_str());
			else
				return;

			auto it_szPath = mDataMap.find(xorstr("PathName").crypt_get());
			if (it_szPath != mDataMap.end())
				strcpy(driverInfos->szPath, it_szPath->second.c_str());
			else
				return;

			auto it_szState = mDataMap.find(xorstr("State").crypt_get());
			if (it_szState != mDataMap.end())
				strcpy(driverInfos->szState, it_szState->second.c_str());
			else
				return;

			auto it_szServiceType = mDataMap.find(xorstr("ServiceType").crypt_get());
			if (it_szServiceType != mDataMap.end())
				strcpy(driverInfos->szType, it_szServiceType->second.c_str());
			else
				return;

			auto it_bStarted = mDataMap.find(xorstr("Started").crypt_get());
			if (it_bStarted != mDataMap.end())
				driverInfos->bStarted = std::stol(it_bStarted->second) ? true : false;
			else
				return;

//			g_nmApp->ScannerInstance()->OnWatcherDriverScan(driverInfos);
		} break;
	}
}


