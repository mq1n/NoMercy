#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include <WinCrypt.h>

// https://github.com/Microsoft/winfile/blob/master/tools/FileSignatureInfo/FileSignatureInfo.cpp

bool PrintCertificateInfo(PCCERT_CONTEXT pCertContext)
{
	bool fReturn = true;

	auto dwData = CertGetNameStringW(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, NULL, 0);
	if (!dwData)
		return fReturn;

	auto szName = (LPWSTR)malloc(dwData * sizeof(LPWSTR) + 1);
	if (!szName)
		return fReturn;

	if (!CertGetNameStringW(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, szName, dwData)) 
	{
		free(szName);
		return fReturn;
	}

	DEBUG_LOG(LL_SYS, "Cert name: %ls", szName);

	free(szName);
	return fReturn;
}

bool isValidFileCertificateEx(const std::wstring & wszFileName)
{
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult = TRUE;
	DWORD dwEncoding = 0, dwContentType = 0, dwFormatType = 0;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	DWORD dwSignerInfo = 0;
	CERT_INFO CertInfo;
	bool bRet = false;

	__try
	{
		fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE, wszFileName.c_str(), CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY, 0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, NULL);
		if (!fResult) __leave;

		fResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo);
		if (!fResult)
			__leave;

		pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
		if (!pSignerInfo)
			__leave;

		fResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo);
		if (!fResult)
			__leave;

		CertInfo.Issuer = pSignerInfo->Issuer;
		CertInfo.SerialNumber = pSignerInfo->SerialNumber;

		pCertContext = CertFindCertificateInStore(hStore, (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING), 0, CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, NULL);
		if (!pCertContext)
			__leave;

		bRet = PrintCertificateInfo(pCertContext);
	}
	__finally
	{
		if (pSignerInfo != NULL) LocalFree(pSignerInfo);
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
		if (hStore != NULL) CertCloseStore(hStore, 0);
		if (hMsg != NULL) CryptMsgClose(hMsg);
	}

	return bRet;
}

bool IScanner::isValidFileCertificate(const std::wstring & wszFileName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	// Disables file system redirection for the calling thread. Redirection cause to 0x80092003 - CRYPT_E_FILE_ERROR
	PVOID OldValue = NULL;
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &OldValue))
		return true;

	auto bRet = isValidFileCertificateEx(wszFileName);

	// Restore file system redirection for the calling thread.
	if (!g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, OldValue, nullptr))
		return bRet;

	return bRet;
}
