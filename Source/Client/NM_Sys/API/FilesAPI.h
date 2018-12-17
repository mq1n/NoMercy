#pragma once

class File final 
{
	public:
		File(const File&) = delete; // Copy constructor
		File(File&&) = delete; // Move constructor
		File& operator = (const File&) = delete; // Copy-assignment operator
		File& operator = (File&&) = delete; // Move-assignment operator

		explicit File(HANDLE hFile) : 
			_hFile(hFile) 
		{
		}
		~File() 
		{ 
			Close(); 
		}

		NTSTATUS Close() 
		{
			const NTSTATUS Status = _hFile ? ZwClose(_hFile) : STATUS_SUCCESS;
			_hFile = NULL;

			return Status;
		}

		HANDLE Get() const 
		{
			return _hFile;
		}

	private:
		HANDLE _hFile;
};

class FilesAPI final 
{
	enum CREATE_FILE_TYPE 
	{
		fCreateEmpty,
		fOpenExisting,
		fOpenOrCreate
	};

	public:
		FilesAPI(LPCWSTR FilePath, CREATE_FILE_TYPE Type, ACCESS_MASK AccessMask, ULONG ShareAccess);
		~FilesAPI() 
		{ 
			Close();
		}

		NTSTATUS GetCreationStatus() const { return CreationStatus; }

		NTSTATUS Read(OUT PVOID Buffer, ULONG Size, OPTIONAL UINT64 Offset = 0) const;
		NTSTATUS Write(IN PVOID Buffer, ULONG Size, OPTIONAL UINT64 Offset = 0) const;
		NTSTATUS Close();

		static NTSTATUS CreateDir(LPCWSTR DirPath);
		static NTSTATUS DeleteFile(LPCWSTR FilePath);
		static NTSTATUS RenameFile(LPCWSTR OriginalFile, LPCWSTR NewName);
		static NTSTATUS MoveFile(LPCWSTR OriginalFile, LPCWSTR Destination);
		static NTSTATUS CopyFile(LPCWSTR OriginalFile, LPCWSTR Destination);
		static SIZE_T GetFileSize(LPCWSTR FilePath);
		static BOOLEAN IsFileExists(LPCWSTR FilePath);
		static BOOLEAN IsDirExists(LPCWSTR DirPath);

	private:
		HANDLE hFile;
		NTSTATUS CreationStatus;
};
