#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Defines.h"
#include "WinVerHelper.h"

bool ScanDNSHistoryTable(const std::string & szEntry)
{
	// TODO:
	// check dns service is it active
	// create connection and validate is it recorded
	// if table is null, throw violation
	// edirect every entry to quarentine for check validation

	if (IsWindows7OrGreater() == false)
		return false;

	auto DnsGetCacheDataTable = (lpDnsGetCacheDataTable)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hDnsapi, xorstr("DnsGetCacheDataTable").crypt_get());
	if (!DnsGetCacheDataTable)
		return false;

	PDNS_CACHE_ENTRY pEntry = nullptr;
	auto iTableStat = DnsGetCacheDataTable(pEntry);
	if (!iTableStat)
		return false;

	int iCount = 0;
	while (pEntry)
	{
		auto c_wszData = pEntry->Name;
		auto bszData = _bstr_t(c_wszData);
		auto wszData = std::wstring(bszData, SysStringLen(bszData));
		auto szData = std::string(wszData.begin(), wszData.end());

		if (strstr(szData.c_str(), szEntry.c_str()))
		{
			DEBUG_LOG(LL_ERR, "Entry found on history. Data: %s", szData.c_str());
			return true;
		}

		pEntry = pEntry->Next;
		++iCount;
	}

	if (iCount == 0) {
		DEBUG_LOG(LL_CRI, "Not found any dns history");
		return true;
	}

	return false;
}

