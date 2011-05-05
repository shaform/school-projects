TITLE   Big Integer (biginteger.asm)

INCLUDE Irvine32.inc
INCLUDE Macros.inc

ISZ = 85
BSZ = 400
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
; For IntPrint
bintbup BIGINT  <>
bintbuq BIGINT  <>
bintbur BIGINT  <>

bint10  BIGINT  <1 DUP(10)>
buffer  BYTE    BSZ DUP(?)
strbuf  BYTE    BSZ DUP(?), 0
STREND = $ - (TYPE BYTE)


.code
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

IntCopy PROC uses esi edi ecx,
        ptD:PTBINT,
        ptS:PTBINT
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
        ; Add A, B directly to C
        call    IntAddI
        mov     edi, ptC
        mov     ebx, ptA
        mov     eax, (BIGINT PTR [ebx]).sign
        mov     (BIGINT PTR [edi]).sign, eax
        jmp     l_IB_RET
l_IB_SUB:
        INVOKE  IntCmp, ptA, ptB
        je      l_IB_ZERO
        mov     eax, (BIGINT PTR [ebx]).sign
        ja      l_IA_SUBB
        xor     eax, 1
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

        mov     edi, ptC
        mov     ebx, ptA
        mov     edx, ptB
        mov     ecx, ISZ
        INVOKE  IntS, ptA, ptB
        jne     l_IA_SUB
        ; Add A, B directly to C
        call    IntAddI
        mov     eax, ptA
        mov     eax, (BIGINT PTR [eax]).sign
        mov     edi, ptC
        mov     (BIGINT PTR [edi]).sign, eax
        jmp     l_IA_RET
l_IA_SUB:
        INVOKE  IntCmp, ptA, ptB
        je      l_IA_ZERO
        mov     eax, (BIGINT PTR [ebx]).sign
        ja      l_IA_SUBB
        xor     eax, 1
        xchg    ebx, edx
l_IA_SUBB:
        mov     (BIGINT PTR [edi]).sign, eax
        call    IntSubI
        jmp     l_IA_RET
l_IA_ZERO:
        INVOKE  IntZero, ptC
l_IA_RET:
        popad
        ret
IntAdd ENDP

;--------------------------------------------------------------------------------
IntMulII PROC,
        ptI:PTBINT,
        num:BYTE
; Multiply a BIGINT with a BYTE.
; Receives: ptI, the offset to the BIGINTEGER.
;           num, the BYTE.
;
; Returns: The multiplied BIGINTEGER as ptI.
;--------------------------------------------------------------------------------
        cmp     num, 0
        je      l_IMII_ZERO
        INVOKE  IntIsZero, ptI
        je      l_IMII_RET

        INVOKE  IntCopy, OFFSET bintbuf, ptI
        INVOKE  IntZero, ptI
l_IMII_M:
        shr     num, 1
        jnc     l_IMII_SK
        INVOKE  IntAdd, ptI, OFFSET bintbuf, ptI
l_IMII_SK:
        INVOKE  IntShL, OFFSET bintbuf
        cmp     num, 0
        jne     l_IMII_M
        jmp     l_IMII_RET
l_IMII_ZERO:
        INVOKE  IntZero, ptI

l_IMII_RET:
        ret
IntMulII ENDP

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
        INVOKE  IntS, ptA, ptB
        mov     esi, 0
        je      l_IM_POS
        mov     esi, 1
l_IM_POS:
        INVOKE  IntIsZero, ptA
        je      l_IM_ZERO
        INVOKE  IntIsZero, ptB
        je      l_IM_ZERO
        INVOKE  IntCopy, OFFSET bintbuf, ptA
        INVOKE  IntCopy, OFFSET bintbug, ptB
        INVOKE  IntZero, ptC


l_IM_M:
        INVOKE  IntShR, OFFSET bintbug
        jnc     l_IM_SK
        mov     edi, ptC
        mov     ebx, OFFSET bintbuf.num
        mov     edx, ptC
        mov     ecx, ISZ
        call    IntAddI
