#pragma once

#define PE_HEADER_SIZE          0x1000

namespace NPEHelper
{
	struct PE_HEADER
	{
		PIMAGE_DOS_HEADER dosHeader;
		PIMAGE_NT_HEADERS ntHeaders;
		PIMAGE_FILE_HEADER fileHeader;
		PIMAGE_OPTIONAL_HEADER optionalHeader;
		std::array<PIMAGE_DATA_DIRECTORY, IMAGE_NUMBEROF_DIRECTORY_ENTRIES> dataDirectory;
		std::vector<PIMAGE_SECTION_HEADER> sectionHeaders;
	};

	struct REMOTE_PE_HEADER : PE_HEADER
	{
		ULONG_PTR remoteBaseAddress;
		BYTE rawData[PE_HEADER_SIZE];
	};
}

class CPEFunctions
{
	public:
		static bool IsValidPEHeader(DWORD BaseAddress);
		static PVOID GetEntryPoint(HMODULE hModule);

		static bool GetSectionInformation(const std::string & szSectionName, DWORD dwBaseAddress, LPDWORD dwOffset, LPDWORD dwLen);
		static PIMAGE_SECTION_HEADER GetSectionInformation(const std::string & szSectionName, DWORD dwBaseAddress);
		static bool GetTextSectionInformation(DWORD dwBaseAddress, LPDWORD dwOffset, LPDWORD dwLen);

		static PVOID GetExportEntry(DWORD dwBaseAddress, const std::string & szAPIName);
		static DWORD CalculateChecksum(DWORD dwSectionStart, DWORD dwSectionLen);
		static DWORD GetPEHeaderSize(DWORD dwBaseAddr);
		static DWORD GetSizeofCode(DWORD dwBaseAddr);
		static DWORD OffsetToCode(DWORD dwBaseAddr);
		static DWORD GetModuleImageSize(DWORD dwBaseAddr);

		static PIMAGE_SECTION_HEADER ImageRVA2Section(IMAGE_NT_HEADERS * pImage_NT_Headers, DWORD dwRVA);
		static DWORD Rva2Offset(DWORD dwModule, DWORD dwRVA);

		static bool FillPEHeader(ULONG_PTR BaseAddress, OUT NPEHelper::PE_HEADER& PEHeader);
		static bool FillRemotePEHeader(HANDLE ProcessHandle, ULONG_PTR BaseAddress, OUT NPEHelper::REMOTE_PE_HEADER& PEHeader);

		static bool IsPackedImage(PUCHAR pImageBase);
};

