#pragma once

class NktHelper
{
	public:
		static PVOID GetModuleBaseAddress_A(const char* c_szModuleName);
		static PVOID GetModuleBaseAddress_W(const wchar_t* c_wszModuleName);

		static PVOID GetProcAddress_A(HMODULE hModule, const char* c_szProcName);
		static PVOID GetProcAddress_W(HMODULE hModule, const wchar_t* c_wszProcName);
};
