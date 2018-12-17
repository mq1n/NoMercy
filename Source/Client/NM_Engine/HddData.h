#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <winioctl.h>

#define IDE_ATA_IDENTIFY 0xEC
#define IDE_ATAPI_IDENTIFY 0xA1
#define DFP_GET_VERSION 0x00074080
#define DFP_RECEIVE_DRIVE_DATA 0x0007c088
#define IOCTL_SCSI_MINIPORT_IDENTIFY ((0x0000001b << 16) + 0x0501)
#define IOCTL_SCSI_MINIPORT 0x0004D008
#define SENDIDLENGTH (sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE)

class CHddData
{
public:
	CHddData();
	~CHddData();

	std::string getHDDSerialNumber()
	{
		return m_HDDSerialNumber;
	};
	std::string getHDDModelNumber()
	{
		return m_HDDModelNumber;
	};

private:
	bool ReadPhysicalDriveInNTUsingSmart();
	bool ReadPhysicalDriveInNTWithAdminRights();

	void SetDiskData(DWORD diskdata[256]);
	void SetDiskData(std::string model, std::string serial);

	void ConvertToString(DWORD diskdata[256], int firstIndex, int lastIndex, std::string& buf);
	void CleanWhitespaces(std::string& buf);

	bool DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned);

private:
	BYTE IdOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
	std::string m_HDDModelNumber;
	std::string m_HDDSerialNumber;

#pragma pack(1)
	typedef struct _IDENTIFY_DATA
	{
		USHORT GeneralConfiguration;            // 00 00
		USHORT NumberOfCylinders;               // 02  1
		USHORT Reserved1;                       // 04  2
		USHORT NumberOfHeads;                   // 06  3
		USHORT UnformattedBytesPerTrack;        // 08  4
		USHORT UnformattedBytesPerSector;       // 0A  5
		USHORT SectorsPerTrack;                 // 0C  6
		USHORT VendorUnique1[3];                // 0E  7-9
		USHORT SerialNumber[10];                // 14  10-19
		USHORT BufferType;                      // 28  20
		USHORT BufferSectorSize;                // 2A  21
		USHORT NumberOfEccBytes;                // 2C  22
		USHORT FirmwareRevision[4];             // 2E  23-26
		USHORT ModelNumber[20];                 // 36  27-46
		UCHAR  MaximumBlockTransfer;            // 5E  47
		UCHAR  VendorUnique2;                   // 5F
		USHORT DoubleWordIo;                    // 60  48
		USHORT Capabilities;                    // 62  49
		USHORT Reserved2;                       // 64  50
		UCHAR  VendorUnique3;                   // 66  51
		UCHAR  PioCycleTimingMode;              // 67
		UCHAR  VendorUnique4;                   // 68  52
		UCHAR  DmaCycleTimingMode;              // 69
		USHORT TranslationFieldsValid : 1;        // 6A  53
		USHORT Reserved3 : 15;
		USHORT NumberOfCurrentCylinders;        // 6C  54
		USHORT NumberOfCurrentHeads;            // 6E  55
		USHORT CurrentSectorsPerTrack;          // 70  56
		ULONG  CurrentSectorCapacity;           // 72  57-58
		USHORT CurrentMultiSectorSetting;       //     59
		ULONG  UserAddressableSectors;          //     60-61
		USHORT SingleWordDMASupport : 8;        //     62
		USHORT SingleWordDMAActive : 8;
		USHORT MultiWordDMASupport : 8;         //     63
		USHORT MultiWordDMAActive : 8;
		USHORT AdvancedPIOModes : 8;            //     64
		USHORT Reserved4 : 8;
		USHORT MinimumMWXferCycleTime;          //     65
		USHORT RecommendedMWXferCycleTime;      //     66
		USHORT MinimumPIOCycleTime;             //     67
		USHORT MinimumPIOCycleTimeIORDY;        //     68
		USHORT Reserved5[2];                    //     69-70
		USHORT ReleaseTimeOverlapped;           //     71
		USHORT ReleaseTimeServiceCommand;       //     72
		USHORT MajorRevision;                   //     73
		USHORT MinorRevision;                   //     74
		USHORT Reserved6[50];                   //     75-126
		USHORT SpecialFunctionsEnabled;         //     127
		USHORT Reserved7[128];                  //     128-255
	} IDENTIFY_DATA, *PIDENTIFY_DATA;
#pragma pack()

	typedef struct _SRB_IO_CONTROL
	{
		ULONG HeaderLength;
		UCHAR Signature[8 + 1];
		ULONG Timeout;
		ULONG ControlCode;
		ULONG ReturnCode;
		ULONG Length;
	} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

	typedef struct _IDSECTOR
	{
		USHORT  wGenConfig;
		USHORT  wNumCyls;
		USHORT  wReserved;
		USHORT  wNumHeads;
		USHORT  wBytesPerTrack;
		USHORT  wBytesPerSector;
		USHORT  wSectorsPerTrack;
		USHORT  wVendorUnique[3];
		CHAR    sSerialNumber[20];
		USHORT  wBufferType;
		USHORT  wBufferSize;
		USHORT  wECCSize;
		CHAR    sFirmwareRev[8];
		CHAR    sModelNumber[40];
		USHORT  wMoreVendorUnique;
		USHORT  wDoubleWordIO;
		USHORT  wCapabilities;
		USHORT  wReserved1;
		USHORT  wPIOTiming;
		USHORT  wDMATiming;
		USHORT  wBS;
		USHORT  wNumCurrentCyls;
		USHORT  wNumCurrentHeads;
		USHORT  wNumCurrentSectorsPerTrack;
		ULONG   ulCurrentSectorCapacity;
		USHORT  wMultSectorStuff;
		ULONG   ulTotalAddressableSectors;
		USHORT  wSingleWordDMA;
		USHORT  wMultiWordDMA;
		BYTE    bReserved[128];
	} IDSECTOR, *PIDSECTOR;

	typedef struct _GETVERSIONOUTPARAMS
	{
		BYTE bVersion;      // Binary driver version.
		BYTE bRevision;     // Binary driver revision.
		BYTE bReserved;     // Not used.
		BYTE bIDEDeviceMap; // Bit map of IDE devices.
		DWORD fCapabilities; // Bit mask of driver capabilities.
		DWORD dwReserved[4]; // For future use.
	} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;
};

