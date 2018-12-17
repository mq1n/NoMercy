#pragma once

class CExceptionHandlers
{
	public:
		static bool InitExceptionHandlers();
		static DWORD OnExceptionThrowed(EXCEPTION_POINTERS * pExceptionInfo);
};
