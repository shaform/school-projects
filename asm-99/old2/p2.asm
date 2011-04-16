TITLE   GCD (p2.asm)

INCLUDE Irvine32.inc

.code
Gcd PROC
        or      eax, eax
        jns     L1
        neg     eax
L1:
        or      ebx, ebx
        jns     L2
        neg     ebx
L2:
        mov     edx, 0
        div     ebx
        cmp     edx, 0
        jnz     REC_GCD
        ret
REC_GCD:
        mov     eax, ebx
        mov     ebx, edx
        call    Gcd
        ret
Gcd ENDP

main PROC
        
        call    ReadInt
        mov     ebx, eax
        call    ReadInt
        call    Gcd
        mov     eax, ebx
        call    WriteDec
        call    Crlf

        exit
main ENDP

END main


; vim: set et:
