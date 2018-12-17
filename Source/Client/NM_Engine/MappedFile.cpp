#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "MappedFile.h"
#include "Defines.h"

CMappedFile::CMappedFile() :
	m_hFM(NULL), m_lpMapData(NULL), m_dataOffset(0), m_mapSize(0), m_seekPosition(0), m_pbBufLinkData(NULL), m_dwBufLinkSize(0), m_pbAppendResultDataBlock(NULL), m_dwAppendResultDataSize(0)
{
}

CMappedFile::~CMappedFile()
{
	Destroy();
}

BOOL CMappedFile::Create(const char * filename)
{
	Destroy();
	return CFileBase::Create(filename, FILEMODE_READ);
}

BOOL CMappedFile::Create(const char * filename, void** dest, int offset, int size)
{
	if (!CMappedFile::Create(filename))
		return NULL;

	int ret = Map(dest, offset, size);
	return (ret) > 0;
}

LPCVOID CMappedFile::Get()
{
	return m_lpData;
}

void CMappedFile::Link(DWORD dwBufSize, const void* c_pvBufData)
{
	m_dwBufLinkSize = dwBufSize;
	m_pbBufLinkData = (BYTE*)c_pvBufData;
}

BYTE* CMappedFile::AppendDataBlock(const void* pBlock, DWORD dwBlockSize)
{
	if (m_pbAppendResultDataBlock)
	{
		delete[] m_pbAppendResultDataBlock;
	}

	//realloc
	m_dwAppendResultDataSize = m_dwBufLinkSize + dwBlockSize;
	m_pbAppendResultDataBlock = new BYTE[m_dwAppendResultDataSize];

	memcpy(m_pbAppendResultDataBlock, m_pbBufLinkData, m_dwBufLinkSize);
	memcpy(m_pbAppendResultDataBlock + m_dwBufLinkSize, pBlock, dwBlockSize);

	//redirect
	Link(m_dwAppendResultDataSize, m_pbAppendResultDataBlock);

	return m_pbAppendResultDataBlock;
}

void CMappedFile::Destroy()
{
	if (NULL != m_lpMapData)
	{
		Unmap(m_lpMapData);
		m_lpMapData = NULL;
	}

	if (NULL != m_hFM)
	{
		CloseHandle(m_hFM);
		m_hFM = NULL;
	}

	if (m_pbAppendResultDataBlock)
	{
		delete[] m_pbAppendResultDataBlock;
		m_pbAppendResultDataBlock = NULL;
	}

	m_dwAppendResultDataSize = 0;

	m_pbBufLinkData = NULL;
	m_dwBufLinkSize = 0;

	m_seekPosition = 0;
	m_dataOffset = 0;
	m_mapSize = 0;

	CFileBase::Destroy();
}

int CMappedFile::Seek(DWORD offset, int iSeekType)
{
	switch (iSeekType)
	{
	case SEEK_TYPE_BEGIN:
		if (offset > m_dwSize)
			offset = m_dwSize;

		m_seekPosition = offset;
		break;

	case SEEK_TYPE_CURRENT:
		m_seekPosition = std::min(m_seekPosition + offset, Size());
		break;

	case SEEK_TYPE_END:
		m_seekPosition = std::max(0UL, Size() - offset);
		break;
	}

	return m_seekPosition;
}

int CMappedFile::Map(void **dest, int offset, int size)
{
	m_dataOffset = offset;

	if (size == 0)
		m_mapSize = m_dwSize;
	else
		m_mapSize = size;

	if (m_dataOffset + m_mapSize > m_dwSize)
		return NULL;

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwSysGran = SysInfo.dwAllocationGranularity;
	DWORD dwFileMapStart = (m_dataOffset / dwSysGran) * dwSysGran;
	DWORD dwMapViewSize = (m_dataOffset % dwSysGran) + m_mapSize;
	INT iViewDelta = m_dataOffset - dwFileMapStart;


	m_hFM = CreateFileMappingA(m_hFile, NULL, PAGE_READONLY, 0, m_dataOffset + m_mapSize, NULL);
	if (!m_hFM)
	{
		DEBUG_LOG(LL_ERR, "CMappedFile::Map !m_hFM\n");
		return NULL;
	}

	m_lpMapData = MapViewOfFile(m_hFM, FILE_MAP_READ, 0, dwFileMapStart, dwMapViewSize);
	if (!m_lpMapData)
	{
		DEBUG_LOG(LL_ERR, "CMappedFile::Map !m_lpMapData %lu", GetLastError());
		return 0;
	}

	m_lpData = (char*)m_lpMapData + iViewDelta;
	*dest = (char*)m_lpData;
	m_seekPosition = 0;

	Link(m_mapSize, m_lpData);

	return (m_mapSize);
}

BYTE * CMappedFile::GetCurrentSeekPoint()
{
	return m_pbBufLinkData + m_seekPosition;
}


DWORD CMappedFile::Size()
{
	return m_dwBufLinkSize;
}

DWORD CMappedFile::GetPosition()
{
	return m_dataOffset;
}

BOOL CMappedFile::Read(void * dest, int bytes)
{
	if (m_seekPosition + bytes > Size())
		return FALSE;

	memcpy(dest, GetCurrentSeekPoint(), bytes);
	m_seekPosition += bytes;
	return TRUE;
}

DWORD CMappedFile::GetSeekPosition(void)
{
	return m_seekPosition;
}

bool CMappedFile::Unmap(LPCVOID data)
{
	if (!UnmapViewOfFile(data))
	{
		m_lpData = NULL;

		DEBUG_LOG(LL_ERR, "UnmapViewOfFile fail! File name: %s", m_filename);
		return false;
	}

	m_lpData = NULL;
	return true;
}
