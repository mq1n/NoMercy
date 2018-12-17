# NoMercy

The "NoMercy" project is "the gold standard" open source Windows kernel/user mode anti cheat written with C/C++.  

## Notes;
* The project will not be updated for a while, due than my computer is corrupted.
* **The project is still under development**
* Currently just developed and tested on Windows 10 x64 RS4(1804) as WoW64 process
* Build 3rd party dependencies or download [released pre-compiled bundle](https://github.com/mq1n/NoMercy/releases/download/v1.0/CompiledLibs.zip) and extract .zip content to "NoMercy\Extern\CompiledLibs"

### Prerequisites
* [Microsoft Visual Studio](https://visualstudio.microsoft.com/downloads/)
* [Windows Driver Kit](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)

### System requirements
* Windows Vista or greater

## Planned Features;
* DLL inject detection
* Thread inject detection
* Harmful thread detection
* Virtual memory code inject detection
* Virtual memory code modification detection
* Virtual memory module integrity protection
* Virtual memory hook & breakpoint & dump protection
* Ingame harmful window detection
* Mouse & Keyboard automation detection
* Ingame keyboard hijack detection
* Software & hardware macro detection
* Multi client detection
* Game client modification detection
* Detect/Reject/Analysis game process memory access
* Detect/Reject/Analysis game process handle access
* Detect/Reject Kernel and User mode debugging
* Detection execute on virtual machine, emulation and generic sandboxes
* Hardware information based ban system
* Heuristic detection
	* Common checks; File, Driver, Handle, Heap, Module, System object, Process, Virtual memory section, Thread, Window
* Self integrity protection
* Splash UI
* Game specific protection methods
* Special ring3 access protection

## Workflow
	- to be added later

## Download
	- to be added later

## TODO, In development features;

### TODO Main features;

* Node JS based WebSocket API server
* Plugin based heartbeat system
* Self updater
* Global hook engine
* Protected network wrapper API for game <> client communication
* Web interface
* ARK like Remote Administration tool for manage master server connections

### TODO Network Features;

* Sequence
* Daemon for master server(s)
* P2P pool for master server(s)

### TODO Kernel features;

* Driver to Service Logger instance
* Integrity check for self OB callbacks
* Enumerate kernel memory for find manually mapped and hidden(unlinked), deleted(file), non-signed drivers, hijacked objects, non device created drivers also check loaded driver file-memory integritys
* Anti hook (Inline, IAT, EAT, SSDT, SSSDT, IDT)
* APC monitor for block injection
* Anti speed hack based time modification detection/manipulation
* Ring3 protection(PPL) for self protected apps
* Anti debug/sandbox/hypervisor/virtualization
* Scan/Analyse non paged allocations
* Physical memory based SBD scan (MmGetPhysicalMemoryRanges)
* Child process create monitor for our protected apps
* Protect from inherit'd handles
* Scan/Analyse VAD entries and hidden VAD entries
* g_CiOptions integrity check
* ExpLookupHandleTableEntry integrity check
* KPP integrity check
* SMEP integrity check
* PPL manipulation check
* PspNotifyEnableMask integrity check
* Physical memory user mode access check
* Block game hwnd access from window APIs(NtUserQueryWindow/NtUserBuildHwndList/NtUserFindWindowEx/NtUserGetForegroundWindow/NtUserSetWindowLong/NtUserWindowFromPoint)
* Block suspend access to our kernel threads from System(4) process
* Split handle owner system apps(csrss, lsass, pcasvc) accesses
* Scan unloaded drivers(MmUnloadedDriver-MmLastUnloadedDriver) Also manipulation check just like list is it null, list is filled with fake modules(time ranges)
* Hook & Analyse IRP_MJ_CREATE calls

### TODO WebAPI Features;

* Real time detection status
* Game statistics data as Daily/weekly/monthly/quarterly/yearly
* Accessed counts
* Executed counts
* Hacking tool detection counts
* Unknown hardware devise frequency of use
* Nationally executed/access frequency of use
* Game play frequency of use
* Operate system frequency use
* National VPN detour frequency use
* List of nationality hacking tool detection/user
* Provide daily/weekly/monthly user statistic data
* Rank of hacking tool used IP
* Rank of hacking tool used system
* Rank of hacking tool used account
* Provide daily/weekly data
* Accessed account/system mapping information
* Hacking tool detection log information

## Dependencies

* [amy](https://github.com/liancheng/amy) - [License](https://github.com/liancheng/amy/blob/master/LICENSE)
* [asyncplusplus](https://github.com/Amanieu/asyncplusplus) - [License](https://github.com/Amanieu/asyncplusplus/blob/master/LICENSE)
* [boost](https://github.com/boostorg/) - [License](https://www.boost.org/users/license.html)
* [ClanLib](https://github.com/sphair/ClanLib) - [License](https://github.com/sphair/ClanLib/blob/master/COPYING)
* [cryptopp](https://github.com/weidai11/cryptopp/) - [License](https://github.com/weidai11/cryptopp/blob/master/License.txt)
* [Deviare-InProc](https://github.com/nektra/Deviare-InProc/) - [License](https://github.com/nektra/Deviare-InProc/blob/master/LICENSE.GPL)
* [lazy_importer](https://github.com/JustasMasiulis/lazy_importer) - [License](https://github.com/JustasMasiulis/lazy_importer/blob/master/LICENSE)
* [MySQL](https://www.mysql.com/products/connector/) - [License](https://downloads.mysql.com/docs/licenses/connector-j-5.1-gpl-en.pdf)
* [rtti-obfuscator](https://github.com/koemeet/rtti-obfuscator) - [License](https://github.com/koemeet/rtti-obfuscator/blob/master/LICENSE)
* [spdlog](https://github.com/gabime/spdlog) - [License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE)
* [tinyxml2](https://github.com/leethomason/tinyxml2) - [License](https://www.zlib.net/zlib_license.html)
* [xorstr](https://github.com/JustasMasiulis/xorstr) - [License](https://github.com/JustasMasiulis/xorstr/blob/master/LICENSE)
* [xxHash](https://github.com/Cyan4973/xxHash) - [License](https://github.com/Cyan4973/xxHash/blob/dev/LICENSE)
* [rewolf-wow64ext](https://github.com/rwfpl/rewolf-wow64ext) - [License](https://github.com/rwfpl/rewolf-wow64ext/blob/master/lgpl-3.0.txt)
* [ReflectiveDLLRefresher](https://github.com/CylanceVulnResearch/ReflectiveDLLRefresher) - [License](https://github.com/CylanceVulnResearch/ReflectiveDLLRefresher/blob/master/LICENSE)
* [Kernel-Bridge](https://github.com/HoShiMin/Kernel-Bridge) - [License](https://github.com/HoShiMin/Kernel-Bridge/blob/master/LICENSE)

### Protector SDK files
* [Themida/CodeVirtualizer](https://www.oreans.com)
* [Enigma](https://enigmaprotector.com)
* [Shielden](http://safengine.com)
* [VMProtect](http://vmpsoft.com)

## License
NoMercy is licensed under the [GPLv3](https://github.com/mq1n/NoMercy/blob/master/LICENSE) License. Dependencies are under their respective licenses.

## Contributing
* Fork it(https://github.com/mq1n/NoMercy/fork)
* Create your feature branch: "git checkout -b my-new-feature".
* Commit your changes: "git commit -am 'Add some feature'".
* Push to the branch: "git push origin my-new-feature".
* Submit a pull request.

## Credits
- [mq1n](https://github.com/mq1n)

## Donations
If you did like to buy me a coffee, I won't complain. x)

### BTC
	1P6GqjR9TcoDkVNz4TJqQaH5SyTgnsyirB

### Etherium
	0xdA847F888f2dfB1c491f0bf4cEc1D3A718e56695

### BTC Cash
	qz9jn9yjkyxvp7tfjffjc2wjf4yxvgey3sekl3yrqu
