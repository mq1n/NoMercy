#pragma once

typedef char*(__cdecl* TPluginName)(void);
typedef char*(__cdecl* TPluginVersion)(void);
typedef char*(__cdecl* TPluginErrorMessage)(void);
typedef bool (__cdecl* TPluginOnInit)(LPVOID);
typedef bool (__cdecl* TPluginOnFinal)(void);

typedef struct _PLUGIN_DATA
{
	HMODULE		hPlugin;
	DWORD_PTR	dwName;
	DWORD_PTR	dwVersion;
	DWORD_PTR	dwErrorMessage;
	DWORD_PTR	dwInitRoutine;
	DWORD_PTR	dwFinalRoutine;
} SPluginData, *PPluginData;

typedef struct _PLUGIN_INFO_DATA
{
	char	szPluginName[MAX_PATH];
	DWORD	dwFileSize;
	char	szEggHash[128];
//	char	szDecompressedHash[128];
//	char	szPureModuleHash[128];
//	char	szCryptptedCryptKey[128]; XOR'd AES256
} SPluginInfoData, *PPluginInfoData;


class CPlugin
{
	public:
		CPlugin() = default;
		~CPlugin() = default;

		bool LoadPlugins();
		bool UnloadPlugins();
		bool ExecutePlugins();
		bool FinalizePlugins();

		bool RequestPluginList();
		bool OnPluginInfoReceived(const std::shared_ptr <SPluginInfoData> & pPluginInfo);

	private:
		std::vector <std::shared_ptr <SPluginData> >		m_vLoadedPlugins;
		std::vector <std::shared_ptr <SPluginInfoData> >	m_vPlugins;
};

