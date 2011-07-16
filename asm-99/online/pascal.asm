TITLE   Pascal's triangel (pascal.asm)

INCLUDE Irvine32.inc
INCLUDE Macros.inc

SZ = 10000

.data
arr1    DWORD   SZ DUP(?)
arr2    DWORD   SZ DUP(?)
rsz     DWORD   ?

.code

Prints PROC uses ecx esi eax
        mov     esi, OFFSET arr1
        mov     ecx, rsz
l_P:
        mov     eax, DWORD PTR [esi]
        call    WriteDec
        mWriteSpace 1
        add     esi, TYPE DWORD
        loop    l_P



        call    Crlf
        ret
Prints ENDP

Generates PROC uses eax ecx
        mov     ecx, rsz
        inc     ecx
        mov     rsz, ecx

        mov     esi, OFFSET arr1
        mov     edi, OFFSET arr2
        mov     DWORD PTR [edi], 1
        add     edi, TYPE DWORD

        sub     ecx, 2
        jz      l_G1e

        mov     eax, rsz
l_G:
        mov     eax, DWORD PTR [esi]
        mov     DWORD PTR [edi], eax
        add     esi, TYPE DWORD
        mov     eax, DWORD PTR [esi]
        add     DWORD PTR [edi], eax
        add     edi, TYPE DWORD
        loop    l_G
l_G1e:
        mov     DWORD PTR [edi], 1



        mov     ecx, rsz
        mov     esi, OFFSET arr2
        mov     edi, OFFSET arr1

l_G2:
        mov     eax, DWORD PTR [esi]
        mov     DWORD PTR [edi], eax
        add     esi, TYPE DWORD
        add     edi, TYPE DWORD
        loop    l_G2

        ret
Generates ENDP

main PROC

L1:
        call    ReadDec
        cmp     eax, 0
        jz      L3

        mov     ecx, eax
        mov     rsz, 1
        mov     arr1, 1
L2:
        call    Prints
        call    Generates
        loop    L2

        jmp     L1
L3:
        exit
main ENDP

END main

; vim: set et:
