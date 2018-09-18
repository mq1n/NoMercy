#pragma once

static enum EAppTypes
{
	NM_NULL_APP_TYPE,
	NM_CLIENT,
	NM_SERVER,
	NM_LAUNCHER,
	NM_SERVICE,
	NM_SHADOW,
	NM_TESTAPP,
	NM_HELPERAPP,	
	NM_MASTERSERVER	 // todo
};

class CNM_App : public std::enable_shared_from_this <CNM_App>
{
	public:
		CNM_App();
		~CNM_App() = default;

		// Initilziation
		bool Initialize(bool bClient, bool bProtected, LPDWORD pDwErrorCode);

		// Finalization
		bool Finalize(LPDWORD pDwErrorCode);

	protected:	
		__forceinline bool InitializeEx(bool bClient, bool bProtected, LPDWORD pDwErrorCode);
		__forceinline bool InitializeServer(LPDWORD pDwErrorCode);
		__forceinline bool InitializeClient(LPDWORD pDwErrorCode);
		__forceinline bool InitializeLauncher(bool bProtected, LPDWORD pDwErrorCode);
		__forceinline bool InitializeService(bool bProtected, LPDWORD pDwErrorCode);
		__forceinline bool InitializeTestMode(bool bClient, LPDWORD pDwErrorCode);
		__forceinline bool InitializeHelperApp(LPDWORD pDwErrorCode);
};

