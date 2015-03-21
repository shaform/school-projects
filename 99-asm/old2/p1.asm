TITLE   Matrix operations (p1.asm)

INCLUDE Irvine32.inc
INCLUDE Macros.inc

SZ = 4
.data
matrixA DWORD   SZ*SZ DUP (?)
matrixB DWORD   SZ*SZ DUP (?)
matrixC DWORD   SZ*SZ DUP (?)
matrixT DWORD   SZ*SZ DUP (?)




.code

ReadMatrix PROC uses eax edi ecx,
           matrix:DWORD

        mov     edi, matrix
        mov     ecx, SZ*SZ
l_RM:
        call    ReadInt
        mov     [edi], eax
        add     edi, TYPE DWORD
        loop    l_RM

        ret
ReadMatrix ENDP

AddMatrix PROC uses eax ecx ebx edx edi
        mov     ebx, OFFSET matrixA
        mov     edx, OFFSET matrixB
        mov     edi, OFFSET matrixC
        mov     ecx, SZ*SZ
l_AM:
        mov     eax, [ebx]
        add     eax, [edx]
        mov     [edi], eax
        add     ebx, TYPE DWORD
        add     edx, TYPE DWORD
        add     edi, TYPE DWORD
        loop    l_AM

        ret
AddMatrix ENDP

MultiplyMatrix PROC uses eax ecx ebx edx edi esi
        mov     ebx, OFFSET matrixA
        mov     esi, OFFSET matrixB
        mov     edi, OFFSET matrixC
        mov     ecx, SZ
l_MM1:
        push    ebx
        push    esi

        push    ecx
        mov     ecx, SZ
l_MM2:
        mov     DWORD PTR[edi], 0
        push    ebx
        push    esi

        push    ecx
        mov     ecx, SZ
l_MM3:
        mov     eax, [ebx]
        imul    DWORD PTR [esi]
        add     [edi], eax

        add     ebx, TYPE DWORD
        add     esi, TYPE DWORD * SZ
        loop    l_MM3

        pop     ecx
        pop     esi
        pop     ebx
        add     esi, TYPE DWORD
        add     edi, TYPE DWORD
        loop    l_MM2
        pop     ecx
        pop     esi
        pop     ebx
        add     ebx, TYPE DWORD * SZ
        loop    l_MM1

        ret
MultiplyMatrix ENDP

PrintMatrix PROC uses eax ecx esi
        mov     ecx, SZ
        mov     esi, OFFSET matrixC
l_PM1:
        push    ecx
        mov     eax, [esi]
        call    WriteInt
        add     esi, TYPE DWORD

        mov     ecx, SZ-1
l_PM2:
        mWriteSpace     1
        mov     eax, [esi]
        call    WriteInt
        add     esi, TYPE DWORD
        loop    l_PM2

        pop     ecx

        call    Crlf
        loop    l_PM1

        ret
PrintMatrix ENDP

main PROC
        
        INVOKE  ReadMatrix, OFFSET matrixA
        INVOKE  ReadMatrix, OFFSET matrixB

        mWrite  "Added matrix:"
        call    Crlf
        call    AddMatrix
        call    PrintMatrix

        call    Crlf

        mWrite  "Multiplied matrix:"
        call    Crlf
        call    MultiplyMatrix
        call    PrintMatrix

        exit
main ENDP

END main


; vim: set et:
