#include "NM_Main.h"
#include "NM_Index.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"

// https://processhacker.sourceforge.io/doc/struct___s_y_s_t_e_m___s_i_n_g_l_e___m_o_d_u_l_e___i_n_f_o_r_m_a_t_i_o_n.html

#if 0
* First module(ntoskrnl) integrity
* Forward file based scan
* Digital signature informations
* Map to memory and forward section based scan
* File name check
* Hidden file check
#endif


bool IScanner::ScanDrivers()
{
	return true;
}

void IScanner::OnScanDriver(const std::string & szDriverPathName)
{
}

bool IScanner::IsScannedDriver(const std::string & szDriverPathName)
{
	return false;
}
