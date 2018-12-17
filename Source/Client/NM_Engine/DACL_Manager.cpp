#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Access.h"
#include "WinVerHelper.h"
#include "Defines.h"

#define THREAD_POISONED_ACCESS					\
	THREAD_SUSPEND_RESUME						| \
	THREAD_SET_CONTEXT							| \
	THREAD_SET_INFORMATION						| \
	THREAD_SET_THREAD_TOKEN						| \
	THREAD_TERMINATE

#define PROCESS_POISONED_ACCESS					\
	PROCESS_CREATE_THREAD						| \
	PROCESS_VM_WRITE							| \
	PROCESS_VM_READ								| \
	PROCESS_VM_OPERATION						| \
	PROCESS_SET_INFORMATION						| \
	PROCESS_SET_LIMITED_INFORMATION				| \
	PROCESS_QUERY_INFORMATION					| \
	PROCESS_SUSPEND_RESUME						| \
	PROCESS_CREATE_PROCESS

bool CAccess::BlockAccess(HANDLE hTarget)
{
	std::string szSD =
		xorstr("D:P"
			"(D;OICI;GA;;;BG)"  /* Deny access to built-in guests */
			"(D;OICI;GA;;;AN)"  /*		  ^		 anonymous logon */
			"(D;OICI;GA;;;AU)"  /*		  ^		 authenticated users */
			"(D;OICI;GA;;;BA)"  /*		  ^		 administrators */
			"(D;OICI;GA;;;LA)"  /*		  ^		 Built-in Administrator */
		).crypt_get();

	SECURITY_ATTRIBUTES sa	= { 0 };
	sa.nLength				= sizeof(sa);
	sa.bInheritHandle		= FALSE;

	if (!g_winapiApiTable->ConvertStringSecurityDescriptorToSecurityDescriptorA(szSD.c_str(), SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL))
	{
		DEBUG_LOG(LL_ERR, "ConvertStringSecurityDescriptorToSecurityDescriptorA fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	if (!g_winapiApiTable->SetKernelObjectSecurity(hTarget, DACL_SECURITY_INFORMATION, sa.lpSecurityDescriptor))
	{
		DEBUG_LOG(LL_ERR, "SetKernelObjectSecurity fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	return true;
}

bool CAccess::DenyProcessAccess()
{
	SID pSid					= { 0 };
	pSid.Revision				= SID_REVISION;
	pSid.IdentifierAuthority	= SECURITY_WORLD_SID_AUTHORITY;
	pSid.SubAuthorityCount		= 1;
	pSid.SubAuthority[0]		= SECURITY_WORLD_RID;

	BYTE pBuffer[0x400] = { 0 };

	auto pACL			= (PACL)&pBuffer;
	pACL->AclRevision	= ACL_REVISION;
	pACL->AclSize		= sizeof(pBuffer);
	pACL->AceCount		= 0;

	auto ntStatus = g_winapiApiTable->RtlAddAccessDeniedAce(pACL, ACL_REVISION, PROCESS_POISONED_ACCESS, &pSid);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "RtlAddAccessDeniedAce fail! Error: %p", ntStatus);
		return false;
	}

	if (g_winapiApiTable->SetSecurityInfo(NtCurrentProcess, SE_KERNEL_OBJECT, PROTECTED_DACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION, 0, 0, pACL, 0) != ERROR_SUCCESS)
	{
		DEBUG_LOG(LL_ERR, "SetSecurityInfo fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	return true;
}

bool CAccess::DenyThreadAccess(HANDLE hThread)
{
	SID pSid					= { 0 };
	pSid.Revision				= SID_REVISION;
	pSid.IdentifierAuthority	= SECURITY_WORLD_SID_AUTHORITY;
	pSid.SubAuthorityCount		= 1;
	pSid.SubAuthority[0]		= SECURITY_WORLD_RID;

	BYTE pBuffer[0x400] = { 0 };

	auto pACL			= (PACL)&pBuffer;
	pACL->AclRevision	= ACL_REVISION;
	pACL->AclSize		= sizeof(pBuffer);
	pACL->AceCount		= 0;

	auto ntStatus = g_winapiApiTable->RtlAddAccessDeniedAce(pACL, ACL_REVISION, THREAD_POISONED_ACCESS, &pSid);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "RtlAddAccessDeniedAce fail! Error: %p", ntStatus);
		return false;
	}

	if (g_winapiApiTable->SetSecurityInfo(hThread, SE_KERNEL_OBJECT, PROTECTED_DACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION, 0, 0, pACL, 0) != ERROR_SUCCESS)
	{
		DEBUG_LOG(LL_ERR, "SetSecurityInfo fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	return true;
}

