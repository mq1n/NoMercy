#pragma once

class CSelfProtection
{
	public:
		static bool InitializeAntiDump(HMODULE hModule);
		static bool IsDumpTriggered();

		static void ProtectSelfPE(HMODULE hModule);
		static void HideModuleLinks(HMODULE hModule);
};

