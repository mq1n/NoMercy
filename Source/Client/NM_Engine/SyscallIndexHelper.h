#pragma once

typedef struct _syscall_index_list
{
	DWORD dwFunc;
	DWORD dwWinXPSP0;
	DWORD dwWinXpSP1;
	DWORD dwWinXpSP2;
	DWORD dwWinXPSP3;
	DWORD dwWinVistaSP0;
	DWORD dwWinVistaSP1;
	DWORD dwWinVistaSP2;
	DWORD dwWinSevenSP0;
	DWORD dwWinSevenSP1;
	DWORD dwWinEight;
	DWORD dwWinEightPointOne;
	DWORD dwWinTenBuild1507;
	DWORD dwWinTenBuild1511;
	DWORD dwWinTenBuild1607;
	DWORD dwWinTenBuild1703;
	DWORD dwWinTenBuild1709;
	DWORD dwWinTenBuild1803;
} SSyscallIndexList, *PSyscallIndexList;

class CSyscallIndexHelper
{
	public:
		CSyscallIndexHelper();
		~CSyscallIndexHelper();

		DWORD GetSyscallId(const std::string & szFunction);
		DWORD GetSyscallId(const std::wstring & wszFunction);
		DWORD GetSyscallId(DWORD dwFunctionStrHash);

	protected:
		DWORD ParseFromNtdllMemory(DWORD dwAddress);
		bool ParseFromNtdllFile(const std::string & szAPIName);

		void BuildSyscallList();
		void DestroySyscallList();

		void RegisterSyscall(const std::string & szFunction,
			DWORD dwXPsp0idx, DWORD dwXPsp1idx, DWORD dwXPsp2idx, DWORD dwXPsp3idx,
			DWORD dwVistaSp0idx, DWORD dwVistaSp1idx, DWORD dwVistaSp2idx,
			DWORD dwSevenSp0idx, DWORD dwSevenSp1idx,
			DWORD dwEightidx, DWORD dwEightPointOneIdx,
			DWORD dwTenBuild1507Idx, DWORD dwTenBuild1511Idx, DWORD dwTenBuild1607Idx, DWORD dwTenBuild1703Idx, DWORD dwTenBuild1709Idx, DWORD dwTenBuild1803Idx);

	private:
		std::vector < std::shared_ptr <SSyscallIndexList> > m_vSyscallIndexContainer;
};
