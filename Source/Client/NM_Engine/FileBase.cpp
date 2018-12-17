#include "NM_Main.h"
#include "NM_Index.h"
#include "FileBase.h"
#include "Defines.h"

CFileBase::CFileBase() :
	m_hFile(NULL), m_dwSize(0)
{
}

CFileBase::~CFileBase()
{
	Destroy();
}

char * CFileBase::GetFileName()
{
	return m_filename;
}

void CFileBase::Destroy()
{
	Close();
	m_dwSize = 0;
}

void CFileBase::Close()
{
	if (m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
}

BOOL CFileBase::Create(const char* filename, EFileMode mode)
{
	Destroy();

	strncpy(m_filename, filename, MAX_PATH);

	DWORD dwMode = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	if (mode == FILEMODE_WRITE)
	{
		dwMode = GENERIC_READ | GENERIC_WRITE;
		dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}

	m_hFile = CreateFileA(filename, dwMode, dwShareMode, NULL, mode == FILEMODE_READ ? OPEN_EXISTING : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		m_dwSize = GetFileSize(m_hFile, NULL);
		m_mode = mode;
		return true;
	}

	m_hFile = NULL;
	return false;
}

DWORD CFileBase::Size()
{
	return (m_dwSize);
}

void CFileBase::SeekCur(DWORD size)
{
	SetFilePointer(m_hFile, size, NULL, FILE_CURRENT);
}

void CFileBase::Seek(DWORD offset)
{
	if (offset > m_dwSize)
		offset = m_dwSize;

	SetFilePointer(m_hFile, offset, NULL, FILE_BEGIN);
}

DWORD CFileBase::GetPosition()
{
	return SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
}

BOOL CFileBase::Write(const void* src, int bytes)
{
	DWORD dwUseless;
	BOOL ret = WriteFile(m_hFile, src, bytes, &dwUseless, NULL);

	if (!ret)
		return false;

	m_dwSize = GetFileSize(m_hFile, NULL);
	return true;
}

BOOL CFileBase::Read(void* dest, int bytes)
{
	DWORD dwUseless;
	return ReadFile(m_hFile, dest, bytes, &dwUseless, NULL);
}

BOOL CFileBase::IsNull()
{
	return !m_hFile ? true : false;
}
