//////////////////////////////////////////////////////////////////////////////
// Product:  UDP test utility
// Last Updated for Version: 4.0.03
// Date of the Last Update:  Mar 14, 2009
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
//
// This software may be distributed and modified under the terms of the GNU
// General Public License version 2 (GPL) as published by the Free Software
// Foundation and appearing in the file GPL.TXT included in the packaging of
// this file. Please note that GPL Section 2[b] requires that all works based
// on this software must also be made publicly available under the terms of
// the GPL ("Copyleft").
//
// Alternatively, this software may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GPL and are specifically designed for licensees interested in
// retaining the proprietary status of their code.
//
// Contact information:
// Quantum Leaps Web site:  http://www.quantum-leaps.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdio.h>
#include <conio.h>
#include <winsock2.h>                                   // for Windows sockets
#include <time.h>                                               // for clock()
#include <math.h>

//............................................................................
static SOCKET socketOpen(unsigned short localPortNum) {
    // initialize Windows sockets
    static WSADATA wsaData;
    int wsaErr = WSAStartup(MAKEWORD(2,0), &wsaData);
    if (wsaErr == SOCKET_ERROR) {
        fprintf(stderr, "Windows Sockets cannot be initialized.\n"
                        "The library reported error %d\n", wsaErr);
        return INVALID_SOCKET;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);                // UDP socket
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "creating UDP socket failed");
        WSACleanup();
        return INVALID_SOCKET;
    }

    // set the socket to non-blocking mode.
    ULONG ioctl_opt = 1;
    if (ioctlsocket(sock, FIONBIO, &ioctl_opt) == SOCKET_ERROR) {
        fprintf(stderr, "Socket configuration failed.\n"
                        "Windows socket error %d\n",
                        WSAGetLastError());
        WSACleanup();
        return INVALID_SOCKET;
    }

    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(localPortNum);

    // bind() associates a local address and port combination with the
    // socket just created. This is most useful when the application is a
    // server that has a well-known port that clients know about in advance.
    if (bind(sock, (struct sockaddr *)&local, sizeof(local))
        == SOCKET_ERROR)
    {
        fprintf(stderr, "Error by binding server socket\n"
                        "Windows socket error %d\n",
                        WSAGetLastError());
        WSACleanup();
        return INVALID_SOCKET;
    }

    return sock;                             // no error message, i.e. success
}

//............................................................................
static bool createRemoteAddr(struct sockaddr_in *targetAddr,
                             int targetAddrSize,
                             char const *remoteHostName,
                             unsigned short remotePorNum)
{
    struct hostent *remoteHost;
    remoteHost = gethostbyname(remoteHostName);   // initialize remote addr...
    if (remoteHost == 0) {
        fprintf(stderr, "gethostbyname() failed.\n"
                        "Windows socket error %d\n",
                        WSAGetLastError());
        WSACleanup();
        return false;
    }
    memset(targetAddr, 0, targetAddrSize);
    memcpy(&targetAddr->sin_addr, remoteHost->h_addr, remoteHost->h_length);
    targetAddr->sin_family = AF_INET;
    targetAddr->sin_port   = htons(remotePorNum);
    return true;
}
//............................................................................
int main(int argc, char *argv[]) {
    static char l_rx_buf[20];           // packet received from the UDP socket
    static char l_tx_buf[10];     // packet to be transmitted tothe UDP socket


    printf("qudp utility (c) Quantum Leaps, LLC, www.state-machine.com\n");
    if (argc < 2) {
        fprintf(stderr, "Usage:  qudp <ip-addr> [<port> [<local port>]]\n");
        return -1;
    }

    unsigned short port = 777;
    unsigned short local_port = 1777;
    if (argc > 2) {
        if (sscanf(argv[2], "%hd", &port) != 1) {
            fprintf(stderr, "incorrect port number: %s\n", argv[2]);
            return -2;
        }
        if (argc > 3) {
            if (sscanf(argv[3], "%hd", &local_port) != 1) {
                fprintf(stderr, "incorrect port number: %s\n", argv[3]);
                return -3;
            }
        }
    }
    printf("Opening and binding local socket at port=%hd...", local_port);
    static SOCKET sock = socketOpen(local_port);
    if (sock == INVALID_SOCKET) {
        return -4;
    }
    printf("ready\n");

    printf("Remote UDP connection: IP-address=%s, port=%hd\n", argv[1], port);
    struct sockaddr_in targetAddr;                // IP address of the tareget
    if (!createRemoteAddr(&targetAddr, sizeof(targetAddr), argv[1], port)) {
        return -5;
    }

    printf("Type command(s); press Enter to send command, ESC to quit\n");
    int len = 0;
    for (;;) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);

        struct timeval delay;
        delay.tv_sec  = 0;
        delay.tv_usec = 50*1000;                                 // wait 50 ms

        int nfound = ::select(0, &readSet, 0, 0, &delay);
        if (nfound > 0) {
            struct sockaddr from;
            int fromSize = sizeof(from);
            int nrec = recvfrom(sock, l_rx_buf, sizeof(l_rx_buf), 0,
                                &from, &fromSize);
            if (nrec == SOCKET_ERROR) {
                fprintf(stderr, "recvfrom() failed.\n"
                                "Windows socket error %d\n",
                                WSAGetLastError());
                closesocket(sock);
                printf("Aborted.\n");
                return -6;
            }
            else if (nrec > 0) {
                if (nrec > (int)sizeof(l_rx_buf) - 1) {
                    nrec = (int)sizeof(l_rx_buf) - 1;
                }
                l_rx_buf[nrec] = 0; // make sure that l_buf is zero-terminated
                printf("Received: \"%s\"\n", (char const *)l_rx_buf);
            }
        }

        if (kbhit()) {
            int ch = getch();
            if (ch == 0x1B) {                                      // ESC key?
                break;                                            // terminate
            }
            else if (ch == 0x0D) {                               // Enter key?
                l_tx_buf[len++] = 0;                         // zero-terminate
                sendto(sock, l_tx_buf, len, 0,
                       (struct sockaddr *)&targetAddr, sizeof(targetAddr));
                printf("\nSending:  \"%s\"\n", l_tx_buf);
                len = 0;                         // get ready for next command
            }
            else if (ch == 0x08) {                                // backspace
                if (len > 0) {
                    l_tx_buf[--len] = ' ';
                    printf("%c", (char)ch);    // echo the char to the console
                }
            }
            else {
                if (len < (int)sizeof(l_tx_buf) - 1) {
                    l_tx_buf[len++] = (char)ch;        // add to the TX buffer
                    printf("%c", (char)ch);    // echo the char to the console
                }
            }
        }
    }

    closesocket(sock);
    printf("Done.\n");

    return 0;
}

