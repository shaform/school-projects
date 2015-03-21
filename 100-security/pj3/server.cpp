// Version of Windows, 0x501 means WINXP
#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <algorithm>
using namespace std;

#define STUDENT_ID "0000000"
const char *SEC_PORT = "2000";  // server port
const int BUF_LEN = 100;

#pragma comment(lib,"Ws2_32.lib")

// -- RC4 -- //
const char *KEY = "hackerneverdie";
unsigned char S[256], PRGAi, PRGAj;
void KSA(const char *key, int keylen)
{
    for (int i=0; i<256; ++i)
        S[i] = i;

    for (int i=0, j=0; i<256; ++i) {
        j = (j + S[i] + key[i%keylen]) % 256;
        std::swap(S[i], S[j]);
    }
    PRGAi = PRGAj = 0;
}
void PRG_init()
{
    KSA(KEY, strlen(KEY));
}
unsigned char PRGA()
{
    PRGAi = (PRGAi + 1) % 256;
    PRGAj = (PRGAj + S[PRGAi]) % 256;
    std::swap(S[PRGAi], S[PRGAj]);
    return S[(S[PRGAi] + S[PRGAj]) % 256];
}
// -- RC4 -- //

SOCKET clientSocket;

DWORD WINAPI handle(LPVOID)
{
    char recvbuf[BUF_LEN+1];
    int iResult;

    PRG_init();  // initialize PRG on every connection

    do {
        iResult = recv(clientSocket, recvbuf, BUF_LEN, 0);
        if (iResult > 0) {
            // end the string
            recvbuf[iResult] = '\0';

            printf("received   : %s\n", recvbuf);
            printf("hexadecimal: ");
            for (int i=0; i<iResult; ++i) {
                unsigned ohex = recvbuf[i];
                printf("%02x ", ohex & 0xff);
            }
            printf("\n");

            // decrypt the message
            for (int i=0; i<iResult; ++i) {
                recvbuf[i] = recvbuf[i] ^ PRGA();
            }
            printf("decrypted  : %s\n", recvbuf);
            FILE *log_file = fopen("C:\\" STUDENT_ID ".txt", "a+");
            // only save when the file was opened successfully
            if (log_file) {
                fputs(recvbuf, log_file);
                fclose(log_file);
            }
        } else if (iResult == 0) {
            printf("Connection closing...\n");
        } else {
            printf("recv failed with error: %d\n", WSAGetLastError());
        }
    } while (iResult > 0);

    iResult = shutdown(clientSocket, SD_SEND);  // close sending channel
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    }

    closesocket(clientSocket);
    return 0;
}

int main()
{
    WSADATA wsdata;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsdata);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  // Use IPv4
    hints.ai_socktype = SOCK_STREAM;  // Sequenced, reliable, two-way connection
    hints.ai_protocol = IPPROTO_TCP;  // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;  // Intend to be used with bind

    printf("Obtaining the local address info...\n");
    iResult = getaddrinfo(NULL, SEC_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    printf("Creating a SOCKET for connection with server...\n");
    SOCKET serverSocket = INVALID_SOCKET;
    ptr = result;
    serverSocket = socket(ptr->ai_family,
            ptr->ai_socktype, ptr->ai_protocol);

    if (serverSocket == INVALID_SOCKET) {
        printf("cannot open socket: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(serverSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR){
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    ptr = NULL;

    iResult = listen(serverSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    HANDLE hand;
    bool sndConn = false;
    while (true) {
        SOCKET tmpSocket = INVALID_SOCKET;
        tmpSocket = accept(serverSocket, NULL, NULL);
        printf("Accept connection.\n");
        if (tmpSocket == INVALID_SOCKET) {
            printf("accept failed with error: %ld\n", WSAGetLastError());
        } else {
            if (sndConn) {
                WaitForSingleObject(hand, INFINITE);  // only handle a connection at a time
            }
            clientSocket = tmpSocket;
            hand = CreateThread(NULL, 0, handle, NULL, 0, NULL);  // start receiving data
            sndConn = true;
        }
    }
    // we should never reach here
    if (sndConn) {
        WaitForSingleObject(hand, INFINITE);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
