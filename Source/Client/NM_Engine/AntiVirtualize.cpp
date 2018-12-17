#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "AntiDebug.h"
#include "DynamicWinapi.h"
#include "Defines.h"

#ifndef _M_X64

// IsInsideVPC's exception filter
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
	PCONTEXT ctx = ep->ContextRecord;

	ctx->Ebx = -1; // Not running VPC
	ctx->Eip += 4; // skip past the "call VPC" opcodes
	return EXCEPTION_CONTINUE_EXECUTION;
	// we can safely resume execution since we skipped faulty instruction
}
#endif

// High level language friendly version of IsInsideVPC()
inline bool IsInsideVPC()
{
#ifndef _M_X64
	bool rc = false;
	__try
	{
		_asm push ebx
		_asm mov  ebx, 0 // It will stay ZERO if VPC is running
		_asm mov  eax, 1 // VPC function number

						 // call VPC
		_asm __emit 0Fh
		_asm __emit 3Fh
		_asm __emit 07h
		_asm __emit 0Bh

		_asm test ebx, ebx
		_asm setz[rc]
			_asm pop ebx
	}
	// The except block shouldn't get triggered if VPC is running!!
	__except (IsInsideVPC_exceptionFilter(GetExceptionInformation()))
	{
	}
	return rc;
#else
	return true;
#endif
}

