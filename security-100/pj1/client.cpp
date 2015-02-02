// Version of window, 0x501 means WINXP
#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
using namespace std;

const int DEFAULT_BUFLEN = 512; // read buffer size
const int SEC_MSG_LEN = 7; // length of message to send
const char *SEC_IP = "140.113.216.151";  // server IP
const char *SEC_PORT = "2000";  // server port
const char SEC_MSG[SEC_MSG_LEN+1] = "0000000";  // message to send

#pragma comment(lib,"Ws2_32.lib")

// the socket to connect with server
SOCKET clientSocket;

DWORD WINAPI sec_send(LPVOID lpParam)
{
    printf("Sending data to server...\n");
    int iResult;
    iResult = send(clientSocket, SEC_MSG, SEC_MSG_LEN, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Shuting down sending connection...\n");
    iResult = shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown sending failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        return 1;
    }
    return 0;
}

DWORD WINAPI sec_recv(LPVOID lpParam)
{
    char recvbuf[DEFAULT_BUFLEN+1];

    int iResult;
    do {
        iResult = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("%d bytes received: %s\n", iResult, recvbuf);
        } else if (iResult == 0 || WSAGetLastError() == WSAECONNRESET)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);

    return 0;
}
int main()
{
    printf("Initializing Winsock...\n");
    WSADATA wsdata;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsdata);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // Sequenced, reliable, two-way connection
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    
    printf("Obtaining the server address...\n");
    iResult = getaddrinfo(SEC_IP, SEC_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    printf("Creating a SOCKET for connection with server...\n");
    clientSocket = INVALID_SOCKET;
    ptr = result;
    clientSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

    if (clientSocket == INVALID_SOCKET) {
        printf("cannot open socket: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    printf("Connecting to the server...\n");
    iResult = connect(clientSocket, ptr->ai_addr,
            (int)ptr->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);
    ptr = NULL;

    if (clientSocket == INVALID_SOCKET) {
        printf("Unable to connect to the server!\n");
        WSACleanup();
        return 1;
    }

    printf("Creating threads...\n");
    HANDLE threads[2];
    threads[0] = CreateThread(NULL, 0, sec_send, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, sec_recv, NULL, 0, NULL);

    WaitForMultipleObjects(2, threads, true, INFINITE);

    printf("Closing...\n");
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
