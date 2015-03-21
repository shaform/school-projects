TITLE   Binary subtraction (bsub.asm)

INCLUDE Irvine32.inc
INCLUDE Macros.inc

ISZ = 85
BSZ = 400
STSZ = 200

; The Big Number Structure.
BIGINT STRUCT
        num     DWORD   ISZ DUP(0)
        sign    DWORD   0
BIGINT ENDS

PTBINT  TYPEDEF PTR BIGINT

;--------------------------------------------------------------------------------
mDebug MACRO string:REQ, integer:REQ
; Prints Debug string.
;
; Receives: string, the string to print.
;           integer, the integer to print.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    eax
        mWrite  string
        mov     eax, integer
        call    WriteDec
        call    Crlf
        pop     eax
ENDM

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


.data

; BIGINT holders.
; For IntDiv
bintbud BIGINT  <>
bintbue BIGINT  <>
; For IntMul
bintbuf BIGINT  <>
bintbug BIGINT  <>
; For IntDiv
bintbuh BIGINT  <>
; For other uses
bintbui BIGINT  <>
bintbuj BIGINT  <>
bintbuk BIGINT  <>
; For BinPrint
bintbuq BIGINT  <>
bintbur BIGINT  <>

bint1   BIGINT  <1 DUP(1)>
bint2   BIGINT  <1 DUP(2)>
buffer  BYTE    BSZ DUP(?)
strbuf  BYTE    BSZ DUP(?), 0
STREND = $ - (TYPE BYTE)



.code

;--------------------------------------------------------------------------------
SkipSpaces PROC
; Skips spaces in the string
;
; Receives: EDX, the offset to the string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
l_SS:
        cmp     BYTE PTR [edx],  ' '
        jne     l_SS_RET
        inc     edx
        jmp     l_SS

l_SS_RET:
        ret
SkipSpaces ENDP

;--------------------------------------------------------------------------------
IntS PROC uses eax ebx,
     ptA:PTBINT,
     ptB:PTBINT
; Checks if the signs of the BIGINTEGERs are equal.
;
; Receives: ptA, the offset to the first BIGINTEGER.
;           ptB, the offset to the second BIGINTEGER.
;
; Returns: Zero flag is set if equal.
;--------------------------------------------------------------------------------
        mov     eax, ptA
        mov     ebx, ptB
        mov     eax, (BIGINT PTR [eax]).sign
        mov     ebx, (BIGINT PTR [ebx]).sign
        cmp     eax, ebx
        ret
IntS ENDP

;--------------------------------------------------------------------------------
SetSg PROC uses eax edi,
      ptI:PTBINT,
      sg:DWORD
; Sets the sign of the BIGINTEGER.
;
; Receives: ptI, the offset to the BIGINTEGER.
;           sg, the sign.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        mov     eax, sg
        mov     edi, ptI
        mov     (BIGINT PTR [edi]).sign, eax
        ret
SetSg ENDP

;--------------------------------------------------------------------------------
IntCopy PROC uses esi edi ecx,
        ptD:PTBINT,
        ptS:PTBINT
; Copies a BIGINTEGER to another BIGINTEGER.
;
; Receives: ptD, the offset of the destination BIGINTEGER.
;           ptS, the offset of the source BIGINTEGER.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        mov     ecx, ISZ
        mov     edi, ptD
        mov     esi, ptS
        cld
        rep movsd
        mov     edi, ptD
        mov     esi, ptS
        mov     esi, (BIGINT PTR [esi]).sign
        mov     (BIGINT PTR [edi]).sign, esi
        ret
IntCopy ENDP

;--------------------------------------------------------------------------------
IntIsZero PROC uses ecx eax edi,
        ptI:PTBINT
; Checks if a BIGINT is zero.
; Receives: ptI, the offset to the BIGINTEGER.
;
; Returns: Zero flag is set if is zero.
;--------------------------------------------------------------------------------
        mov     edi, ptI
        mov     ecx, ISZ
        mov     eax, 0
        cld
        repe    scasd
        ret
IntIsZero ENDP

;--------------------------------------------------------------------------------
IntZero PROC uses eax edi ecx,
        ptI:PTBINT
; Zeros a BIGINT.
; Receives: ptI, the offset to the BIGINTEGER.
;
; Returns: The zeroed BIGINTGER as ptI.
;--------------------------------------------------------------------------------
        mov     edi, ptI
        mov     ecx, ISZ
        mov     eax, 0
        cld
        rep     stosd
        mov     edi, ptI
        mov     (BIGINT PTR [edi]).sign, 0
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
IntShL PROC uses esi ecx,
        ptI:PTBINT,
; Shifts a BIGINT left by 1 bit.
; Receives: ptI, the offset to the BIGINTEGER.
;
; Returns: The shifted BIGINT as ptI.
;--------------------------------------------------------------------------------
        mov     esi, ptI
        mov     ecx, ISZ * TYPE DWORD
        clc
l_ISL:
        rcl     BYTE PTR [esi], 1
        inc     esi
        loop    l_ISL
        ret
IntShL ENDP

;--------------------------------------------------------------------------------
IntShR PROC uses esi ecx,
        ptI:PTBINT,
; Shifts a BIGINT right by 1 bit.
; Receives: ptI, the offset to the BIGINTEGER.
;
; Returns: The shifted BIGINT as ptI.
;--------------------------------------------------------------------------------
        mov     esi, ptI
        add     esi, ISZ * TYPE DWORD - TYPE BYTE
        mov     ecx, ISZ * TYPE DWORD
        clc
l_ISR:
        rcr     BYTE PTR [esi], 1
        dec     esi
        loop    l_ISR
        ret
IntShR ENDP

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

        INVOKE  IntS, ptA, ptB
        mov     edi, ptC
        mov     ebx, ptA
        mov     edx, ptB
        mov     ecx, ISZ
        je      l_IB_SUB
        ; Signs are different. Add A, B directly to C
        call    IntAddI
        mov     edi, ptC
        mov     ebx, ptA
        mov     eax, (BIGINT PTR [ebx]).sign
        mov     (BIGINT PTR [edi]).sign, eax
        jmp     l_IB_RET
l_IB_SUB:
        INVOKE  IntCmp, ptA, ptB
        je      l_IB_ZERO                                               ; Sets result as zero if A==B
        mov     eax, (BIGINT PTR [ebx]).sign
        ja      l_IA_SUBB
        xor     eax, 1                                                  ; A < B, sets the sign of C as B's.
        xchg    ebx, edx
l_IA_SUBB:
        mov     (BIGINT PTR [edi]).sign, eax
        call    IntSubI
        jmp     l_IB_RET
l_IB_ZERO:
        INVOKE  IntZero, ptC
l_IB_RET:
        popad
        ret
IntSub ENDP

;--------------------------------------------------------------------------------
IntAddII PROC uses edx eax,
        ptI:PTBINT,
        num:BYTE
; Addes a BIGINT with a BYTE.
; Receives: ptI, the offset to the BIGINTEGER.
;           num, the BYTE.
;
; Returns: The added BIGINTEGER as ptI.
;--------------------------------------------------------------------------------
        clc
        mov     edx, ptI
        mov     al, num
        add     [edx], al
        mov     al, 0
l_IAII:
        jnc     l_IAII_RET
        inc     edx
        adc     [edx], al
        jc      l_IAII
l_IAII_RET:
        ret
IntAddII ENDP

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
BinPrint PROC,
        ptI:PTBINT
; Prints a binary BIGINTEGER.
; Receives: ptI, the offset to the BIGINTEGER.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        pushad
        INVOKE  IntCopy, OFFSET bintbuq, ptI
        INVOKE  SetSg, OFFSET bintbuq, 0
        mov     ebx, 2
        mov     esi, OFFSET bintbuq
        mov     edi, STREND

l_IP_P:


        dec     edi
        mov     eax, [esi]                                              ; Extracts the lowest digit.
        mov     edx, 0
        div     ebx
        add     dl, '0'                                                 ; Moves the lowest digit to output buffer.
        mov     [edi], dl
	INVOKE	IntSHR, OFFSET bintbuq

        INVOKE  IntIsZero, OFFSET bintbuq
        jne     l_IP_P

        mov     eax, ptI
        mov     eax, (BIGINT PTR [eax]).sign
        cmp     eax, 0
        je      l_IP_OUT
        dec     edi
        mov     BYTE PTR [edi], '-'
l_IP_OUT:
        mov     edx, edi
        call    WriteString
        call    Crlf
        popad
        ret
BinPrint ENDP

BinRead PROC uses eax,
        ptI:PTBINT

        mov     eax, ptI
        mov     eax, [eax]
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
        INVOKE  SetSg, ptI, 1
l_IR_CK:
        mNext
        mIsDigit
        je      l_IR_D
        stc                                                             ; Invalid character detected.
        ret

l_IR_D:
        INVOKE  IntShL, ptI
        mov     al, [edx]
        sub     al, '0'
        INVOKE  IntAddII, ptI, al
        mNext
        mIsDigit
        je      l_IR_D
        INVOKE  IntIsZero, ptI
        jne     @f
        INVOKE  SetSg, ptI, 0
@@:
        clc
        ret
BinRead ENDP


main PROC
        mov     edx, OFFSET buffer
        mov     ecx, LENGTHOF buffer
        call    ReadString                                              ; Reads in the binary number.
        cmp     eax, 0
        je      l_RET                                                   ; Breaks if no input.

	INVOKE	BinRead, OFFSET	bintbui
	jc	l_ERROR

        mov     edx, OFFSET buffer
        mov     ecx, LENGTHOF buffer
        call    ReadString                                              ; Reads in the binary number.
        cmp     eax, 0
        je      l_RET                                                   ; Breaks if no input.

	INVOKE	BinRead, OFFSET	bintbuj
	jc	l_ERROR


	INVOKE	IntSub, OFFSET bintbui, OFFSET bintbuj, OFFSET bintbuk
	INVOKE	BinPrint, OFFSET bintbuk
l_RET:
        exit
l_ERROR:
	mWrite	"ERROR DETECTED!!"
	exit
main ENDP

END main

; vim: set et:
