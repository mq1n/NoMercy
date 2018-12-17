#pragma once
#include "ScannerInterface.h"

typedef struct _TEST_SCAN_DATA
{
	int		iIndex;
	char	szWindowName[255];
	char	szClassName[255];
} STestScanData, *PTestScanData;

typedef struct _PROCESS_BASE_SCAN_DATA // TODO: Wildcard support
{
	int			iIndex;
	PVOID64		lpBaseAddress;
	BYTE		pMem[12];
}SProcessBaseScanData, *PProcessBaseScanData;

typedef struct _PROCESS_ICON_HASH_SCAN_DATA
{
	int		iIndex;
	char	szIconHash[255];;
}SProcessIconHashScanData, *PProcessIconHashScanData;

typedef struct _FILE_BASIC_INFORMATION_SCAN_DATA
{
	int		iIndex;
	char	szShaHash[255];;
}SFileBasicInformationScanData, *PFileBasicInformationScanData;

typedef struct _FILE_INFORMATION_SCAN_DATA
{
	int			iIndex;
	DWORD		dwFileIndexLow;
	DWORD		dwFileIndexHigh;
	DWORD		dwVolumeSerialNumber;
}SFileInformationScanData, *PFileInformationScanData;

typedef struct _FILE_CERTIFICATE_SCAN_DATA
{
	int			iIndex;
	char		szPublisher[255];
	char		szHash[255];
}SFileCertificateScanData, *PFileCertificateScanData;

typedef struct _MAPPED_FILE_PE_SECTION_SCAN_DATA
{
	int			iIndex;
	DWORD		dwSize;
	DWORD		dwChecksum;
}SMappedFilePeSectionData, *PMappedFilePeSectionData;

typedef struct _MAPPED_FILE_PE_EAT_DATA
{
	int		iIndex;
	DWORD	dwExportDirectory;
	DWORD	dwImageExportDataSize;
	DWORD	dwEATChecksum;
}SMappedFilePeEatData, *PMappedFilePeEatData;

typedef struct _MAPPED_FILE_PE_IAT_DATA
{
	int		iIndex;
	DWORD	dwImportDirectory;
	DWORD	dwImageImportDataSize;
	DWORD	dwIATChecksum;
}SMappedFilePeIatData, *PMappedFilePeIatData;

typedef struct _MAPPED_FILE_PE_REGION_HASH_DATA
{
	int		iIndex;
	char	szHash[255];
}SMappedFilePeRegionHashData, *PMappedFileRegioHashData;


class CQuarentine
{
	public:
		CQuarentine();
		~CQuarentine();

		// ERR_VIOLATION_TEST_SCAN
		void CheckTestScan(const std::string & szWindowName, const std::string & szClassName);
		void AppendTestScanData(int iIndex, const std::string & szWindowName, const std::string & szClassName);

		// ERR_VIOLATION_PROCESS_BASE_SCAN
		void CheckProcessBaseMem(const std::string & szProcessName, PVOID64 lpBase, LPBYTE lpMemCopy);
		void AppendProcessBaseMemData(int iIndex, LPVOID lpBase, LPBYTE lpMemCopy);

		// ERR_VIOLATION_PROCESS_ICON_HASH_SCAN
		void CheckProcessIconHash(const std::string & szProcessName, const std::string & szIconHash);
		void AppendProcessIconHashData(int iIndex, const std::string & szIconHash);

		// ERR_VIOLATION_FILE_BASIC_INFORMATION_CHECK
		void CheckFileBasicInformations(const std::string & szFileName, DWORD dwFileLen, float fEntropy, const std::string & szShaHash, BOOL bIsSignedFile, BOOL bIsSignAnalysed);
		void AppendFileBasicInformationsData(int iIndex, const std::string & szShaHash);

		// ERR_VIOLATION_FILE_INFORMATION_CHECK
		void CheckFileInformations(const std::string & szFileName, DWORD dwFileIndexLow, DWORD dwFileIndexHigh, DWORD dwVolumeSerialNumber);
		void AppendFileInformationsData(int iIndex, DWORD dwFileIndexLow, DWORD dwFileIndexHigh, DWORD dwVolumeSerialNumber);

		// ERR_VIOLATION_FILE_CERTIFICATE_CHECK
		void CheckSignCertInformations(const std::string & szFileName, const std::string & pCertHash, LPSTR szSerialNumber, LPSTR szIssuer, LPSTR szSubject, LPSTR szFriendlyName, LPSTR szAuthority, LPSTR szPublisher);
		void AppendFileCertificateData(int iIndex, const std::string & szHash, const std::string & szPublisher);

		// ERR_VIOLATION_MAPPED_FILE_PE_CHECK
		void CheckMappedFilePEInformations(const std::string & szFileName, const std::string & szSectionName, DWORD dwSize, DWORD dwChecksum, DWORD dwCharacteristics, float fEntropy);
		void AppendMappedFilePEData(int iIndex, DWORD dwSize, DWORD dwChecksum);

		// ERR_VIOLATION_MAPPED_FILE_EAT_CHECK
		void CheckMappedFileEAT(const std::string & szFileName, DWORD dwExportDirectory, DWORD dwImageExportDataSize, DWORD dwEATChecksum);
		void AppendMappedFileEATData(int iIndex, DWORD dwExportDirectory, DWORD dwImageExportDataSize, DWORD dwEATChecksum);

		// ERR_VIOLATION_MAPPED_FILE_IAT_CHECK
		void CheckMappedFileIAT(const std::string & szFileName, DWORD dwImportDirectory, DWORD dwImageImportDataSize, DWORD dwIATChecksum);
		void AppendMappedFileIATData(int iIndex, DWORD dwImportDirectory, DWORD dwImageImportDataSize, DWORD dwIATChecksum);

		// ERR_VIOLATION_MAPPED_FILE_REGION_HASH_CHECK
		void CheckMappedFileRegionHash(const std::string & szFileName, const std::string & szMappedFileName, DWORD dwSize, const std::string & szSectionHash, DWORD dwCharacteristics);
		void AppendMappedFileRegionHashData(int iIndex, const std::string & szSectionHash);

	protected:
		void InitializeVeriables();

	private:
		mutable std::recursive_mutex m_Mutex;

		// Badlist
		std::vector < std::shared_ptr <STestScanData> >					m_vBadTestScanList;
		std::vector < std::shared_ptr <SProcessBaseScanData> >			m_vBadBaseMemList;
		std::vector < std::shared_ptr <SProcessIconHashScanData> >		m_vBadIconHashList;
		std::vector < std::shared_ptr <SFileBasicInformationScanData> >	m_vBadFileBasicInformationList;
		std::vector < std::shared_ptr <SFileInformationScanData> >		m_vBadFileInformationList;
		std::vector < std::shared_ptr <SFileCertificateScanData> >		m_vBadFileCertificateList;
		std::vector < std::shared_ptr <SMappedFilePeSectionData> >		m_vBadFilePESectionList;
		std::vector < std::shared_ptr <SMappedFilePeEatData> >			m_vBadFilePEEATList;
		std::vector < std::shared_ptr <SMappedFilePeIatData> >			m_vBadFilePEIATList;
		std::vector < std::shared_ptr <SMappedFilePeRegionHashData> >	m_vBadFilePERegionHashList;

		// Whitelist
		// ...

		// Processedlist
		// ...
};

