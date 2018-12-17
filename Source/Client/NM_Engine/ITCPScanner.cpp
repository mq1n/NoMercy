#pragma once
#include "NM_Main.h"
#include "NM_Index.h"
#include "Defines.h"

#if 0

#ifndef AF_INET
	#define AF_INET         2               // internetwork: UDP, TCP, etc.
#endif

void ListTcpConnection()
{
	MIB_TCPTABLE_OWNER_PID *tcp_pid;
	MIB_TCPROW_OWNER_PID *owner_pid;
	DWORD size;
	DWORD dresult;

	dresult = GetExtendedTcpTable(NULL, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, NULL);

	if (dresult < 0)
	{
		std::cout << "GetExtendedTcpTable failed" << std::endl;
		return;
	}

	tcp_pid = new MIB_TCPTABLE_OWNER_PID[size];
	dresult = GetExtendedTcpTable(tcp_pid, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, NULL);

	struct in_addr addr;
	for (int i = 0; i < tcp_pid->dwNumEntries; ++i)
	{
		owner_pid = &tcp_pid->table[i];

		std::cout << "pid = \t" << owner_pid->dwOwningPid;
		addr.S_un.S_addr = (unsigned long)owner_pid->dwLocalAddr;
		std::cout << "\t\tlocal addr = " << g_winapiApiTable->inet_ntoa(addr);
		addr.S_un.S_addr = (unsigned long)owner_pid->dwRemoteAddr;
		std::cout << "\t\t\tremot addr = " << g_winapiApiTable->inet_ntoa(addr) << std::endl;
	}

	delete[] tcp_pid;
}

#endif

