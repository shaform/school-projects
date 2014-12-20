TITLE   mergeSort (p3.asm)

INCLUDE Irvine32.inc

.data
arrayA  DWORD   2,20,8,12,16,4,10,18,6,14
arrayB  DWORD   3,5,19,11,15,7,1,9,17,13

MAXL = LENGTHOF arrayA + LENGTHOF arrayB

arrayC  DWORD   MAXL DUP(5)

.code


mergeSort PROC uses eax ebx ecx edx edi esi,
          arrA:DWORD,
          M:DWORD,
          arrB:DWORD,
          N:DWORD,
          arrC:DWORD

        mov     eax, 0
        mov     ebx, 0
        mov     ecx, arrA
        mov     edx, arrB
        mov     edi, arrC

LM1:
        cmp     eax, M
        jae     LM2
        cmp     ebx, N
        jae     LM2
        mov     esi, [ecx]
        cmp     esi, [edx]
        ja      LM1A

        mov     [edi], esi
        add     edi, TYPE DWORD
        add     ecx, TYPE DWORD
        inc     eax
        jmp     LM1
LM1A:
        mov     esi, [edx]
        mov     [edi], esi
        add     edi, TYPE DWORD
        add     edx, TYPE DWORD
        inc     ebx
        jmp     LM1

LM2:
        cmp     eax, M
        jae     LM3
        mov     esi, [ecx]
        mov     [edi], esi
        add     edi, TYPE DWORD
        add     ecx, TYPE DWORD
        inc     eax
        jmp     LM2
LM3:
        cmp     ebx, N
        jae     LMRET
        mov     esi, [edx]
        mov     [edi], esi
        add     edi, TYPE DWORD
        add     edx, TYPE DWORD
        inc     ebx
        jmp     LM2
LMRET:
        ret
mergeSort ENDP

Sort PROC uses eax ebx ecx edx esi,
     arr:DWORD,
     N:DWORD

        LOCAL   arrC[MAXL]:DWORD

        cmp     N, 1
        jbe     LSRET

        mov     ecx, N
        shr     ecx, 1
        INVOKE  Sort, arr, ecx
        mov     eax, TYPE DWORD
        mul     ecx
        mov     esi, eax
        add     esi, arr

        mov     ebx, N
        add     ebx, 1
        shr     ebx, 1
        INVOKE  Sort, esi, ebx

        lea     eax, arrC
        INVOKE  mergeSort, arr, ecx, esi, ebx, eax

        mov     esi, eax
        mov     ecx, N
        mov     eax, arr
LS:
        mov     ebx, [esi]
        mov     [eax], ebx
        add     eax, TYPE DWORD
        add     esi, TYPE DWORD
        loop LS
LSRET: 
        ret
Sort ENDP

main PROC
        INVOKE  Sort, OFFSET arrayA, LENGTHOF arrayA
        INVOKE  Sort, OFFSET arrayB, LENGTHOF arrayB
        INVOKE  mergeSort, OFFSET arrayA, LENGTHOF arrayA,
                           OFFSET arrayB, LENGTHOF arrayB,
                           OFFSET arrayC
        mov     ecx, LENGTHOF arrayA
        add     ecx, LENGTHOF arrayB
        mov     esi, 0
L1:
        mov     eax, arrayC[esi]
        call    WriteDec
        call    Crlf
        add     esi, TYPE DWORD
        loop L1

        exit
main ENDP

END main


; vim: set et:
