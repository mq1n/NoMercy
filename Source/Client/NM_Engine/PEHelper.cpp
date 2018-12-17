#include "NM_Index.h"
#include "NM_Main.h"
#include "PEHelper.h"
#include "Defines.h"

bool CPEFunctions::IsValidPEHeader(DWORD dwBaseAddress)
{
	if (!dwBaseAddress) 
		return false;

	auto pIDH = (PIMAGE_DOS_HEADER)dwBaseAddress;
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = (PIMAGE_NT_HEADERS)dwBaseAddress + pIDH->e_lfanew;
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pIOH = (PIMAGE_OPTIONAL_HEADER)&pINH->OptionalHeader;
	if (pIOH->Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC) 
		return false;

	return true;
}

PVOID CPEFunctions::GetEntryPoint(HMODULE hModule)
{
	if (!hModule)
		return nullptr;

	auto pIDH = (PIMAGE_DOS_HEADER)hModule;
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	auto pINH = (PIMAGE_NT_HEADERS)((PBYTE)hModule + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	auto pIOH = (PIMAGE_OPTIONAL_HEADER)&pINH->OptionalHeader;
	return (PBYTE)hModule + pIOH->AddressOfEntryPoint;
}

bool CPEFunctions::GetSectionInformation(const std::string & szSectionName, DWORD dwBaseAddress, LPDWORD dwOffset, LPDWORD dwLen)
{
	auto pImageDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddress;
	if (!pImageDosHeader || pImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pImageNtHeaders = (PIMAGE_NT_HEADERS)(dwBaseAddress + pImageDosHeader->e_lfanew);
	if (!pImageNtHeaders || pImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
	if (!pImageSectionHeader)
		return false;

	for (std::size_t i = 0; i < pImageNtHeaders->FileHeader.NumberOfSections; ++i)
	{
		if (!strcmp(szSectionName.c_str(), (char*)pImageSectionHeader[i].Name))
		{
			*dwOffset	= DWORD(dwBaseAddress + pImageSectionHeader[i].VirtualAddress);
			*dwLen		= pImageSectionHeader[i].SizeOfRawData;
			return true;
		}
	}
	return false;
}

PIMAGE_SECTION_HEADER CPEFunctions::GetSectionInformation(const std::string & szSectionName, DWORD dwBaseAddress)
{
	auto pImageDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddress;
	if (!pImageDosHeader || pImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	auto pImageNtHeaders = (PIMAGE_NT_HEADERS)(dwBaseAddress + pImageDosHeader->e_lfanew);
	if (!pImageNtHeaders || pImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	auto pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
	if (!pImageSectionHeader)
		return nullptr;

	for (std::size_t i = 0; i < pImageNtHeaders->FileHeader.NumberOfSections; ++i)
	{
		if (!strcmp(szSectionName.c_str(), (char*)pImageSectionHeader[i].Name))
		{
			return &pImageSectionHeader[i];
		}
	}
	return nullptr;
}

bool CPEFunctions::GetTextSectionInformation(DWORD dwBaseAddress, LPDWORD dwOffset, LPDWORD dwLen)
{
	return GetSectionInformation(xorstr(".text").crypt_get(), dwBaseAddress, dwOffset, dwLen);
}

#define RVA_TO_VA(B,O) ((PCHAR)(((PCHAR)(B)) + ((ULONG_PTR)(O))))
#define MAKE_PTR(B,O,T) (T)(RVA_TO_VA(B,O))

PVOID CPEFunctions::GetExportEntry(DWORD dwBaseAddress, const std::string & szAPIName)
{
	auto pImageDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddress;
	if (!pImageDosHeader || pImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	auto pImageNtHeaders = (PIMAGE_NT_HEADERS)(dwBaseAddress + pImageDosHeader->e_lfanew);
	if (!pImageNtHeaders || pImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	auto pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
	if (!pImageSectionHeader)
		return nullptr;

	DWORD dwExportSize;
	auto pImageExport = (PIMAGE_EXPORT_DIRECTORY)g_winapiApiTable->RtlImageDirectoryEntryToData((LPVOID)dwBaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &dwExportSize);
	if (!pImageExport || !dwExportSize)
		return nullptr;
	
	PDWORD pAddrOfNames = MAKE_PTR(dwBaseAddress, pImageExport->AddressOfNames, PDWORD);
	for (DWORD i = 0; i < pImageExport->NumberOfNames; i++)
	{
		if (!strcmp(RVA_TO_VA(dwBaseAddress, pAddrOfNames[i]), szAPIName.c_str()))
		{
			PDWORD pAddrOfFunctions = MAKE_PTR(dwBaseAddress, pImageExport->AddressOfFunctions, PDWORD);
			PWORD pAddrOfOrdinals	= MAKE_PTR(dwBaseAddress, pImageExport->AddressOfNameOrdinals, PWORD);

			return RVA_TO_VA(dwBaseAddress, pAddrOfFunctions[pAddrOfOrdinals[i]]);
		}
	}

	return nullptr;
}

DWORD CPEFunctions::CalculateChecksum(DWORD dwSectionStart, DWORD dwSectionLen)
{
	auto dwChecksumResult = 0UL;
	__try
	{ 
		dwChecksumResult = g_winapiApiTable->RtlComputeCrc32(0, (const BYTE*)dwSectionStart, dwSectionLen); 
	}
	__except (1) { }

	return dwChecksumResult;
}

DWORD CPEFunctions::GetPEHeaderSize(DWORD dwBaseAddr)
{
	auto pDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddr;
	if (!pDosHeader || pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	auto pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	if (!pNTHeader->FileHeader.SizeOfOptionalHeader)
		return 0;

	auto wSize = pNTHeader->FileHeader.SizeOfOptionalHeader;
	return wSize;
}

DWORD CPEFunctions::GetSizeofCode(DWORD dwBaseAddr)
{
	auto pDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddr;
	if (!pDosHeader || pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	auto pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	return pNTHeader->OptionalHeader.SizeOfCode;
}

DWORD CPEFunctions::OffsetToCode(DWORD dwBaseAddr)
{
	auto pDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddr;
	if (!pDosHeader || pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	auto pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	PIMAGE_OPTIONAL_HEADER pOptionalHeader = &pNTHeader->OptionalHeader;
	if (!pOptionalHeader)
		return NULL;

	return pOptionalHeader->BaseOfCode;
}

DWORD CPEFunctions::GetModuleImageSize(DWORD dwBaseAddr)
{
	auto pDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddr;
	if (!pDosHeader || pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	auto pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	return pNTHeader->OptionalHeader.SizeOfImage;
}

PIMAGE_SECTION_HEADER CPEFunctions::ImageRVA2Section(IMAGE_NT_HEADERS * pImage_NT_Headers, DWORD dwRVA)
{
	if (pImage_NT_Headers)
	{
		auto pISH = (IMAGE_SECTION_HEADER *)(((BYTE *)pImage_NT_Headers) + sizeof(IMAGE_NT_HEADERS));
		for (std::size_t i = 0; i < pImage_NT_Headers->FileHeader.NumberOfSections; i++)
		{
			if ((pISH->VirtualAddress) && (dwRVA <= (pISH->VirtualAddress + pISH->SizeOfRawData)))
			{
				return (PIMAGE_SECTION_HEADER)pISH;
			}

			pISH++;
		}
	}
	return nullptr;
}

DWORD CPEFunctions::Rva2Offset(DWORD dwModule, DWORD dwRVA)
{
	auto lpDosHdr	= (IMAGE_DOS_HEADER *)dwModule;
	auto pNtHdrs	= (IMAGE_NT_HEADERS *)(dwModule + lpDosHdr->e_lfanew);
	auto pISH		= IMAGE_FIRST_SECTION(pNtHdrs);

	for (std::size_t i = 0, sections = pNtHdrs->FileHeader.NumberOfSections; i < sections; i++, pISH++)
	{
		if (pISH->VirtualAddress <= dwRVA)
		{
			if ((pISH->VirtualAddress + pISH->Misc.VirtualSize) > dwRVA)
			{
				dwRVA -= pISH->VirtualAddress;
				dwRVA += pISH->PointerToRawData;
				return dwRVA;
			}
		}
	}
	return 0;
}

bool CPEFunctions::FillPEHeader(ULONG_PTR BaseAddress, OUT NPEHelper::PE_HEADER& PEHeader)
{
	if (!IsValidPEHeader(BaseAddress))
		return false;

	PEHeader.dosHeader = PIMAGE_DOS_HEADER(BaseAddress);
	PEHeader.ntHeaders = PIMAGE_NT_HEADERS(ULONG_PTR(PEHeader.dosHeader) + PEHeader.dosHeader->e_lfanew);
	PEHeader.fileHeader = PIMAGE_FILE_HEADER(&PEHeader.ntHeaders->FileHeader);
	PEHeader.optionalHeader = PIMAGE_OPTIONAL_HEADER(&PEHeader.ntHeaders->OptionalHeader);

	for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
		PEHeader.dataDirectory[i] = &PEHeader.ntHeaders->OptionalHeader.DataDirectory[i];

	const ULONG_PTR firstSectionHeader = ULONG_PTR(IMAGE_FIRST_SECTION(PEHeader.ntHeaders));
	for (int i = 0; i < PEHeader.fileHeader->NumberOfSections; i++)
		PEHeader.sectionHeaders.push_back(PIMAGE_SECTION_HEADER(i * sizeof(IMAGE_SECTION_HEADER) + firstSectionHeader));

	return true;
}

bool CPEFunctions::FillRemotePEHeader(HANDLE ProcessHandle, ULONG_PTR BaseAddress, OUT NPEHelper::REMOTE_PE_HEADER& PEHeader)
{
	ZeroMemory(PEHeader.rawData, PE_HEADER_SIZE);

	if (!ReadProcessMemory(ProcessHandle, PVOID(BaseAddress), PEHeader.rawData, PE_HEADER_SIZE, NULL))
		return false;

	if (!FillPEHeader(ULONG_PTR(&PEHeader.rawData), PEHeader))
		return false;

	PEHeader.remoteBaseAddress = BaseAddress;
	return true;
}

bool IsPackedImage(PUCHAR pImageBase)
{
	auto pDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pNTHeader = (PIMAGE_NT_HEADERS)(pImageBase + pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pOEP = pNTHeader->OptionalHeader.AddressOfEntryPoint;
	auto pISH = IMAGE_FIRST_SECTION(pNTHeader);
	for (std::size_t i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if (pOEP >= pISH[i].VirtualAddress && pOEP <= pISH[i].VirtualAddress + pISH[i].Misc.VirtualSize)
		{
			auto pSection = pISH[i];
			if (pSection.Characteristics & IMAGE_SCN_MEM_WRITE)
			{
				if (memcmp(pSection.Name, xorstr(".textbss").crypt_get(), 8) != 0
					&& memcmp(pSection.Name, xorstr(".text\x0\x0\x0").crypt_get(), 8) != 0
					&& memcmp(pSection.Name, xorstr("CODE\x0\x0\x0\x0").crypt_get(), 8) != 0
					&& memcmp(pSection.Name, xorstr("INIT\0\0\0\0").crypt_get(), 8) != 0)
				{
					return true;
				}
			}
			if (i > 2)
			{
				if (pNTHeader->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_NATIVE)
				{
					if (memcmp(pSection.Name, xorstr("INIT\0\0\0\0").crypt_get(), 8) == 0)
						return false;
				}
				return true;
			}
			break;
		}
	}
	return false;
}


