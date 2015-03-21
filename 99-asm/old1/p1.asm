TITLE   Prime number (p1.asm)

INCLUDE Irvine32.inc

MAX = 256
.data
Ptab     BYTE    MAX DUP(0)


.code
GenPrimes PROC
        mov     Ptab, 1
        mov     [Ptab + 1], 1
        mov     eax, 2
LGP:
        cmp     Ptab[eax], 1
        je      LGP3

        mov     ebx, eax
        add     ebx, eax
LGP2:
        cmp     ebx, MAX
        jae     LGP3
        mov     Ptab[ebx], 1
        add     ebx, eax
        jmp     LGP2
LGP3:
        
        
        inc     eax
        cmp     eax, MAX
        jb      LGP
        

        ret
GenPrimes ENDP

main PROC
        
        call    GenPrimes
        call    ReadDec
        movzx   esi, al
        call    ReadDec
        movzx   edi, al
        mov     ebx, 0

        cmp     esi, edi
        jb      L1
        xchg    esi, edi
L1:
        cmp     Ptab[esi], 0
        jne     NOTPRIME
        cmp     ebx, 0
        je      FIRST
        mov     al, ','
        call    WriteChar
        mov     al, ' '
        call    WriteChar
FIRST:
        inc     ebx
        mov     eax, esi
        call    WriteDec

NOTPRIME:
        inc     esi
        cmp     esi, edi
        jbe     L1

        exit
main ENDP

END main


; vim: set et:
