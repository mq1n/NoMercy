#pragma once

class CDirFunctions : public std::enable_shared_from_this <CDirFunctions>
{
	public:
		bool IsFileExist(const std::string& szFileName);
		bool IsDirExist(const std::string& dirName_in);

		std::string ReadFileContent(const std::string& szFileName);
		void WriteFileContent(const std::string& szFileName, const std::string& szText);

		bool ManageFsRedirection(bool bDisable, PVOID pCookie, PVOID * ppCookie);

		int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories = true);

		std::string ExeName();
		std::string ExePath();;
		std::string ExeNameWithPath();
		std::string WinPath();
		std::string SystemPath();

		bool IsFromWindowsPath(const std::string & szPath);
		bool IsFromCurrentPath(const std::string & szPath);

		std::string GetNameFromPath(std::string __wszFileName);
		std::wstring GetNameFromPath(std::wstring __wszFileName);

		std::string GetPathFromProcessName(std::string szBuffer);

		bool IsPackedExecutable(const std::string & szName);

		std::string CreateTempFileName(const std::string & szPrefix);

		DWORD GetFileSize(const std::string & szName);
		std::string GetFileMd5(const std::string & szName);
		std::string GetSHA256(LPCVOID c_lpData, std::size_t cbSize);
		std::string GetFileSHA256(const std::string & szName);

		void CheckFileSize(const std::string & szName, DWORD ulCorrectSize);
		void CheckFileMd5(const std::string & szName, const std::string & szCorrectHash);
};
