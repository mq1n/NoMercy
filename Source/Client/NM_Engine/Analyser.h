#pragma once
#include <map>
#include <mutex>

enum EAnalyseTypes
{
	ANALYSE_NULL,
	ANALYSE_PROCESS,
	ANALYSE_PROCESS_2,
	ANALYSE_THREAD,
	ANALYSE_MODULE,
	ANALYSE_MODULE_2,
	ANALYSE_DRIVER,
	ANALYSE_SERVICE,
	ANALYSE_WINDOW,
	ANALYSE_MAX
};

enum EStartAddressCheckTypes
{
	CHECK_TYPE_THREAD,
	CHECK_TYPE_LDRLOAD,
	CHECK_TYPE_RtlGetFullPathName_U,
	CHECK_TYPE_MANIFEST_PROBER,
	CHECK_TYPE_NOTIFICATION_CALLBACK,
	CHECK_TYPE_WMI
};

class CAnalyser
{
	public:
		CAnalyser() = default;
		~CAnalyser() = default;

		void OnWMITriggered(int analyseType, std::map <std::string /* szType */, std::string /* szValue */> mDataMap, bool bProcessTerminated = false);

	public:
		bool IsCallerAddressLegit(LPVOID lpCaller, int iType, const std::string & szName, LPDWORD pdwReturnCode);

	public:
		// Hook callbacks
		bool OnThreadCreated(DWORD dwThreadID, LPVOID lpStartAddress, LPVOID lpEIP, LPDWORD pdwErrorType);
		bool OnModuleLoaded(const std::string & szName, DWORD dwCurrentTID, int iCheckType, LPDWORD pdwErrorType);
		bool OnModuleRequested(const std::string & szName, LPDWORD pdwErrorType);
		bool OnSectionMapped(DWORD dwBase, LPVOID lpArbitraryUserPointer, LPDWORD pdwErrorType);
		bool OnExceptionThrowed(PEXCEPTION_RECORD ExceptionInfo, LPDWORD pdwErrorType);
		bool OnConnected(const std::string & szTargetAddress, LPDWORD pdwErrorType);
		bool OnWndProcHooked(HWND hWnd, int nIndex, LONG dwNewLong, LPDWORD pdwErrorType);

	private:
		mutable std::recursive_mutex	m_mutex;
};

