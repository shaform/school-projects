TITLE   Big Integer (biginteger.asm)

INCLUDE Irvine32.inc
INCLUDE Macros.inc

.data

ISZ = 20

BIGINT STRUCT
        num     DWORD   ISZ DUP(0)
        sign    DWORD   0
BIGINT ENDS

PTBINT  TYPEDEF PTR BIGINT


.code
mIntS MACRO ptA, ptB
        push    eax
        mov     eax, (BIGINT PTR [ptA]).sign
        cmp     eax, (BIGINT PTR [ptB]).sign
        pop     eax
ENDM

IntIsZero PROC uses ecx eax
        mov     ecx, ISZ
        mov     eax, 0
        cld
        repe    scasd
        ret
IntIsZero ENDP

;--------------------------------------------------------------------------------
IntZero PROC uses eax edi ecx,
        ptI:PTBINT
; Zeros a BIGINTs.
; Receives: ptI, the offset to the BIGINTEGER.
;
; Returns: The zeroed BIGINTGER as ptI.
;--------------------------------------------------------------------------------
        mov     edi, (BIGINT PTR [ptI]).num
        mov     ecx, ISZ
        mov     eax, 0
        cld
        rep     stosd
        mov     (BIGINT PTR [ptI]).sign, 0
        ret
IntZero ENDP

;--------------------------------------------------------------------------------
IntCmp PROC uses esi edi ecx,
        ptA:PTBINT,
        ptB:PTBINT
; Compares two BIGINTs (ignores their signs).
; Receives: ptA, the offset to the first BIGINTEGER.
;           ptB, the offset to the second BIGINTEGER.
;
; Returns: Zero flag = 0 if they are equal.
;--------------------------------------------------------------------------------
        mov     ecx, ISZ
        mov     esi, ptA
        mov     edi, ptB
        add     esi, TYPE DWORD * ISZ
        add     edi, TYPE DWORD * ISZ
        dec     esi
        dec     edi
        std
        repe    cmpsd
        ret
IntCmp ENDP

;--------------------------------------------------------------------------------
IntSubI PROC
; Substracts two BIGINTs (internal version).
; Receives: EBX, the offset to the first BIGINTEGER's num.
;           EDX, the offset to the second BIGINTEGER's num.
;           EDI, the offset to the destination BIGINTEGER's num.
;           ECX, the length of the num.
;
; Returns: The substracted BIGINTEGER as EDI.
;--------------------------------------------------------------------------------
        clc
l_IBI:
        mov     eax, [ebx]
        sbb     eax, [edx]
        pushfd
        mov     [edi], eax

        add     ebx, TYPE DWORD
        add     edx, TYPE DWORD
        add     edi, TYPE DWORD

        popfd
        loop    l_IBI
        ret
IntSubI ENDP

;--------------------------------------------------------------------------------
IntSub PROC,
        ptA:PTBINT,
        ptB:PTBINT,
        ptC:PTBINT
; Substracts two BIGINTs.
; Receives: ptA, the offset to the first BIGINTEGER.
;           ptB, the offset to the second BIGINTEGER.
;           ptC, the offset to the destination BIGINTEGER.
;
; Returns: The substracted BIGINTEGER as ptC.
;--------------------------------------------------------------------------------
        pushad

        mIntS   ptA, ptB
        mov     edi, (BIGINT PTR [ptC]).num
        mov     ebx, (BIGINT PTR [ptA]).num
        mov     edx, (BIGINT PTR [ptB]).num
        mov     ecx, ISZ
        je      l_IB_SUB
        ; Add A, B directly to C
        call    IntAddI
        mov     eax, (BIGINT PTR [ptA]).sign
        mov     (BIGINT PTR [ptC]).sign, eax
        jmp     l_IB_RET
l_IB_SUB:
        INVOKE  IntCmp, ptA, ptB
        je      l_IB_ZERO
        mov     eax, (BIGINT PTR [ptA]).sign
        ja      l_IA_SUBB
        xor     eax, 1
        xchg    ebx, edx
l_IA_SUBB:
        mov     (BIGINT PTR [ptC]).sign, eax
        call    IntSubI
        jmp     l_IB_RET
l_IB_ZERO:
        INVOKE  IntZero, ptC
