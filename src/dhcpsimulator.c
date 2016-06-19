/*
 ============================================================================
 Name        : dhcpsimulator.c
 Author      : t9knowledge
 Version     :
 Copyright   : t9knowledge@knowledgedatabase.jp
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#define DEBUG
#ifdef DEBUG
#define trace printf
#else
#define trace 1 ? (void) 0 : printf
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <winsock2.h>

#include "dhcp.h"

void parseMac(unsigned char *data, const char *mac) {
    int i;
    printf("string:%s\n", mac);

    int len = (int) strlen(mac);
    for (i = 0; i < len; i += 2) {
        unsigned int x;
        sscanf((char *) (mac + i), "%02x", &x);
        data[i / 2] = x;
    }
    for (i = len / 2; i < 16; ++i) {
        data[i] = (byte) 0;
    }
}

int sendDiscoverMessage() {
    struct WSAData wsaData;
    SOCKET sock;
    struct sockaddr_in addr;
    BOOL yes = 1;

    char *message;
    message = (char *) malloc(sizeof(char) * (244));
    if (message == NULL)
        exit(EXIT_FAILURE);

    message[0] = 1;
    message[1] = 1;
    message[2] = 6;
    message[3] = 0;
    message[4] = (int) 1;
    message[8] = 0;
    message[9] = 0;
    message[10] = htons(0x8000);
    message[12] = 0;
    message[13] = 0;
    message[14] = 0;
    message[15] = 0;
    message[16] = 0;
    message[17] = 0;
    message[18] = 0;
    message[19] = 0;
    message[20] = 0;
    message[21] = 0;
    message[22] = 0;
    message[23] = 0;
    message[24] = 0;
    message[25] = 0;
    message[26] = 0;
    message[27] = 0;
    message[28] = 1;
    message[29] = 2;
    message[30] = 3;
    message[31] = 4;
    message[32] = 5;
    message[33] = 6;
    message[34] = 0;
    message[35] = 0;
    message[36] = 0;
    message[37] = 0;
    message[38] = 0;
    message[39] = 0;
    message[40] = 0;
    message[41] = 0;
    message[42] = 0;
    message[43] = 0;
    int i;
    for (i = 0; i < 64; i++) {
        message[44 + i] = 0;
    }
    for (i = 0; i < 128; i++) {
        message[108 + i] = 0;
    }
    message[236] = 99;
    message[237] = 130;
    message[238] = 83;
    message[239] = 99;
    message[240] = (char) dhcpMessageType;
    message[241] = 1;
    message[242] = 1;
    message[243] = 0xff;

    WSAStartup(MAKEWORD(2, 0), &wsaData);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(67);
    addr.sin_addr.S_un.S_addr = inet_addr("192.168.215.255");

    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &yes, sizeof(yes));

    sendto(sock, message, 244, 0, (struct sockaddr *) &addr, sizeof(addr));

    closesocket(sock);

    WSACleanup();
    free(message);
    return EXIT_SUCCESS;
}

int checkAscii(int c) {
    return c >= ' ' && c <= '\~';
}

void dump(int len, char response[]) {
    printf("Received: [%d]\n", len);
    int i, j;
    char *psz = "%02x ";
    for (j = 0; j < len; j++) {
        printf("[%03x]  : ", j);
        for (i = 0; i < 16; i++) {
            //          if (recvString[j + i] != -1)
            printf(psz, response[j + i] & 0x000000FF);
        }
        printf("   ");
        for (i = 0; i < 16; i++)
            (checkAscii(response[j + i]) == 0 && (*(response + j + i) = '.')), putchar(response[j + i]);
        printf("\n");
        j = j + 15;
    }
}

int main(void) {

    struct WSAData wsaData;
    SOCKET sock;
    struct sockaddr_in broadcastAddr;
    char recvString[2048];
    int recvStringLen;
    unsigned long nonblocking = 1;

    WSAStartup(MAKEWORD(2, 0), &wsaData);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    ioctlsocket(sock, FIONBIO, &nonblocking);
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    broadcastAddr.sin_port = htons(68);

    bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr));

    sendDiscoverMessage();
    for (;;) {
        if ((recvStringLen = recv(sock, recvString, 2048, 0)) < 0) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                return EXIT_FAILURE;
            } else {
                printf(
                        "Still have not received packet...Waiting and then trying again\n");
                Sleep(2000);
            }
        } else
            break;
    }
    recvString[recvStringLen] = '\0';

#ifdef DEBUG
    dump(recvStringLen, recvString);
#endif

    closesocket(sock);
    WSACleanup();

    return EXIT_SUCCESS;
}
