#pragma once

class CDbgHelpWrapper
{
	public:
		CDbgHelpWrapper();
		~CDbgHelpWrapper();

		BOOL InitializeDbgHelp(LPSTR SymbolsPath = NULL);
		BOOL DeinitializeDbgHelp();

		BOOL LoadSymbols(LPSTR ModulePath);

		BOOL GetRootSymbol(LPSTR SymbolName, PULONG SymbolIndex);
		BOOL GetChildrenCount(ULONG SymbolIndex, OUT PULONG ChildrenCount);
		BOOL GetChildrenSymbols(ULONG ParentSymbolIndex, ULONG* IndicesBuffer, ULONG MaxIndices, OUT ULONG &ChildrenCount);
	
		ULONG GetSymbolIndex(LPWSTR SymbolName, ULONG* IndicesBuffer, ULONG IndicesCount);
		ULONG GetSymbolIndex(ULONG ParentSymbolIndex, LPWSTR SymbolName);

		BOOL GetSymbolName(ULONG SymbolIndex, OUT LPWSTR* SymbolName);
		VOID FreeSymbolName(LPWSTR SymbolName);
		BOOL GetSymbolOffset(ULONG SymbolIndex, OUT PULONG Offset);

	private:
		BOOL    m_IsInitialized;
		HANDLE  m_hProcess;
		DWORD64 m_ModuleBase;

		LPCSTR	m_DefaultSymbolsPath;
};

