#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include <DbgHelp.h>
#include "DbgHelpWrapper.h"


CDbgHelpWrapper::CDbgHelpWrapper()
{
	m_hProcess = GetCurrentProcess();
	m_IsInitialized = FALSE;
	m_ModuleBase = 0;
	m_DefaultSymbolsPath = "srv*C:\\Symbols*http://msdl.microsoft.com/download/symbols";
}

CDbgHelpWrapper::~CDbgHelpWrapper() 
{
	DeinitializeDbgHelp();
}


BOOL CDbgHelpWrapper::InitializeDbgHelp(LPSTR SymbolsPath) 
{
	if (SymbolsPath == NULL)
		SymbolsPath = (LPSTR)m_DefaultSymbolsPath;

	if (m_IsInitialized) 
		DeinitializeDbgHelp();

	m_IsInitialized = SymInitialize(m_hProcess, SymbolsPath, FALSE);
	return m_IsInitialized;
}

BOOL CDbgHelpWrapper::DeinitializeDbgHelp()
{
	if (m_IsInitialized && SymCleanup(m_hProcess))
		m_IsInitialized = FALSE;

	return m_IsInitialized;
}


BOOL CDbgHelpWrapper::LoadSymbols(LPSTR ModulePath) 
{
	m_ModuleBase = SymLoadModuleEx(m_hProcess, NULL, ModulePath, NULL, 0, 0, NULL, 0);
	return m_ModuleBase != 0;
}

BOOL CDbgHelpWrapper::GetRootSymbol(LPSTR SymbolName, PULONG SymbolIndex)
{
	SYMBOL_INFO SymbolInfo = { 0 };
	SymbolInfo.SizeOfStruct = sizeof(SymbolInfo);

	BOOL Status = SymGetTypeFromName(m_hProcess, m_ModuleBase, SymbolName, &SymbolInfo);
	if (Status) 
		*SymbolIndex = SymbolInfo.Index;
	return Status;
}

BOOL CDbgHelpWrapper::GetChildrenCount(ULONG SymbolIndex, OUT PULONG ChildrenCount)
{
	return SymGetTypeInfo(m_hProcess, m_ModuleBase, SymbolIndex, TI_GET_CHILDRENCOUNT, ChildrenCount);
}

BOOL CDbgHelpWrapper::GetChildrenSymbols(ULONG ParentSymbolIndex, ULONG* IndicesBuffer, ULONG MaxIndices, OUT ULONG &ChildrenCount) 
{
	if ((IndicesBuffer == NULL) || (MaxIndices == 0))
		return FALSE;

	if (!GetChildrenCount(ParentSymbolIndex, &ChildrenCount))
		return FALSE;

	if (ChildrenCount == 0) 
		return TRUE;

	CONST ULONG FindChildrenSize = sizeof(TI_FINDCHILDREN_PARAMS) + ChildrenCount * sizeof(ULONG);
	TI_FINDCHILDREN_PARAMS* FindChildrenParams = (TI_FINDCHILDREN_PARAMS*)malloc(FindChildrenSize);
	memset(FindChildrenParams, 0, FindChildrenSize);

	FindChildrenParams->Count = ChildrenCount;

	if (!SymGetTypeInfo(m_hProcess, m_ModuleBase, ParentSymbolIndex, TI_FINDCHILDREN, FindChildrenParams)) 
	{
		free(FindChildrenParams);
		return FALSE;
	}

	ULONG IndicesToCopyCount = ChildrenCount > MaxIndices ? MaxIndices : ChildrenCount;
	for (ULONG i = 0; i < IndicesToCopyCount; i++) 
	{
		IndicesBuffer[i] = FindChildrenParams->ChildId[i];
	}

	free(FindChildrenParams);

	return TRUE;
}



ULONG CDbgHelpWrapper::GetSymbolIndex(LPWSTR SymbolName, ULONG* IndicesBuffer, ULONG IndicesCount)
{
	for (ULONG i = 0; i < IndicesCount; i++) 
	{
		LPWSTR CurrentSymbolName = NULL;
		if (GetSymbolName(IndicesBuffer[i], &CurrentSymbolName)) 
		{
			if (wcscmp(CurrentSymbolName, SymbolName) == 0) 
			{
				FreeSymbolName(SymbolName);
				return IndicesBuffer[i];
			}

			FreeSymbolName(SymbolName);
		}
	}

	return 0;
}

ULONG CDbgHelpWrapper::GetSymbolIndex(ULONG ParentSymbolIndex, LPWSTR SymbolName)
{
	ULONG ChildrenIndex = 0;

	ULONG ChildrenCount = 0;
	if (!GetChildrenCount(ParentSymbolIndex, &ChildrenCount)) 
		return 0;

	if (ChildrenCount == 0)
		return 0;

	PULONG ChildrenIndices = (PULONG)malloc(ChildrenCount * sizeof(ULONG));
	memset(ChildrenIndices, 0, ChildrenCount);

	if (GetChildrenSymbols(ParentSymbolIndex, ChildrenIndices, ChildrenCount, ChildrenCount)) 
	{
		ChildrenIndex = GetSymbolIndex(SymbolName, ChildrenIndices, ChildrenCount);
	}

	free(ChildrenIndices);

	return ChildrenIndex;
}



BOOL CDbgHelpWrapper::GetSymbolName(ULONG SymbolIndex, OUT LPWSTR* SymbolName) 
{
	return SymGetTypeInfo(m_hProcess, m_ModuleBase, SymbolIndex, TI_GET_SYMNAME, SymbolName);
}

VOID CDbgHelpWrapper::FreeSymbolName(LPWSTR SymbolName)
{
	VirtualFree(SymbolName, 0, MEM_RELEASE);
}

BOOL CDbgHelpWrapper::GetSymbolOffset(ULONG SymbolIndex, OUT PULONG Offset)
{
	return SymGetTypeInfo(m_hProcess, m_ModuleBase, SymbolIndex, TI_GET_OFFSET, Offset);
}

