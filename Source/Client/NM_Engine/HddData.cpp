#include "NM_Index.h"
#include "NM_Main.h"
#include "HddData.h"

#include <functional>
#include <algorithm>

#include <sstream>

CHddData::CHddData()
{
	bool done = false;

	done = ReadPhysicalDriveInNTWithAdminRights();

	if (!done)
	{
		done = ReadPhysicalDriveInNTUsingSmart();
	}
}

CHddData::~CHddData()
{
}

bool CHddData::ReadPhysicalDriveInNTWithAdminRights()
{
	bool bRet = false;

	auto hPhysicalDriveIOCTL = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); //  Windows NT, Windows 2000, must have admin rights
	if (!IS_VALID_HANDLE(hPhysicalDriveIOCTL))
	{
		return bRet;
	}

	DWORD cbBytesReturned = 0;
	GETVERSIONOUTPARAMS VersionParams = { 0 };
	if (!DeviceIoControl(hPhysicalDriveIOCTL, DFP_GET_VERSION, NULL, 0, &VersionParams, sizeof(VersionParams), &cbBytesReturned, NULL)) // Get the version, etc of PhysicalDrive IOCTL
	{
		return bRet;
	}

	if (VersionParams.bIDEDeviceMap <= 0)
	{
		return bRet;
	}

	auto bIDCmd = (BYTE)(VersionParams.bIDEDeviceMap >> 0 & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

	memset(IdOutCmd, 0, sizeof(IdOutCmd));

	SENDCMDINPARAMS scip = { 0 };
	if (!DoIDENTIFY(hPhysicalDriveIOCTL, &scip, (PSENDCMDOUTPARAMS)&IdOutCmd, (BYTE)bIDCmd, (BYTE)0, &cbBytesReturned))
	{
		return bRet;
	}

	USHORT* pIdSector = (USHORT*)((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;

	DWORD diskdata[256];
	for (auto ijk = 0; ijk < 256; ijk++)
	{
		diskdata[ijk] = pIdSector[ijk];
	}

	SetDiskData(diskdata);

	bRet = true;

	CloseHandle(hPhysicalDriveIOCTL);

	return bRet;
}

bool CHddData::ReadPhysicalDriveInNTUsingSmart()
{
	bool bRet = false;

	auto hPhysicalDriveIOCTL = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (!IS_VALID_HANDLE(hPhysicalDriveIOCTL))
	{
		return bRet;
	}

	DWORD cbBytesReturned = 0;
	GETVERSIONINPARAMS GetVersionParams = { 0 };
	if (!DeviceIoControl(hPhysicalDriveIOCTL, SMART_GET_VERSION, NULL, 0, &GetVersionParams, sizeof(GETVERSIONINPARAMS), &cbBytesReturned, NULL))
	{
		return bRet;
	}

	ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
	PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS)malloc(CommandSize);
	Command->irDriveRegs.bCommandReg = ID_CMD;
	DWORD BytesReturned = 0;

	if (!DeviceIoControl(hPhysicalDriveIOCTL, SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS), Command, CommandSize, &BytesReturned, NULL))
	{
		return bRet;
	}

	DWORD diskdata[256];
	USHORT* pIdSector = (USHORT*)(PIDENTIFY_DATA)((PSENDCMDOUTPARAMS)Command)->bBuffer;

	for (int ijk = 0; ijk < 256; ijk++)
	{
		diskdata[ijk] = pIdSector[ijk];
	}

	SetDiskData(diskdata);
	bRet = true;

	// Done
	CloseHandle(hPhysicalDriveIOCTL);
	free(Command);

	return bRet;
}

// DoIDENTIFY FUNCTION: Send an IDENTIFY command to the drive bDriveNum = 0-3 bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
bool CHddData::DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned)
{
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE; // Set up data structures for IDENTIFY command.
	pSCIP->irDriveRegs.bFeaturesReg = 0;
	pSCIP->irDriveRegs.bSectorCountReg = 1;
	pSCIP->irDriveRegs.bCylLowReg = 0;
	pSCIP->irDriveRegs.bCylHighReg = 0;

	pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4); 	// Compute the drive number.

	pSCIP->irDriveRegs.bCommandReg = bIDCmd; // The command can either be IDE identify or ATAPI identify.
	pSCIP->bDriveNumber = bDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return (DeviceIoControl(hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA, (LPVOID)pSCIP, sizeof(SENDCMDINPARAMS) - 1, (LPVOID)pSCOP, sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1, lpcbBytesReturned, NULL) != 0);
}

void CHddData::SetDiskData(DWORD diskdata[256])
{
	std::string serialNumber;
	std::string modelNumber;

	ConvertToString(diskdata, 10, 19, serialNumber); 	//  copy the hard drive serial number to the buffer
	ConvertToString(diskdata, 27, 46, modelNumber); 	//  copy the hard drive model number to the buffer

	SetDiskData(modelNumber, serialNumber);
}

void CHddData::SetDiskData(std::string model, std::string serial)
{
	if (m_HDDSerialNumber.empty() && (isalnum(serial[0]) || isalnum(serial[19])) && !serial[0] == 0)
	{
		CleanWhitespaces(model);
		CleanWhitespaces(serial);

		m_HDDModelNumber = model;
		m_HDDSerialNumber = serial;
	}
}

void CHddData::ConvertToString(DWORD diskdata[256], int firstIndex, int lastIndex, std::string& buf)
{
	std::stringstream ss;
	for (int index = firstIndex; index <= lastIndex; index++)
	{
		ss.put((char)(diskdata[index] / 256)); //  get high byte for 1st character
		ss.put((char)(diskdata[index] % 256)); //  get low byte for 2nd character
	}
	buf = ss.str();
	CleanWhitespaces(buf);
}

void CHddData::CleanWhitespaces(std::string& buf)
{
	buf.erase(std::remove(buf.begin(), buf.end(), ' '), buf.end()); // remove whitespaces from everywhere
}