l_IB_RET:
        popad
        ret
IntSub ENDP

;--------------------------------------------------------------------------------
IntAddI PROC
; Addes two BIGINTs (internal version).
; Receives: EBX, the offset to the first BIGINTEGER's num.
;           EDX, the offset to the second BIGINTEGER's num.
;           EDI, the offset to the destination BIGINTEGER's num.
;           ECX, the length of the num.
;
; Returns: The added BIGINTEGER as EDI.
;--------------------------------------------------------------------------------
        clc
l_IAI:
        mov     eax, [ebx]
        adc     eax, [edx]
        pushfd
        mov     [edi], eax

        add     ebx, TYPE DWORD
        add     edx, TYPE DWORD
        add     edi, TYPE DWORD

        popfd
        loop    l_IAI
        ret
IntAddI ENDP

;--------------------------------------------------------------------------------
IntAdd PROC,
        ptA:PTBINT,
        ptB:PTBINT,
        ptC:PTBINT
; Addes two BIGINTs.
; Receives: ptA, the offset to the first BIGINTEGER.
;           ptB, the offset to the second BIGINTEGER.
;           ptC, the offset to the destination BIGINTEGER.
;
; Returns: The added BIGINTEGER as ptC.
;--------------------------------------------------------------------------------
        pushad

        mIntS   ptA, ptB
        mov     edi, (BIGINT PTR [ptC]).num
        mov     ebx, (BIGINT PTR [ptA]).num
        mov     edx, (BIGINT PTR [ptB]).num
        mov     ecx, ISZ
        jne     l_IA_SUB
        ; Add A, B directly to C
        call    IntAddI
        mov     eax, (BIGINT PTR [ptA]).sign
        mov     (BIGINT PTR [ptC]).sign, eax
        jmp     l_IA_RET
l_IA_SUB:
        INVOKE  IntCmp, ptA, ptB
        je      l_IA_ZERO
        mov     eax, (BIGINT PTR [ptA]).sign
        ja      l_IA_SUBB
        xor     eax, 1
        xchg    ebx, edx
l_IA_SUBB:
        mov     (BIGINT PTR [ptC]).sign, eax
        call    IntSubI
        jmp     l_IA_RET
l_IA_ZERO:
        INVOKE  IntZero, ptC
l_IA_RET:
        popad
        ret
IntAdd ENDP

;--------------------------------------------------------------------------------
IntMul PROC,
        ptA:PTBINT,
        ptB:PTBINT,
        ptC:PTBINT
; Multiply two BIGINTs.
; Receives: ptA, the offset to the first BIGINTEGER.
;           ptB, the offset to the second BIGINTEGER.
;           ptC, the offset to the destination BIGINTEGER.
;
; Returns: The multiplied BIGINTEGER as ptC.
;--------------------------------------------------------------------------------
        pushad

        mIntS   ptA, ptB
        mov     edi, (BIGINT PTR [ptC]).num
        mov     ebx, (BIGINT PTR [ptA]).num
        mov     edx, (BIGINT PTR [ptB]).num
        mov     ecx, ISZ
        jne     l_IA_SUB
        ; Add A, B directly to C
        call    IntAddI
        mov     eax, (BIGINT PTR [ptA]).sign
        mov     (BIGINT PTR [ptC]).sign, eax
        jmp     l_IA_RET
l_IA_SUB:
        INVOKE  IntCmp, ptA, ptB
        je      l_IA_ZERO
        mov     eax, (BIGINT PTR [ptA]).sign
        ja      l_IA_SUBB
        xor     eax, 1
        xchg    ebx, edx
l_IA_SUBB:
        mov     (BIGINT PTR [ptC]).sign, eax
        call    IntSubI
        jmp     l_IA_RET
l_IA_ZERO:
        mov     DWORD PTR [edi], 0
        loop    l_IA_ZERO
        mov     (BIGINT PTR [ptC]).sign, 0

l_IA_RET:
        popad
        ret
IntMul ENDP

IntPrint PROC,
        ptInt:DWORD
        mov     edi, ptInt
        call    IntIsZero
        
        
        ret
IntPrint ENDP



main PROC
        exit
main ENDP

END main

; vim: set et:
