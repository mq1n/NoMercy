#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "WMI.h"
#include "ProcessFunctions.h"
#include "Defines.h"

inline bool DumpWMIClassObject(int iAnalyseType, IWbemClassObject * pClassObject, bool bProcessCreated, int iIndentationLevel)
{
	auto mDataMap = std::map<std::string, std::string>();

	SAFEARRAY * pStrNames;
	auto hError = pClassObject->GetNames(NULL, WBEM_FLAG_ALWAYS | WBEM_FLAG_NONSYSTEM_ONLY, NULL, &pStrNames);
	if (FAILED(hError))
		return false;

	auto lLowerBound = 0L;
	hError = SafeArrayGetLBound(pStrNames, 1, &lLowerBound);
	if (FAILED(hError))
		return false;

	auto lUpperBound = 0L;
	hError = SafeArrayGetUBound(pStrNames, 1, &lUpperBound);
	if (FAILED(hError))
		return false;

	auto lElementCount = lUpperBound - lLowerBound + 1;
	if (!lElementCount)
		return false;

	for (auto i = 0L; i < lElementCount; i++)
	{
		auto bszName = ((BSTR*)pStrNames->pvData)[i];

		VARIANT varVal = { 0 };
		CIMTYPE cymType = 0;
		hError = pClassObject->Get(bszName, 0, &varVal, &cymType, NULL);
		if (SUCCEEDED(hError))
		{		
			if (wcscmp(bszName, xorstr(L"TargetInstance").crypt_get()) == 0)
			{
				DumpWMIClassObject(iAnalyseType, reinterpret_cast<IWbemClassObject*>(varVal.uintVal), bProcessCreated, iIndentationLevel + 1);
			}
			
			else if (cymType == CIM_STRING)
			{
				if (varVal.bstrVal)
				{
					std::wstring wszName(bszName, SysStringLen(bszName));
					std::string szName(wszName.begin(), wszName.end());

					std::wstring wszValue(varVal.bstrVal, SysStringLen(varVal.bstrVal));
					std::string szValue(wszValue.begin(), wszValue.end());

					mDataMap[szName] = szValue;
				}
			}

			else 
			{
				std::wstring wszName(bszName, SysStringLen(bszName));
				std::string szName(wszName.begin(), wszName.end());

				mDataMap[szName] = std::to_string(varVal.uintVal);
			}
		}
	}

	g_nmApp->AnalyserInstance()->OnWMITriggered(iAnalyseType, mDataMap, bProcessCreated);
	return true;
}


CWMI::CWMI() : 
	m_bWatcherIsTerminated(false), m_pWbemServices(nullptr)
{
}


void CWMI::TerminateWatcher()
{
	m_bWatcherIsTerminated = true;
}

bool CWMI::WatcherIsTerminated()
{
	return m_bWatcherIsTerminated;
}

DWORD WINAPI CWMI::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CWMI*)lpParam;
	return This->ThreadRoutine();
}

bool CWMI::AnalyseModules(DWORD dwTargetProcessId) 
{
	if (!m_pWbemServices)
		return false;

	auto wszQuery = const_cast<wchar_t*>(g_nmApp->FunctionsInstance()->CreateString(xorstr(L"SELECT * FROM Win32_ModuleLoadTrace WHERE ProcessID = %u").crypt_get(), dwTargetProcessId).c_str());

	EventSink eventModuleLoaded(EAnalyseTypes::ANALYSE_MODULE_2, false);
	auto hRes = m_pWbemServices->ExecNotificationQueryAsync(xorstr(L"WQL").crypt_get(), wszQuery, 0, NULL, &eventModuleLoaded);
	return hRes == S_OK;
}

DWORD CWMI::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "WMI Watcher thread event has been started!");

	auto pWMITimer		= CTimer<std::chrono::milliseconds>();

	auto currentThread	= std::shared_ptr<CThread>();
	do
	{
		if (pWMITimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_WMI_CREATE_THREAD_FAIL, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_WMI);

	} while (!IS_VALID_SMART_PTR(currentThread));


	auto wszWQL					= xorstr(L"WQL").crypt_get();
	auto wszRootCim				= xorstr(L"root\\cimv2").crypt_get();
	auto wszProcessCreateQuery	= xorstr(L"SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process'").crypt_get();
	auto wszProcessDeleteQuery	= xorstr(L"SELECT * FROM Win32_ProcessStopTrace").crypt_get();
	auto wszThreadLoadQuery		= xorstr(L"SELECT * FROM Win32_ThreadTrace").crypt_get();
	auto wszModuleLoadQuery		= xorstr(L"SELECT * FROM Win32_ModuleLoadTrace").crypt_get();
	auto wszDriverLoadQuery		= xorstr(L"SELECT * FROM __InstanceCreationEvent WITHIN 5 WHERE TargetInstance ISA 'Win32_SystemDriver'").crypt_get();
	auto wszServiceLoadQuery	= xorstr(L"SELECT * FROM __InstanceCreationEvent WITHIN 5 WHERE TargetInstance ISA 'Win32_Service'").crypt_get();

	// ---
	auto wszBaseBoardQuery		= xorstr(L"SELECT SerialNumber,Manufacturer,Product FROM Win32_BaseBoard").crypt_get();
	auto wszDiskDriveQuery		= xorstr(L"SELECT Caption,DeviceID,Model,SerialNumber FROM Win32_DiskDrive").crypt_get();
	auto wszBiosQuery			= xorstr(L"SELECT Caption,Manufacturer,SerialNumber FROM Win32_BIOS").crypt_get();
	auto wszNetAdapQuery		= xorstr(L"SELECT GUID,MACAddress,NetConnectionID,PNPDeviceID,NetConnectionStatus FROM Win32_NetworkAdapter").crypt_get();
	auto wszNetAdapConfQuery	= xorstr(L"SELECT MACAddress,Index,InterfaceIndex,DefaultIPGateway FROM Win32_NetworkAdapterConfiguration").crypt_get();
	auto wszOSQuery				= xorstr(L"SELECT SerialNumber,InstallDate,Version,Organization,Debug FROM Win32_OperatingSystem").crypt_get();
	auto wszPhysMemQuery		= xorstr(L"SELECT SerialNumber,PartNumber FROM Win32_PhysicalMemory").crypt_get();
	auto wszSoundDevQuery		= xorstr(L"SELECT PnpDeviceId FROM Win32_SoundDevice").crypt_get();
	auto wszVideoCtrlQuery		= xorstr(L"SELECT PnpDeviceId,VideoProcessor,DeviceID,DriverVersion,InstallDate FROM Win32_VideoController").crypt_get();
	auto wszIDECtrlQuery		= xorstr(L"SELECT PnpDeviceId,Name FROM Win32_IDEController").crypt_get();
	auto wszProcessorQuery		= xorstr(L"SELECT ProcessorId,Name FROM Win32_Processor").crypt_get();
	auto wszPnpEntityQuery		= xorstr(L"SELECT DeviceId FROM Win32_PnPEntity").crypt_get();
	auto wszPhysMediaQuery		= xorstr(L"SELECT Name,MediaType,Capacity,Manufacturer,SerialNumber FROM Win32_PhysicalMedia").crypt_get();

	// ---

	auto hError = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hError)) 
	{
		DEBUG_LOG(LL_ERR, "CoInitializeEx fail! hError: %p Last error: %u", hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_INIT_FAIL_1, g_winapiApiTable->GetLastError());
		return 0;
	}

	hError = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT /* RPC_C_AUTHN_LEVEL_PKT */, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "CoInitializeSecurity fail! hError: %p Last error: %u", hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_INIT_FAIL_2, g_winapiApiTable->GetLastError());
		return 0;
	}

	IWbemLocator * pLoc = nullptr;
	hError = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "CoCreateInstance fail! hError: %p Last error: %u", hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_INIT_FAIL_3, g_winapiApiTable->GetLastError());
		return 0;
	}

	hError = pLoc->ConnectServer(wszRootCim, NULL, NULL, NULL, WBEM_FLAG_CONNECT_USE_MAX_WAIT /* 0 */, NULL, NULL, &m_pWbemServices);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ConnectServer fail! hError: %p Last error: %u", hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_INIT_FAIL_4, g_winapiApiTable->GetLastError());
		return 0;
	}

	DEBUG_LOG(LL_SYS, "WMI Watcher created!");

