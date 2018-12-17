#pragma once

#pragma comment(linker, "/NODEFAULTLIB:LIBCMTD")
#pragma comment(linker, "/NODEFAULTLIB:msvcrt")

#ifdef _M_X64
	#pragma comment(lib, "MySQL/libmysql_x64.lib")
	#pragma comment(lib, "MySQL/mysqlclient_x64.lib")

	#pragma comment(lib, "boost/lib64-msvc-14.1/libboost_date_time-vc141-mt-sgd-x64-1_68.lib")
	#pragma comment(lib, "boost/lib64-msvc-14.1/libboost_regex-vc141-mt-sgd-x64-1_68.lib")
	#pragma comment(lib, "boost/lib64-msvc-14.1/libboost_system-vc141-mt-sgd-x64-1_68.lib")

	#ifdef _DEBUG
		#pragma comment(lib, "cryptopp/cryptlib_x64_debug.lib")
		#pragma comment(lib, "Deviare-InProc/NktHookLib64_Debug.lib")
		#pragma comment(lib, "ClanLib/x64/clanCore-static-mt-debug.lib")
		#pragma comment(lib, "xxHash/xxhash_x64_debug.lib")
	#else
		#pragma comment(lib, "cryptopp/cryptlib_x64_release.lib")
		#pragma comment(lib, "Deviare-InProc/NktHookLib64.lib")
		#pragma comment(lib, "ClanLib/x64/clanCore-static-mt.lib")
		#pragma comment(lib, "xxHash/xxhash_x64_release.lib")
	#endif
#elif _M_IX86
	#pragma comment(lib, "MySQL/libmysql_x86.lib")
	#pragma comment(lib, "MySQL/mysqlclient_x86.lib")

	#pragma comment(lib, "boost/lib32-msvc-14.1/libboost_date_time-vc141-mt-sgd-x32-1_68.lib")
	#pragma comment(lib, "boost/lib32-msvc-14.1/libboost_regex-vc141-mt-sgd-x32-1_68.lib")
	#pragma comment(lib, "boost/lib32-msvc-14.1/libboost_system-vc141-mt-sgd-x32-1_68.lib")

	#ifdef _DEBUG
		#pragma comment(lib, "cryptopp/cryptlib_x86_debug.lib")
		#pragma comment(lib, "Deviare-InProc/NktHookLib_Debug.lib")
		#pragma comment(lib, "ClanLib/Win32/clanCore-static-mt-debug.lib")
		#pragma comment(lib, "xxHash/xxhash_x86_debug.lib")
	#else
		#pragma comment(lib, "cryptopp/cryptlib_x86_release.lib")
		#pragma comment(lib, "Deviare-InProc/NktHookLib.lib")
		#pragma comment(lib, "ClanLib/Win32/clanCore-static-mt.lib")
		#pragma comment(lib, "xxHash/xxhash_x86_release.lib")
	#endif
#else
	#error "architecture unsupported"
#endif

