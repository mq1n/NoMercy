#pragma once

class CAntiBreakpoint
{
	public:
		static bool HasHardwareBreakpoint();
		static bool HasEntrypointBreakpoint();
		static bool HasMemoryBreakpoint();
		static bool InitAntiSoftBP();
};

