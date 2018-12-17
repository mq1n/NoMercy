#pragma once

enum EFileMode
{
	FILEMODE_READ = (1 << 0),
	FILEMODE_WRITE = (1 << 1)
};

class CFileBase
{
	public:
		CFileBase();
		virtual	~CFileBase();

		void			Destroy();
		void			Close();

		BOOL			Create(const char* filename, EFileMode mode);
		DWORD			Size();
		void			SeekCur(DWORD size);
		void			Seek(DWORD offset);
		DWORD			GetPosition();

		virtual BOOL	Write(const void* src, int bytes);
		BOOL			Read(void* dest, int bytes);

		char*			GetFileName();
		BOOL			IsNull();

	protected:
		int				m_mode;
		char			m_filename[MAX_PATH+1];
		HANDLE			m_hFile;
		DWORD			m_dwSize;
};