//	m_pWbemServices->ExecQuery(wszWQL, "query", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &eventsink);

	EventSink eventProcCreated(EAnalyseTypes::ANALYSE_PROCESS, true);
	hError = m_pWbemServices->ExecNotificationQueryAsync(wszWQL, wszProcessCreateQuery, WBEM_FLAG_SEND_STATUS, NULL, &eventProcCreated);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecNotificationQueryAsync(%d) fail! hError: %p Last error: %u", EAnalyseTypes::ANALYSE_PROCESS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_1, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventProcTerminated(EAnalyseTypes::ANALYSE_PROCESS_2, false);
	hError = m_pWbemServices->ExecNotificationQueryAsync(wszWQL, wszProcessDeleteQuery, 0, NULL, &eventProcTerminated);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecNotificationQueryAsync(%d) fail! hError: %p Last error: %u", EAnalyseTypes::ANALYSE_PROCESS_2, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_2, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventThreadCreated(EAnalyseTypes::ANALYSE_THREAD, false);
	hError = m_pWbemServices->ExecNotificationQueryAsync(wszWQL, wszThreadLoadQuery, 0, NULL, &eventThreadCreated);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecNotificationQueryAsync(%d) fail! hError: %p Last error: %u", EAnalyseTypes::ANALYSE_THREAD, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_3, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventModuleLoaded(EAnalyseTypes::ANALYSE_MODULE, false);
	hError = m_pWbemServices->ExecNotificationQueryAsync(wszWQL, wszModuleLoadQuery, 0, NULL, &eventModuleLoaded);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecNotificationQueryAsync(%d) fail! hError: %p Last error: %u", EAnalyseTypes::ANALYSE_MODULE, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_4, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventDriverLoaded(EAnalyseTypes::ANALYSE_DRIVER, false);
	hError = m_pWbemServices->ExecNotificationQueryAsync(wszWQL, wszDriverLoadQuery, 0, NULL, &eventDriverLoaded);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecNotificationQueryAsync(%d) fail! hError: %p Last error: %u", EAnalyseTypes::ANALYSE_DRIVER, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_5, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventServiceLoaded(EAnalyseTypes::ANALYSE_SERVICE, false);
	hError = m_pWbemServices->ExecNotificationQueryAsync(wszWQL, wszServiceLoadQuery, 0, NULL, &eventServiceLoaded);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecNotificationQueryAsync(%d) fail! hError: %p Last error: %u", EAnalyseTypes::ANALYSE_SERVICE, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_6, g_winapiApiTable->GetLastError());
		return 0;
	}

	// ---

	EventSink eventWmiBaseboardInformations(EWMIQueryTypes::WMI_BASEBOARD_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszBaseBoardQuery, 0, NULL, &eventWmiBaseboardInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_BASEBOARD_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_7, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiDiskInformations(EWMIQueryTypes::WMI_DISKDRIVE_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL,wszDiskDriveQuery, 0, NULL, &eventWmiDiskInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_DISKDRIVE_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_8, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiBiosInformations(EWMIQueryTypes::WMI_BIOS_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszBiosQuery, 0, NULL, &eventWmiBiosInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_BIOS_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_9, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiNetAdapterInformations(EWMIQueryTypes::WMI_NETADAPTER_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszNetAdapQuery, 0, NULL, &eventWmiNetAdapterInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_NETADAPTER_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_10, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiNetAdapterConfInformations(EWMIQueryTypes::WMI_NETADAPTERCONF_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszNetAdapConfQuery, 0, NULL, &eventWmiNetAdapterConfInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_NETADAPTERCONF_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_11, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiOSInformations(EWMIQueryTypes::WMI_OS_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszOSQuery, 0, NULL, &eventWmiOSInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_OS_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_12, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiMemoryInformations(EWMIQueryTypes::WMI_MEMORY_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszPhysMemQuery, 0, NULL, &eventWmiMemoryInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_MEMORY_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_13, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiSoundDeviceInformations(EWMIQueryTypes::WMI_SOUNDDEVICE_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszSoundDevQuery, 0, NULL, &eventWmiSoundDeviceInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_SOUNDDEVICE_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_14, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiGPUInformations(EWMIQueryTypes::WMI_GPU_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszVideoCtrlQuery, 0, NULL, &eventWmiGPUInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_GPU_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_15, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiIDEControllerInformations(EWMIQueryTypes::WMI_IDECONTROLLER_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszIDECtrlQuery, 0, NULL, &eventWmiIDEControllerInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_IDECONTROLLER_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_16, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiCpuInformations(EWMIQueryTypes::WMI_CPU_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszProcessorQuery, 0, NULL, &eventWmiCpuInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_CPU_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_17, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiPnPInformations(EWMIQueryTypes::WMI_PNP_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszPnpEntityQuery, 0, NULL, &eventWmiPnPInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_PNP_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_18, g_winapiApiTable->GetLastError());
		return 0;
	}

	EventSink eventWmiPMediaInformations(EWMIQueryTypes::WMI_PHYSC_MEDIA_INFORMATIONS, false);
	hError = m_pWbemServices->ExecQueryAsync(wszWQL, wszPhysMediaQuery, 0, NULL, &eventWmiPMediaInformations);
	if (FAILED(hError))
	{
		DEBUG_LOG(LL_ERR, "ExecQueryAsync(%d) fail! hError: %p Last error: %u", EWMIQueryTypes::WMI_PHYSC_MEDIA_INFORMATIONS, hError, g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WMI_QUERY_FAIL_19, g_winapiApiTable->GetLastError());
		return 0;
	}


	/*
	"SELECT AdapterRAM,Description,DeviceID,DriverDate,DriverVersion,InstallDate,MaxMemorySupported,Name Win32_VideoController
	*/

	CTimer <std::chrono::milliseconds> pCheckTimer;

	MSG msg;
	while (g_winapiApiTable->GetMessageA(&msg, NULL, 0, 0)) 
	{
		g_winapiApiTable->TranslateMessage(&msg);
		g_winapiApiTable->DispatchMessageA(&msg);

		if (pCheckTimer.diff() > 5000)
		{
			if (g_nmApp->WMIInstance()->WatcherIsTerminated())
				break;

			// increase tick count
			pCheckTimer.reset();
		}
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc13()
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

bool CWMI::InitWMIWatcher()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)StartThreadRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_WMI, StartThreadRoutine, ulFuncSize, (void*)this, 5000 + 1000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}