inline bool IsInsideVMWare()
{
#ifndef _M_X64
	bool rc = true;
	__try
	{
		__asm
		{
			push   edx
			push   ecx
			push   ebx

			mov    eax, 'VMXh'
			mov    ebx, 0 // any value but not the MAGIC VALUE
			mov    ecx, 10 // get VMWare version
			mov    edx, 'VX' // port number

			in     eax, dx // read port
						   // on return EAX returns the VERSION
						   cmp    ebx, 'VMXh' // is it a reply from VMWare?
						   setz[rc] // set return value

						   pop    ebx
						   pop    ecx
						   pop    edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		rc = false;
	}
	return rc;
#else
	return true;
#endif
}


inline bool AntiVPC()
{
	DEBUG_LOG(LL_SYS, "Anti virtual pc check has been started!");

	if (IsInsideVPC() || g_winapiApiTable->GetModuleHandleA_o(xorstr("vmcheck.dll").crypt_get()))
	{
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti virtual pc check completed!");
	return true;
}

inline bool AntiVMware()
{
	DEBUG_LOG(LL_SYS, "Anti vmware check has been started!");

	if (IsInsideVMWare()) 
	{
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti vmware check completed!");
	return true;
}

inline bool AntiSandBoxie()
{
	DEBUG_LOG(LL_SYS, "Anti sandboxie check has been started!");

	if (g_winapiApiTable->GetModuleHandleA_o(xorstr("SbieDll.dll").crypt_get()) || g_winapiApiTable->GetModuleHandleA_o(xorstr("SbieDllX.dll").crypt_get()))
	{
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti sandboxie check completed!");
	return true;
}

inline bool AntiVirtualMachine()
{
	DEBUG_LOG(LL_SYS, "Anti virtual machine check has been started!");
#ifndef _M_X64
	unsigned int reax = 0;
	__asm
	{
		mov eax, 0xCCCCCCCC;
		smsw eax;
		mov DWORD PTR[reax], eax;
	}

	if ((((reax >> 24) & 0xFF) == 0xcc) && (((reax >> 16) & 0xFF) == 0xcc)) 
	{
		return false;
	}
#endif

	DEBUG_LOG(LL_SYS, "Anti virtual machine check completed!");
	return true;
}

inline bool AntiVirtualBox()
{
	DEBUG_LOG(LL_SYS, "Anti virtual box check has been started!");

	unsigned long pnsize = 0x1000;
	char* provider = (char*)g_winapiApiTable->LocalAlloc(LMEM_ZEROINIT, pnsize);

	int retv = g_winapiApiTable->WNetGetProviderNameA(WNNC_NET_RDR2SAMPLE, provider, &pnsize);
	if (retv == NO_ERROR)
	{
		if (g_winapiApiTable->lstrcmpA(provider, xorstr("VirtualBox Shared Folders!").crypt_get()) == 0)
		{
			return false;
		}
	}
	if (provider)
		g_winapiApiTable->LocalFree(provider);

	/*
	//todo: check with createfile != invalid_Handle
	string devices[4] = { "\\\\.\\VBoxMiniRdrDN", "\\\\.\\pipe\\VBoxMiniRdDN", 
		"\\\\.\\VBoxTrayIPC", "\\\\.\\pipe\\VBoxTrayIPC" };

		if (createFile("\\\\.\\pipe\\VBoxTrayIPC") != INVALID_HANDLE_VALUE)

	*/

	DEBUG_LOG(LL_SYS, "Anti virtual box check completed!");
	return true;
}

inline bool AntiSunbeltSandBox()
{
	DEBUG_LOG(LL_SYS, "Anti sunbelt sandbox check has been started!");

	CHAR szFileName[MAX_PATH];
	g_winapiApiTable->GetModuleFileNameA(NULL, szFileName, MAX_PATH);

	if (!strcmp(szFileName, xorstr("C:\\file.exe").crypt_get()) || g_winapiApiTable->GetModuleHandleA_o(xorstr("pstorec.dll").crypt_get()))
	{
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti sunbelt sandbox check completed!");
	return true;
}

inline bool AntiWPEPro()
{
	DEBUG_LOG(LL_SYS, "Anti wpe check has been started!");

	if (g_winapiApiTable->GetModuleHandleA_o(xorstr("wpespy.dll").crypt_get()))
	{
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti wpe check completed!");
	return true;
}

inline bool AntiWine()
{
	DEBUG_LOG(LL_SYS, "Anti wine check has been started!");

	if (g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32, xorstr("wine_get_unix_file_name").crypt_get()))
	{
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti wine check completed!");
	return true;
}

inline bool Anticuckoomon()
{
	DEBUG_LOG(LL_SYS, "Anti cuckoomon check has been started!");

	if (g_winapiApiTable->GetModuleHandleA_o(xorstr("cuckoomon.dll").crypt_get()))
	{
		return false;
	}

	if (g_winapiApiTable->CreateFileA(xorstr("\\\\.\\pipe\\cuckoo").crypt_get(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0) != INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DEBUG_LOG(LL_SYS,  "Anti cuckoomon check completed!");
	return true;
}

inline bool AntiSandbox()
{
	DEBUG_LOG(LL_SYS, "Anti sandbox check has been started!");

	// serviste rip
#if 0
	auto hwSandbox = g_winapiApiTable->FindWindowExA(0, 0, xorstr("Progman").crypt_get(), xorstr("Program Manager").crypt_get());
	if (!hwSandbox) 
	{
		return false;
	}
#endif

	// Normal sandbox uygulamasý pcde aktifse
#if 0
	auto hwSandbox2 = g_winapiApiTable->FindWindowExA(0, 0, xorstr("SandboxieControlWndClass"), 0);
	if (hwSandbox2) 
	{
		return false;
	}
#endif

	DEBUG_LOG(LL_SYS, "Anti sandbox check completed!");
	return true;
}

inline bool CheckRegistry_DiskEnum(LPDWORD pdwReturnCode)
{
	DEBUG_LOG(LL_SYS, "Registry Disk Enum check has been started!");

	char RegKey[_MAX_PATH] = { 0 };
	DWORD BufSize = _MAX_PATH;
	DWORD dataType = REG_SZ;

	HKEY hKey;
	long lError = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum").crypt_get(), NULL, KEY_QUERY_VALUE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		long lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("0").crypt_get(), NULL, &dataType, (LPBYTE)&RegKey, &BufSize);
		if (lVal == ERROR_SUCCESS)
		{
			std::string szRegKey = RegKey;
			if (strstr(g_nmApp->FunctionsInstance()->szLower(szRegKey).c_str(), xorstr("vmware").crypt_get()))
			{
				if (pdwReturnCode) *pdwReturnCode = 1;
				return false;
			}
			if (strstr(g_nmApp->FunctionsInstance()->szLower(szRegKey).c_str(), xorstr("virtual").crypt_get()))
			{
				if (pdwReturnCode) *pdwReturnCode = 2;
				return false;
			}
			if (strstr(g_nmApp->FunctionsInstance()->szLower(szRegKey).c_str(), xorstr("vbox").crypt_get()))
			{
				if (pdwReturnCode) *pdwReturnCode = 3;
				return false;
			}
			if (strstr(g_nmApp->FunctionsInstance()->szLower(szRegKey).c_str(), xorstr("qemu").crypt_get()))
			{
				if (pdwReturnCode) *pdwReturnCode = 4;
				return false;
			}
			if (strstr(g_nmApp->FunctionsInstance()->szLower(szRegKey).c_str(), xorstr("xen").crypt_get()))
			{
				if (pdwReturnCode) *pdwReturnCode = 5;
				return false;
			}
		}
		g_winapiApiTable->RegCloseKey(hKey);
	}

	DEBUG_LOG(LL_SYS, "Registry Disk Enum check completed!");
	return true;
}

inline bool CheckRdtsc()
{
#ifndef _M_X64
	unsigned int time1 = 0;
	unsigned int time2 = 0;
	__asm
	{
		RDTSC
		MOV time1, EAX
		RDTSC
		MOV time2, EAX
	}

	if ((time2 - time1) > 200)
	{
		return false;
	}
#endif
	return true;
}

#define PUserKData        ((LPBYTE)0x00005800)
#define CURTLSPTR_OFFSET 0x000
#define UTlsPtr() (*(LPDWORD *)(PUserKData+CURTLSPTR_OFFSET))

#define TLSSLOT_MSGQUEUE    0
#define TLSSLOT_RUNTIME     1
#define TLSSLOT_KERNEL      2

#define TLSKERN_NOFAULT         0x00000002
#define TLSKERN_NOFAULTMSG      0x00000010

bool IsRunningOnVirtualMachineEx()
{
#ifndef _M_X64
	BOOL fRetVal = TRUE;
	DWORD OldPtr = UTlsPtr()[TLSSLOT_KERNEL];
	UTlsPtr()[TLSSLOT_KERNEL] |= TLSKERN_NOFAULT | TLSKERN_NOFAULTMSG;

	__try
	{
		__asm
		{
			// Execute a synthetic VMCPUID instruction.
			__emit  0x0F
			__emit  0xC7
			__emit  0xC8
			__emit  0x01
			__emit  0x00
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		// this should be EXCEPTION_ILLEGAL_INSTRUCTION
		fRetVal = FALSE;
	}

	UTlsPtr()[TLSSLOT_KERNEL] = OldPtr;
	return fRetVal;
#else
	return true;
#endif
}

bool IsRunningOnVirtualMachine()
{
	if (IsRunningOnVirtualMachineEx())
	{
		DEBUG_LOG(LL_ERR, "IsRunningOnVirtualMachine detected!");
		return true;
	}
	return false;
}

void CrashMachine()
{
	__try
	{
		__sidt((void*)(NULL));
	}
	__except (1)
	{
	}
}

bool CAntiDebug::AntiVirtualize(LPDWORD pdwReturnCode)
{
	DEBUG_LOG(LL_SYS, "Anti virtualize event has been started!");

	auto pdwDiskRet = 0UL;

//	AntiVPC();
//	AntiVMware();
//	IsRunningOnVirtualMachine();

	if (AntiSandBoxie() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 1;
		return false;
	}
	if (AntiSandbox() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 2;
		return false;
	}

	if (AntiVirtualMachine() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 3;
		return false;
	}
	if (AntiVirtualBox() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 4;
		return false;
	}
	if (AntiSunbeltSandBox() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 5;
		return false;
	}
	if (AntiWPEPro() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 6;
		return false;
	}
	if (AntiWine() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 7;
		return false;
	}
	if (Anticuckoomon() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 8;
		return false;
	}
	if (CheckRdtsc() == false)
	{
		if (pdwReturnCode) *pdwReturnCode = 9;
		return false;
	}
	if (CheckRegistry_DiskEnum(&pdwDiskRet) == false)
	{
		if (pdwReturnCode) *pdwReturnCode = pdwDiskRet;
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti virtualize event completed!");
	return true;
}

