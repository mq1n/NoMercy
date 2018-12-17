#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "TempFile.h"
#include "Defines.h"
#include "DirFunctions.h"

CTempFile::~CTempFile()
{
	Destroy();
	DeleteFileA(m_szFileName);
}

CTempFile::CTempFile(const char * c_pszPrefix)
{
	strncpy(m_szFileName, g_nmApp->DirFunctionsInstance()->CreateTempFileName(c_pszPrefix).c_str(), MAX_PATH);

	if (!Create(m_szFileName, EFileMode::FILEMODE_WRITE))
	{
		DEBUG_LOG(LL_ERR, "Can not create temporary file. Filename: %s", m_szFileName);
		return;
	}
}
