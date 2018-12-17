#pragma once
#include "Analyser.h"
#include <WbemCli.h>
#include <WbemIdl.h>

enum EWMIQueryTypes
{
	WMI_BASEBOARD_INFORMATIONS=100,
	WMI_DISKDRIVE_INFORMATIONS,
	WMI_BIOS_INFORMATIONS,
	WMI_NETADAPTER_INFORMATIONS,
	WMI_NETADAPTERCONF_INFORMATIONS,
	WMI_OS_INFORMATIONS,
	WMI_MEMORY_INFORMATIONS,
	WMI_SOUNDDEVICE_INFORMATIONS,
	WMI_GPU_INFORMATIONS,
	WMI_CPU_INFORMATIONS,
	WMI_IDECONTROLLER_INFORMATIONS,
	WMI_PNP_INFORMATIONS,
	WMI_PHYSC_MEDIA_INFORMATIONS,
};

extern bool DumpWMIClassObject(int iAnalyseType, IWbemClassObject * pClassObject, bool bProcessCreated, int iIndentationLevel = 0);

class EventSink : public IWbemEventSink
{
	private:
		LONG			m_lRef;
		bool			m_processCreated;
		int				m_analyseType;

	public:
		EventSink(int iAnalyseType, bool processCreated) :
			m_analyseType(iAnalyseType), m_processCreated(processCreated)
		{
		}

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) 
		{
			if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) 
			{
				*ppvObject = (IWbemEventSink*)this;
				AddRef();
				return WBEM_S_NO_ERROR;
			}
			else return E_NOINTERFACE;
		}

		virtual ULONG STDMETHODCALLTYPE AddRef(void) 
		{
			return InterlockedIncrement(&m_lRef);
		}

		virtual ULONG STDMETHODCALLTYPE Release(void)
		{
			LONG lRef = InterlockedDecrement(&m_lRef);
			if (lRef == 0)
				delete this;

			return lRef;
		}

		virtual HRESULT STDMETHODCALLTYPE Indicate(long lObjectCount, __RPC__in_ecount_full(lObjectCount) IWbemClassObject **apObjArray) 
		{
			for (long i = 0; i < lObjectCount; i++)
			{
				auto pObj = apObjArray[i];
				DumpWMIClassObject(m_analyseType, pObj, m_processCreated);
			}
			return WBEM_S_NO_ERROR;
		};

		virtual HRESULT STDMETHODCALLTYPE SetStatus(long lFlags, HRESULT hResult, __RPC__in_opt BSTR strParam, __RPC__in_opt IWbemClassObject *pObjParam) 
		{
			return WBEM_S_NO_ERROR;
		};

		virtual HRESULT STDMETHODCALLTYPE SetSinkSecurity(long lSDLength, __RPC__in_ecount_full(lSDLength) BYTE *pSD) 
		{
			return WBEM_S_NO_ERROR;
		}

		virtual HRESULT STDMETHODCALLTYPE IsActive(void) 
		{
			return WBEM_S_NO_ERROR;
		}

		virtual HRESULT STDMETHODCALLTYPE GetRestrictedSink(long lNumQueries, __RPC__in_ecount_full(lNumQueries) const LPCWSTR *awszQueries, __RPC__in_opt IUnknown *pCallback, __RPC__deref_out_opt IWbemEventSink **ppSink) 
		{
			return WBEM_S_NO_ERROR;
		}

		virtual HRESULT STDMETHODCALLTYPE SetBatchingParameters(LONG lFlags, DWORD dwMaxBufferSize, DWORD dwMaxSendLatency)
		{
			return WBEM_S_NO_ERROR;
		}
};

class CWMI
{
	public:
		CWMI();
		~CWMI() = default;

		bool	InitWMIWatcher();
		void	TerminateWatcher();

		bool	WatcherIsTerminated();

		bool	AnalyseModules(DWORD dwTargetProcessId);

	protected:
		DWORD					ThreadRoutine(void);
		static DWORD WINAPI		StartThreadRoutine(LPVOID lpParam);

	private:
		bool			m_bWatcherIsTerminated;
		IWbemServices*	m_pWbemServices;
};

