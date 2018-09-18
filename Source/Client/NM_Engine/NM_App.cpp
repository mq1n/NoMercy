#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Defines.h"

#include <lazy_importer.hpp>

#pragma optimize("", off)
CNM_App::CNM_App()
{
}

// Routines
bool CNM_App::InitializeServer(LPDWORD pDwErrorCode)
{
	return true;
}

bool CNM_App::InitializeClient(LPDWORD pDwErrorCode)
{
	return true;
}

bool CNM_App::InitializeLauncher(bool bProtected, LPDWORD pDwErrorCode)
{
	return true;
}

bool CNM_App::InitializeService(bool bProtected, LPDWORD pDwErrorCode)
{
	return true;
}

bool CNM_App::InitializeHelperApp(LPDWORD pDwErrorCode)
{
	return true;
}


bool CNM_App::InitializeEx(bool bClient, bool bProtected, LPDWORD pDwErrorCode)
{
	return true;
}

bool CNM_App::InitializeTestMode(bool bClient, LPDWORD pDwErrorCode)
{
	return true;
}

bool CNM_App::Initialize(bool bClient, bool bProtected, LPDWORD pDwErrorCode)
{
	return true;
}

// Finalization
bool CNM_App::Finalize(LPDWORD pDwErrorCode)
{
	return true;
}

#pragma optimize("", on)

