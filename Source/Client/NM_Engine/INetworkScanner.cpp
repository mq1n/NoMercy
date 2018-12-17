#include "NM_Main.h"
#include "NM_Index.h"
#include "Defines.h"

#include <Mstcpip.h>

static void hex_dump_bytes(char *addr, int len) 
{
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;

	for (i = 0; i < len; i++)
	{
		if ((i % 16) == 0)
		{
			if (i != 0)
				printf("  %s\n", buff);

			printf("  %04x ", i);
		}

		printf(" %02x", pc[i]);

		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	while ((i % 16) != 0)
	{
		printf("   ");
		++i;
	}

	printf("  %s\n\n\n", buff);
}


unsigned long get_local_ip()
{
	unsigned long local_ip = 0;
	struct sockaddr_in remote = { 0 };
	struct sockaddr_in local = { 0 };
	int len = sizeof(local);
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (INVALID_SOCKET != s)
	{
		do
		{
			remote.sin_family = AF_INET;
			remote.sin_addr.s_addr = 0x08080808; /* 8.8.8.8 */

			if (0 != connect(s, (struct sockaddr*)&remote, sizeof(remote)))
				break;

			if (0 != getsockname(s, (struct sockaddr*)&local, &len))
				break;

			local_ip = local.sin_addr.s_addr;
		} while (0);

		closesocket(s);
	}

	return local_ip;
}

SOCKET toxic_setup(unsigned long local_ip)
{
	SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

	if (INVALID_SOCKET == sock)
		return sock;

	do
	{
		struct sockaddr_in sa_in = { 0 };
		sa_in.sin_family = AF_INET;
		sa_in.sin_addr.s_addr = local_ip;
		sa_in.sin_port = htons(0);

		if (0 != bind(sock, (SOCKADDR *)&sa_in, sizeof(sa_in)))
			break;

		int rcv = RCVALL_ON; //RCVALL_IPLEVEL;
		DWORD b = 0;

		if (0 != WSAIoctl(sock, SIO_RCVALL, &rcv, sizeof(rcv), 0, 0, &b, 0, 0))
			break;

		return sock;
	} while (0);

	closesocket(sock);
	return INVALID_SOCKET;
}

void toxic_serpent()
{
	unsigned long local_ip = get_local_ip();

	if (0 == local_ip)
		return;

	SOCKET sock = toxic_setup(local_ip);

	if (sock == INVALID_SOCKET)
		return;

	for (;;)
	{
// 		static char buf[65535];
		char buf[512];
		int size = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
	
		if (size > 0)
			hex_dump_bytes(buf, size);

		Sleep(1);
	}

	closesocket(sock);
}

int InitNetworkScan()
{
	WSADATA wsa = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != ERROR_SUCCESS)
	{
		printf("WSAStartup Failed. Error Code: %d\n", WSAGetLastError());
		return 0;
	}

	toxic_serpent();

	WSACleanup();
	return 0;
}