l_IM_SK:
        INVOKE  IntShL, OFFSET bintbuf
        INVOKE  IntIsZero, OFFSET bintbug
        jne     l_IM_M
        jmp     l_IM_RET
l_IM_ZERO:
        INVOKE  IntZero, ptC
        INVOKE  SetSg, ptC, 0
        popad
        ret
l_IM_RET:
        INVOKE  SetSg, ptC, esi
        popad
        ret
IntMul ENDP

;--------------------------------------------------------------------------------
IntDiv PROC,
        ptA:PTBINT,
        ptB:PTBINT,
        ptC:PTBINT
; Divides a BIGINT by a BIGINT.
; Receives: ptA, the offset to the first BIGINTEGER.
;           ptB, the offset to the second BIGINTEGER.
;           ptC, the offset to the destination BIGINTEGER.
;
; Returns: The divided BIGINTEGER as ptC.
;--------------------------------------------------------------------------------
        pushad
        mov     eax, ptA
        INVOKE  IntS, ptA, ptB
        mov     esi, 0
        je      l_ID_POS
        mov     esi, 1
l_ID_POS:
        INVOKE  IntIsZero, ptA
        je      l_ID_ZERO
        INVOKE  IntCmp, ptA, ptB
        jb      l_ID_ZERO
        INVOKE  IntIsZero, ptB
        je      l_ID_DIVZ
        INVOKE  IntCopy, OFFSET bintbud, ptA
        INVOKE  IntCopy, OFFSET bintbue, ptB
        INVOKE  SetSg, OFFSET bintbud, 0
        INVOKE  SetSg, OFFSET bintbue, 0
        INVOKE  IntZero, OFFSET bintbuh
        mov     bintbuh.num, 1
        INVOKE  IntZero, ptC
        clc
l_ID_M:
        INVOKE  IntCmp, OFFSET bintbud, OFFSET bintbue
        jb      l_ID_M2
        INVOKE  IntShL, OFFSET bintbuh
        INVOKE  IntShL, OFFSET bintbue
        jnc     l_ID_M
        jmp     l_ID_D
l_ID_M2:
        INVOKE  IntShR, OFFSET bintbuh
        INVOKE  IntShR, OFFSET bintbue
l_ID_D:
        INVOKE  IntAdd, ptC, OFFSET bintbuh, ptC
        INVOKE  IntSub, OFFSET bintbud, OFFSET bintbue, OFFSET bintbud
l_ID_CMP:
        INVOKE  IntShR, OFFSET bintbuh
        INVOKE  IntShR, OFFSET bintbue
        INVOKE  IntCmp, OFFSET bintbud, OFFSET bintbue
        jb      l_ID_CMP
        
        INVOKE  IntIsZero, OFFSET bintbuh
        jne     l_ID_D

        jmp     l_ID_RET
l_ID_DIVZ:
        stc
        jmp     l_ID_RET
l_ID_ZERO:
        INVOKE  IntZero, ptC
l_ID_RET:
        mov     eax, ptA
        INVOKE  SetSg, ptC, esi
        popad
        ret
IntDiv ENDP

;--------------------------------------------------------------------------------
IntPrint PROC,
        ptI:PTBINT
; Prints a BIGINTEGER.
; Receives: ptI, the offset to the BIGINTEGER.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        pushad
        INVOKE  IntCopy, OFFSET bintbuq, ptI
        INVOKE  SetSg, OFFSET bintbuq, 0
        mov     ebx, 10
        mov     esi, OFFSET bintbup
        mov     edi, STREND

l_IP_P:

        INVOKE  IntCopy, OFFSET bintbup, OFFSET bintbuq
        INVOKE  IntDiv, OFFSET bintbuq, OFFSET bint10, OFFSET bintbuq
        INVOKE  IntMul, OFFSET bintbuq, OFFSET bint10, OFFSET bintbur
        INVOKE  IntSub, OFFSET bintbup, OFFSET bintbur, OFFSET bintbup

        dec     edi
        mov     eax, [esi]                                              ; Extracts the lowest digit.
        mov     edx, 0
        div     ebx
        add     dl, '0'                                                 ; Moves the lowest digit to output buffer.
        mov     [edi], dl

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
        INVOKE  IntMulII, ptI, 10
        mov     al, [edx]
        sub     al, '0'
        INVOKE  IntAddII, ptI, al
        mNext
        mIsDigit
        je      l_IR_D

        ret
