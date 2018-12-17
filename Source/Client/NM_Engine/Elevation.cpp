#include "NM_Index.h"
#include "NM_Main.h"
#include "Access.h"
#include "Defines.h"

BOOL CAccess::IsRunAsAdmin()
{
	BOOL fIsRunAsAdmin = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	PSID pAdministratorsGroup = NULL;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!g_winapiApiTable->AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdministratorsGroup))
	{
		dwError = g_winapiApiTable->GetLastError();
		goto Cleanup;
	}

	if (!g_winapiApiTable->CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
	{
		dwError = g_winapiApiTable->GetLastError();
		goto Cleanup;
	}

Cleanup:
	if (pAdministratorsGroup)
	{
		g_winapiApiTable->FreeSid(pAdministratorsGroup);
		pAdministratorsGroup = NULL;
	}

	if (ERROR_SUCCESS != dwError)
		throw dwError;

	return fIsRunAsAdmin;
}

BOOL CAccess::IsProcessElevated()
{
	BOOL fIsElevated = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hToken = NULL;

	if (!g_winapiApiTable->OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	TOKEN_ELEVATION elevation;
	DWORD dwSize;
	if (!g_winapiApiTable->GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
	{
		dwError = g_winapiApiTable->GetLastError();
		goto Cleanup;
	}

	fIsElevated = elevation.TokenIsElevated;

Cleanup:
	if (hToken)
	{
		g_winapiApiTable->CloseHandle(hToken);
		hToken = NULL;
	}

	if (ERROR_SUCCESS != dwError)
		throw dwError;

	return fIsElevated;
}


DWORD CAccess::GetIntegrityLevel(HANDLE hTarget)
{
	DWORD dwIntegrityLevel = 0;
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hToken = NULL;
	DWORD cbTokenIL = 0;
	PTOKEN_MANDATORY_LABEL pTokenIL = NULL;

	if (!OpenProcessToken(hTarget, TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	if (!GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &cbTokenIL))
	{
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
		{
			dwError = GetLastError();
			goto Cleanup;
		}
	}

	pTokenIL = (TOKEN_MANDATORY_LABEL *)LocalAlloc(LPTR, cbTokenIL);
	if (pTokenIL == NULL)
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	if (!GetTokenInformation(hToken, TokenIntegrityLevel, pTokenIL, cbTokenIL, &cbTokenIL))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	dwIntegrityLevel = *GetSidSubAuthority(pTokenIL->Label.Sid, 0);

Cleanup:
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	if (pTokenIL)
	{
		LocalFree(pTokenIL);
		pTokenIL = NULL;
		cbTokenIL = 0;
	}

	if (ERROR_SUCCESS != dwError)
	{
		DEBUG_LOG(LL_ERR, "GetIntegrityLevel fail! Handle: %p Error: %u", hTarget, dwError);
	}

	return dwIntegrityLevel;
}
