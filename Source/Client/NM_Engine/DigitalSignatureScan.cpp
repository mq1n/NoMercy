#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "ProcessFunctions.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "ScannerInterface.h"

bool IsFileDigitalSignedRoutine(const std::wstring & wszFileName)
{
	DEBUG_LOG(LL_SYS, "Digital sign check started! Target file: %ls", wszFileName.c_str());

	bool bResult = false;
	LONG lStatus = ERROR_SUCCESS;
	DWORD dwLastError = ERROR_SUCCESS;
	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

	// Initialize the WINTRUST_FILE_INFO structure.
	WINTRUST_FILE_INFO FileData = { 0 };
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = wszFileName.c_str();
	FileData.hFile = NULL;
	FileData.pgKnownSubject = NULL;

	// Initialize the WinVerifyTrust input data structure.
	WINTRUST_DATA WinTrustData = { 0 };
	WinTrustData.cbStruct = sizeof(WinTrustData);	
	WinTrustData.pPolicyCallbackData = NULL; // Use default code signing EKU.	
	WinTrustData.pSIPClientData = NULL; // No data to pass to SIP.
	WinTrustData.dwUIChoice = WTD_UI_NONE; // Disable WVT UI.	
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; // No revocation checking.	
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE; // Verify an embedded signature on a file.	
	WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY; // Verify action.
	WinTrustData.hWVTStateData = NULL; // Verification sets this value.
	WinTrustData.pwszURLReference = NULL; // Not used.
	WinTrustData.dwUIContext = 0; // This is not applicable if there is no UI because it changes, the UI to accommodate running applications instead of installing applications.
	WinTrustData.pFile = &FileData; // Set pFile.

	// Disables file system redirection for the calling thread. Redirection cause to 0x80092003 - CRYPT_E_FILE_ERROR
	PVOID OldValue = NULL;
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &OldValue))
		return bResult;

	// WinVerifyTrust verifies signatures as specified by the GUID and Wintrust_Data.
	lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

	switch (lStatus)
	{
		case ERROR_SUCCESS:
		{
			// DEBUG_LOG(LL_SYS, "The file \"%ls\" is signed and the signature was verified.", pwszSourceFile);
			bResult = true;
		} break;

		case TRUST_E_NOSIGNATURE:
		{
			// The file was not signed or had a signature 
			// that was not valid.

			// Get the reason for no signature.
			dwLastError = GetLastError();
			if (TRUST_E_NOSIGNATURE == dwLastError || TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError || TRUST_E_PROVIDER_UNKNOWN == dwLastError)
			{
				// The file was not signed.
			//	DEBUG_LOG(LL_ERR, "The file \"%ls\" is not signed.", pwszSourceFile);
			}
			else
			{
				// The signature was not valid or there was an error 
				// opening the file.
				DEBUG_LOG(LL_ERR, "An unknown error occurred trying to verify the signature of the \"%ls\" file.", wszFileName.c_str());
			}
		} break;

		case TRUST_E_EXPLICIT_DISTRUST:
		{
			// The hash that represents the subject or the publisher 
			// is not allowed by the admin or user.
			DEBUG_LOG(LL_ERR, "The signature is present, but specifically disallowed.\n");
		} break;

		case TRUST_E_SUBJECT_NOT_TRUSTED:
		{
			// The user clicked "No" when asked to install and run.
			DEBUG_LOG(LL_ERR, "The signature is present, but not trusted.\n");
		} break;

		case CRYPT_E_SECURITY_SETTINGS:
		{
			/*
			The hash that represents the subject or the publisher
			was not explicitly trusted by the admin and the
			admin policy has disabled user trust. No signature,
			publisher or time stamp errors.
			*/
			DEBUG_LOG(LL_ERR, "CRYPT_E_SECURITY_SETTINGS - The hash representing the subject or the publisher wasn't explicitly trusted by the admin and admin policy has disabled user trust. No signature, publisher or timestamp errors.\n");
		} break;

		default:
		{
			// The UI was disabled in dwUIChoice or the admin policy 
			// has disabled user trust. lStatus contains the 
			// publisher or time stamp chain error.
			DEBUG_LOG(LL_ERR, "WinVerifyTrust fail! Error code: 0x%x.\n", lStatus);
		} break;
	}

	// Any hWVTStateData must be released by a call with close.
	WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;

	// Set created structs
	lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

	// Restores file system redirection for the calling thread.
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, OldValue, nullptr))
		return bResult;

	SetLastError(lStatus);
	return bResult;
}

inline void ShowError(DWORD dwErrorCode, bool bException)
{
	auto szErrorMessage = g_nmApp->FunctionsInstance()->DisplayError(dwErrorCode);
	DEBUG_LOG(LL_ERR, "File sign scan fail! Is exception throwed: %d Error code: %u Description: %s", bException, dwErrorCode, szErrorMessage.c_str());
}

bool IsFileDigitalSignedEx(const std::wstring & wszFileName)
{
	auto bRet			= true;
	auto bException		= false;
	auto dwErrorCode	= 0UL;

	__try
	{
		bRet = IsFileDigitalSignedRoutine(wszFileName);;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) 
	{
		bException = true;
	}
	dwErrorCode = g_winapiApiTable->GetLastError();

	if (dwErrorCode != ERROR_SUCCESS)
		::ShowError(dwErrorCode, bException);

	return bRet;
}

bool IScanner::IsFileDigitalSigned(const std::wstring & wszFileName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	return IsFileDigitalSignedEx(wszFileName);
}