IntRead ENDP



;--------------------------------------------------------------------------------
Evaluate PROC
; Evaluates a BIGINTEGER expression.
; Receives: EDX, the offset to the input string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        pushad
        mov     edx, OFFSET buffer
        call    CheckPair
        jc      l_EV_RET
;		isopr = false;
;		op_clear();
        mOpClear
;		n_clear();
        mNClear
;		par_c = 0;

l_EV_LEX:
        call    Lex
        cmp     eax, NUM
        cmp     eax, ERROR
        cmp     eax, '('
        cmp     eax, ')'
        cmp     eax, ')'
        ; Operator found

        ; Final Evaulation
;		while (tk = lex()) {
;			switch (tk) {
;				case NUM:
;					n_push(tkd);
;					break;
;				case ERROR:
;					return true;
;				case '(':
;					op_push('(');
;					++par_c;
;					break;
;				case ')':
;					if (--par_c < 0) return true;
;
;					while (op_top() != '(') {
;						eval(op_top());
;						op_pop();
;					}
;					// pop '('
;					op_pop();
;					isopr = true;
;					break;
;				default:
;					while (!op_empty() && pred[op_top()] >= pred[tk] && tk != '^') {
;						eval(op_top());
;						op_pop();
;					}
;					op_push(tk);
;			}
;
;		}
;
;		if (par_c) return true;
;		else while (!op_empty()) {
;			eval(op_top());
;			op_pop();
;		}
;
;		res = n_top();
;		return false;
        

l_EV_RET:
        popad
        ret
Evaluate ENDP

main PROC
l_1:
        mWrite  "Input the expression to evaluate: "
        mov     edx, OFFSET buffer
        mov     ecx, LENGTHOF buffer
        call    ReadString                                              ; Reads in the expression.
        cmp     eax, 0
        je      l_RET                                                   ; Breaks if no input.
;       call    Evaluate                                                ; Starts evaluation.
        INVOKE  IntRead, OFFSET bintbui
        mov     edx, OFFSET buffer
        mov     ecx, LENGTHOF buffer
        call    ReadString                                              ; Reads in the expression.
        cmp     eax, 0
        je      l_RET                                                   ; Breaks if no input.
        INVOKE  IntRead, OFFSET bintbuj
        INVOKE  IntAdd, OFFSET bintbui, OFFSET bintbuj, OFFSET bintbuk
        mWrite  "Add: "
        INVOKE  IntPrint, OFFSET bintbuk
        call    Crlf
        INVOKE  IntSub, OFFSET bintbui, OFFSET bintbuj, OFFSET bintbuk
        mWrite  "Sub: "
        INVOKE  IntPrint, OFFSET bintbuk
        call    Crlf
        INVOKE  IntMul, OFFSET bintbui, OFFSET bintbuj, OFFSET bintbuk
        mWrite  "Mul: "
        INVOKE  IntPrint, OFFSET bintbuk
        call    Crlf
        INVOKE  IntDiv, OFFSET bintbui, OFFSET bintbuj, OFFSET bintbuk
        mWrite  "Div: "
        INVOKE  IntPrint, OFFSET bintbuk
        call    Crlf
        comment @
        mov     ecx, 3
l_2:
        mDebug  "Test : ", bintbui.num
        mDebug  "Test : ", [bintbui.num + TYPE DWORD]
        INVOKE  IntShL, OFFSET bintbui
        loop    l_2
        mDebug  "Read : ", bintbui.num
        mDebug  "Read : ", [bintbui.num + TYPE DWORD]
@
        INVOKE  IntPrint, OFFSET bintbui
        jmp     l_1
l_RET:
        exit
main ENDP

END main

; vim: set et:
