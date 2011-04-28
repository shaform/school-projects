TITLE   Big Integer (biginteger.asm)

INCLUDE Irvine32.inc
INCLUDE Macros.inc

;--------------------------------------------------------------------------------
mIs MACRO char:REQ
; Checks if the character is the given character.
;
; Receives: char, the character to check.
;           EDX, the offset to the string.
;
; Returns: Zero flag is set if matched.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        cmp     BYTE PTR [edx], char
ENDM

;--------------------------------------------------------------------------------
mIsDigit MACRO
; Checks if the character is a digital number.
;
; Receives: EDX, the offset to the character.
;
; Returns: Zero flag is set if it is valid.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        push    eax
        mov     al, BYTE PTR [edx]
        call    IsDigit
        pop     eax
ENDM

;--------------------------------------------------------------------------------
mNext MACRO
; Advances the pointer to the next char.
;
; Receives: EDX, the offset to the string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        inc     edx
ENDM

;--------------------------------------------------------------------------------
mSetSg MACRO ptI:REQ, sg:REQ
; Sets the sign of the BIGINTEGER.
;
; Receives: ptI, the offset to the BIGINTEGER.
;           sg, the sign.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    eax
        mov     eax, sg
        mov     (BIGINT PTR [ptI]).sign, eax
        pop     eax
ENDM

mIntS MACRO ptA, ptB
        push    eax
        mov     eax, (BIGINT PTR [ptA]).sign
        cmp     eax, (BIGINT PTR [ptB]).sign
        pop     eax
ENDM

.data


ISZ = 25
BSZ = 200

BIGINT STRUCT
        num     DWORD   ISZ DUP(0)
        sign    DWORD   0
BIGINT ENDS

PTBINT  TYPEDEF PTR BIGINT
buffer  BYTE    BSZ DUP(?)


.code

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
        sub     esi, TYPE DWORD
        sub     edi, TYPE DWORD
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

;--------------------------------------------------------------------------------
IntDiv PROC,
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
IntDiv ENDP

IntPrint PROC uses esi,
        ptInt:DWORD
        mov     esi, ptInt
        call    IntIsZero
        
        
        ret
IntPrint ENDP

;--------------------------------------------------------------------------------
IntRead PROC uses eax,
        ptI:PTBINT
; Reads in a BIGINTEGER.
; Receives: ptI, the offset to the BIGINTEGER.
;           EDX, the offset to the input string.
;
; Returns: The BIGINTEGER as ptI.
;          Carry flag is set if an error is detected.
;--------------------------------------------------------------------------------
        mSetSg  ptI, 0
        INVOKE  IntZero, ptI
        mIsDigit
        je      l_IR_D
        mIs     '+'
        je      l_IR_CK
        mIs     '-'
        je      l_IR_NEG
        stc                                                             ; Invalid character detected.
        ret
l_IR_NEG:
        mSetSg  ptI, 1
l_IR_CK:
        mNext
        mIsDigit
        je      l_IR_D
        stc                                                             ; Invalid character detected.
        ret

l_IR_D:
        ; ptI *= 10
        mov     al, [edx]
        ; ptI += al
        mNext
        mIsDigit
        je      l_IR_D

        ret
IntRead ENDP



main PROC
l_1:
        mWrite  "Input the expression to evaluate: "
        mov     edx, OFFSET buffer
        mov     ecx, LENGTHOF buffer
        call    ReadString                                              ; Reads in the expression.
        cmp     eax, 0
        je      l_RET                                                   ; Breaks if no input.
        call    Evaluate                                                ; Starts evaluation.
        jmp     l_1
l_RET:
        exit
main ENDP

END main

; vim: set et:
