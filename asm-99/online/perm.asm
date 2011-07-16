TITLE   Permutation (perm.asm)

INCLUDE Irvine32.inc
INCLUDE Macros.inc

.data
arr     BYTE    4 DUP(?)
arrback BYTE    ?
arrend  BYTE    ?

.code
main PROC
        mov     ecx, 6
        mov     edx, OFFSET arr
        call    ReadString

        mov     edx, OFFSET arrback
        mov     edi, OFFSET arrback
        mov     ecx, 4
L1:
        mov     al, BYTE PTR [edx]
        dec     edi
        cmp     BYTE PTR [edi], al
        jb      LG
        dec     edx

        loop    L1
        jmp     L2
LG:
        xchg    BYTE PTR [edi], al
        xchg    BYTE PTR [edx], al
L2:
        mov     edx, OFFSET arr
        call    WriteString
        exit
main ENDP

END main

; vim: set et:
