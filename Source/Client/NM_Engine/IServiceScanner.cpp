#include "NM_Main.h"
#include "NM_Index.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"

#if 0
* Display name
* Service name
* Forward file based scan
#endif

bool IScanner::ScanServices()
{
	return true;
}
void IScanner::OnScanService(const std::string & szServiceName, const std::string & szServiceDisplayName, DWORD dwServiceStatus)
{

}
bool IScanner::IsScannedService(const std::string & szServiceName)
{
	return false;
}
