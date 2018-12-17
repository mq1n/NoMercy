#pragma once

class CAntiDebug
{
	public:
		static bool SeDebugPrivTriggered();

		static bool InitAntiDebug(LPDWORD pdwErrorStep);
		static bool CheckRuntimeAntiDebug(LPDWORD pdwDetectType);
		static bool ParentCheck(const char* c_szPatcherName);
		static bool IsImageSumCorrupted(DWORD dwBaseImage, DWORD dwCorrectSum);

		static bool CheckCPUId();
		static bool HypervisorCheckTriggered();
		static bool CheckUdpPorts();

		static void AntiCuckoo();

		static bool AntiVirtualize(LPDWORD pdwReturnCode);
};

