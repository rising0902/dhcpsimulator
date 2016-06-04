/*
 ============================================================================
 Name        : dhcpsimulator.c
 Author      : t9knowledge
 Version     :
 Copyright   : t9knowledge@knowledgedatabase.jp
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#define MAC 12

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <winsock2.h>

struct DHCPHeader {
	/// Message op code / message type. 1 = BOOTREQUEST, 2 = BOOTREPLY
	byte op;

	/// Hardware address type, see ARP section in "Assigned Numbers" RFC; e.g., '1' = 10mb ethernet.
	byte htype;

	/// Hardware address length (e.g.  '6' for 10mb ethernet).
	byte hlen;

	/// Client sets to zero, optionally used by relay agents when booting via a relay agent.
	byte hops;

	/// Transaction ID, a random number chosen by the client, used by the client and server to associate messages and responses between a client and a server.
	int xid;

	/// Filled in by client, seconds elapsed since client began address acquisition or renewal process.
	short secs;

	/// Flags. (Only the BROADCAST flag is defined.)
	short flags;

	/// Client IP address; only filled in if client is in BOUND, RENEW or REBINDING state and can respond to ARP requests.
	int ciaddr;

	/// 'your' (client) IP address.
	int yiaddr;

	/// IP address of next server to use in bootstrap; returned in DHCPOFFER, DHCPACK by server.
	int siaddr;

	/// Relay agent IP address, used in booting via a relay agent.
	int giaddr;

	/// Client hardware address.
	byte chaddr[16];

	/// Optional server host name, null terminated string.
	char sname[64];

	/// Boot file name, null terminated string; "generic" name or null in DHCPDISCOVER, fully qualified directory-path name in DHCPOFFER.
	char file[128];
};

struct DHCPOption{
	byte type;
	byte data[1];
};

struct DHCPMessage {
	struct DHCPHeader header;
	struct DHCPOption options[1];
};

enum DHCPOptionType
{
	subnetMask = 1,
	timeOffset = 2,
	router = 3,
	timeServer = 4,
	nameServer = 5,
	domainNameServer = 6,
	domainName = 15,
	broadcastServerOption = 28,
	netbiosNodeType = 46,
	leaseTime = 51,
	dhcpMessageType = 53,
	serverIdentifier = 54,
	parameterRequestList = 55,
	renewalTime = 58,
	rebindingTime = 59,
	vendorClassIdentifier = 60,
	tftpServerName = 66,
	bootfileName = 67,
};

enum DHCPMessageType
{
	discover = 1,
	offer,
	request,
	decline,
	ack,
	nak,
	release,
	inform
};

enum NETBIOSNodeType
{
	bNode = 1,
	pNode,
	mMode,
	hNode
};

void toString(unsigned char *data) {
	int i;
	for (i=0; i<16; i++) {
		printf("%02x", data[i]);
	}
	printf("\n");
}

void parseMac(unsigned char *data, const char *mac) {
	int i;
	printf("string:%s\n", mac);

	int len = (int)strlen(mac);
	for (i=0; i<len; i+=2) {
		unsigned int x;
		sscanf((char *)(mac + i), "%02x", &x);
		data[i/2] = x;
	}
	for (i=len/2; i<16; ++i) {
		data[i] = (byte)0;
	}

	toString(data);
}

int main(void) {
	int i;
	byte data[16];
	char mac[] = "000102030405";
	short j;
	printf("int = %d | i = %d\ndouble = %d\n%0" , sizeof(int) , sizeof j , sizeof(double), htons(0x8000));
	parseMac(data, mac);

	struct DHCPHeader header = {(byte)1, (byte)1, (byte)6, (byte)0, (int)1, (short)0, htons(0x8000), (int)0, (int)0, (int)0, (int)0, data, (byte)0, (byte)0};
	byte values[1] = {(byte)1};
	struct DHCPOption option[1] = { (byte)53, values};
	struct DHCPMessage dHCPMessage = {header, option};

	int *heap;
	heap = (byte *)malloc(sizeof(byte) * (238 + 3));
	if (heap == NULL) exit(EXIT_FAILURE);

	heap[0] = header.op;
	heap[1] = header.htype;
	heap[2] = header.hlen;
	heap[3] = header.hops;
	heap[4] = header.xid;
	heap[8] = header.secs;
	heap[10] = header.flags;
	heap[12] = header.ciaddr;
	heap[16] = header.yiaddr;
	heap[20] = header.siaddr;
	heap[24] = header.giaddr;
	for (i = 0; i < sizeof(header.chaddr); i++) {
		heap[27+i] = header.chaddr[i];
	}
	//heap[27] = header.chaddr;
	for (i = 0; i < sizeof(header.sname); i++) {
		heap[43+i] = header.sname[i];
	}
	//heap[43] = header.sname;
	for (i = 0; i < sizeof(header.file); i++) {
		heap[107+i] = header.file[i];
	}
	//heap[107] = header.file;
	heap[235] = (byte)99;
	heap[236] = (byte)130;
	heap[237] = (byte)83;
	heap[238] = (byte)99;
	heap[239] = (byte)53;
	heap[240] = (byte)1;
	heap[241] = (byte)1;

	struct WSAData wsaData;
	SOCKET sock;
	struct sockaddr_in addr;
	BOOL yes = 1;
	struct sockaddr_in broadcastAddr;
	char recvString[2048];
	int recvStringLen;
	char buf[2048];
	unsigned long nonblocking = 1;

	WSAStartup(MAKEWORD(2,0), &wsaData);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(68);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	//setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
	//bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	ioctlsocket(sock, FIONBIO, &nonblocking);
	memset(&broadcastAddr, 0, sizeof(broadcastAddr));
	broadcastAddr.sin_family = AF_INET;
	broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	broadcastAddr.sin_port = htons(68);
	//memset(buf, 0, sizeof(buf));
	//recv(sock, buf, sizeof(buf), 0);
	bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr));

	for (;;)
	{
		if ((recvStringLen = recvfrom(sock, recvString, 2048, 0, NULL, 0)) < 0)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				return EXIT_FAILURE;
			} else {
				printf("Still have not received packet...Waiting and then trying again\n");
				Sleep(2000);  /* Sleep for 2 milliseconds */
			}
		}
		else
			break;
	}
	recvString[recvStringLen] = '\0';
	printf("Received: %s\n", recvString);
	closesocket(sock);
	WSACleanup();
	//printf("---------------------------\n");
	//printf("%s\n", buf);
	//printf("---------------------------\n");

	//closesocket(sock);

	//WSACleanup();
	/*
	WSAStartup(MAKEWORD(2,0), &wsaData);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(67);
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.215.255");

	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));

	sendto(sock, heap, 237, 0, (struct sockaddr *)&addr, sizeof(addr));

	closesocket(sock);

	WSACleanup();
	free(heap);
	printf("exit success\n");*/
	return EXIT_SUCCESS;
}
