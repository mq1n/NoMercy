#pragma once
#include "DynamicWinapi.h"

class CMemoryHelper
{
	public:
		CMemoryHelper(HANDLE hProcess);
		~CMemoryHelper(void) = default;

		LPVOID	Alloc(std::size_t size, DWORD dwProtection = PAGE_EXECUTE_READWRITE, LPVOID pPreferredLoc = nullptr);
		LPVOID	Commit(LPVOID data, std::size_t size);
		bool	Protect(LPVOID address, std::size_t size, DWORD dwProtectFlag);
		bool	Free(LPVOID address, std::size_t size);

		bool	ReadAddress(LPVOID address, LPVOID buffer, std::size_t size);
		bool	ReadAddressSafe(LPVOID address, LPVOID buffer, std::size_t size);
		bool	WriteAddress(LPVOID address, LPVOID buffer, std::size_t size);
		bool	WriteAddressSafe(LPVOID address, LPVOID buffer, std::size_t size);

		bool	Patch(LPVOID address, LPVOID buffer, std::size_t size);

		bool	IsSafePage(LPVOID address);

	public:
		template <typename T>
		T Read(const DWORD_PTR Address)
		{
			T ret;
			g_winapiApiTable->ReadProcessMemory(m_hProcess, (LPCVOID)Address, &ret, sizeof(T), nullptr);
			return ret;
		}
		template <typename T>
		T* ReadStruct(const DWORD_PTR Address)
		{
			T ret;
			if (!g_winapiApiTable->ReadProcessMemory(m_hProcess, (LPCVOID)Address, &ret, sizeof(T), nullptr))
				return nullptr;
			return &ret;
		}
		template <typename T>
		bool ReadStruct(const DWORD_PTR Address, T& data)
		{
			return !!g_winapiApiTable->ReadProcessMemory(m_hProcess, (LPCVOID)Address, &data, sizeof(T), nullptr);
		}

		template <class cData>
		bool Write(const DWORD_PTR dwAddress, cData Value)
		{
			SIZE_T dwWrittenByteCount;
			auto bWriteRet = g_winapiApiTable->WriteProcessMemory(m_hProcess, (LPVOID)dwAddress, &Value, sizeof(cData), &dwWrittenByteCount);
			return bWriteRet;
		}

	private:
		HANDLE m_hProcess;
};

