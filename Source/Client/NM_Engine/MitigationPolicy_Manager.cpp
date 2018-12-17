#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Access.h"
#include "WinVerHelper.h"
#include "Defines.h"

bool CAccess::SetMitigationPolicys()
{
#ifdef _DEBUG
	DEBUG_LOG(LL_SYS, "Set Mitigation Policy event has been started!");

	RTL_OSVERSIONINFOEXW verInfo = { 0 };
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	if (g_winapiApiTable->RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
		DEBUG_LOG(LL_SYS, "Target OS; Major: %u Minor: %u Build: %u SP: %u.%u", verInfo.dwMajorVersion, verInfo.dwMinorVersion, verInfo.dwBuildNumber, verInfo.wServicePackMajor, verInfo.wServicePackMinor);
#endif

	if (!IsWindows8OrGreater())
		return true;

	return true;//activate later

	PROCESS_MITIGATION_DEP_POLICY depPolicy = { 0 }; // 8
	depPolicy.Enable = 1;
	depPolicy.Permanent = TRUE;
	depPolicy.DisableAtlThunkEmulation = TRUE;
	BOOL bDepPolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessDEPPolicy, &depPolicy, sizeof(depPolicy));
	if (bDepPolicyRet) {
		DEBUG_LOG(LL_SYS, "Dep Mitigation policy succesfully enabled!");
	}
	else {
		DEBUG_LOG(LL_ERR, "Dep Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
	}

	PROCESS_MITIGATION_ASLR_POLICY aslrPolicy = { 0 }; // 8
	aslrPolicy.EnableForceRelocateImages = 1;
	aslrPolicy.DisallowStrippedImages = 1;
	BOOL bAslrPolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessASLRPolicy, &aslrPolicy, sizeof(aslrPolicy));
	if (bAslrPolicyRet) {
		DEBUG_LOG(LL_SYS, "ASLR Mitigation policy succesfully enabled!");
	}
	else {
		DEBUG_LOG(LL_ERR, "ASLR Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
	}

	PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY extensionPolicy = { 0 }; // 8
	extensionPolicy.DisableExtensionPoints = 1;
	BOOL bExtensionPolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessExtensionPointDisablePolicy, &extensionPolicy, sizeof(extensionPolicy));
	if (bExtensionPolicyRet) {
		DEBUG_LOG(LL_SYS, "Extension Point Mitigation policy succesfully enabled!");
	}
	else {
		DEBUG_LOG(LL_ERR, "Extension Point Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
	}

	PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY strictHandlePolicy = { 0 }; // 8
	strictHandlePolicy.HandleExceptionsPermanentlyEnabled = 1;
	strictHandlePolicy.RaiseExceptionOnInvalidHandleReference = 1;
	BOOL bStrictHandlePolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessStrictHandleCheckPolicy, &strictHandlePolicy, sizeof(strictHandlePolicy));
	if (bStrictHandlePolicyRet) {
		DEBUG_LOG(LL_SYS, "Strict Handle Mitigation policy succesfully enabled!");
	}
	else {
		DEBUG_LOG(LL_ERR, "Strict Handle Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
	}

	if (IsWindows8Point1OrGreater())
	{
		PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY cfgPolicy = { 0 }; // 8.1
		cfgPolicy.EnableControlFlowGuard = 1;
		BOOL bCfgPolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessControlFlowGuardPolicy, &cfgPolicy, sizeof(cfgPolicy));
		if (bCfgPolicyRet) {
			DEBUG_LOG(LL_SYS, "CFG Mitigation policy succesfully enabled!");
		}
		else {
			DEBUG_LOG(LL_ERR, "CFG Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
		}

		PROCESS_MITIGATION_DYNAMIC_CODE_POLICY dynCodePolicy = { 0 };
		dynCodePolicy.AllowRemoteDowngrade = 0;
		dynCodePolicy.AllowThreadOptOut = 0;
		dynCodePolicy.ProhibitDynamicCode = 1;
		BOOL bDynCodePolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessDynamicCodePolicy, &dynCodePolicy, sizeof(dynCodePolicy));
		if (bDynCodePolicyRet) {
			DEBUG_LOG(LL_SYS, "Dynamic code Mitigation policy succesfully enabled!");
		}
		else {
			DEBUG_LOG(LL_ERR, "Dynamic code Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
		}
	}

	if (IsWindows10OrGreater()) // 10
	{
		PROCESS_MITIGATION_IMAGE_LOAD_POLICY imageLoadPolicy = { 0 };
		imageLoadPolicy.NoLowMandatoryLabelImages = 1;
		imageLoadPolicy.NoRemoteImages = 1;
		imageLoadPolicy.PreferSystem32Images = 1;
		BOOL bImageLoadPolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessImageLoadPolicy, &imageLoadPolicy, sizeof(imageLoadPolicy));
		if (bImageLoadPolicyRet) {
			DEBUG_LOG(LL_SYS, "Image Load Mitigation policy succesfully enabled!");
		}
		else {
			DEBUG_LOG(LL_ERR, "Image Load Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
		}

		PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY signaturePolicy = { 0 };
		signaturePolicy.MicrosoftSignedOnly = 1;
		BOOL bSignaturePolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessSignaturePolicy, &signaturePolicy, sizeof(signaturePolicy));
		if (bSignaturePolicyRet) {
			DEBUG_LOG(LL_SYS, "Binary Signature Mitigation policy succesfully enabled!");
		}
		else {
			DEBUG_LOG(LL_ERR, "Binary Signature Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
		}

		PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY payloadPolicy = { 0 };
		payloadPolicy.EnableExportAddressFilter = 1;
		payloadPolicy.EnableImportAddressFilter = 1;
		payloadPolicy.EnableRopStackPivot = 1;
		payloadPolicy.EnableRopCallerCheck = 1;
		BOOL bPayloadPolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessPayloadRestrictionPolicy, &payloadPolicy, sizeof(payloadPolicy));
		if (bPayloadPolicyRet) {
			DEBUG_LOG(LL_SYS, "Payload restriction Mitigation policy succesfully enabled!");
		}
		else {
			DEBUG_LOG(LL_ERR, "Payload restriction Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
		}

		PROCESS_MITIGATION_CHILD_PROCESS_POLICY childProcPolicy = { 0 };
		childProcPolicy.NoChildProcessCreation = 1;
		BOOL bChildPolicyRet = LI_FIND(SetProcessMitigationPolicy)(ProcessChildProcessPolicy, &childProcPolicy, sizeof(childProcPolicy));
		if (bChildPolicyRet) {
			DEBUG_LOG(LL_SYS, "Child process Mitigation policy succesfully enabled!");
		}
		else {
			DEBUG_LOG(LL_ERR, "Child process Mitigation policy can NOT Enabled! Last err: %u", g_winapiApiTable->GetLastError());
		}
	}
	
	DEBUG_LOG(LL_SYS, "Set Mitigation Policy event completed!");
	return true;
}

