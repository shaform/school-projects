TITLE   Big Integer (biginteger.asm)

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
; For IntPrint
bintbuq BIGINT  <>
bintbur BIGINT  <>
; For Lex
tkd     BIGINT  <>
; For Eval
bintbux BIGINT  <>
bintbuy BIGINT  <>
bintbuz BIGINT  <>

bint10  BIGINT  <1 DUP(10)>
bint1   BIGINT  <1 DUP(1)>
bint2   BIGINT  <1 DUP(2)>
buffer  BYTE    BSZ DUP(?)
strbuf  BYTE    BSZ DUP(?), 0
STREND = $ - (TYPE BYTE)

; The precedence of operators for evaluation.
; 1 : ()
; 2 : + -
; 3 : * / %
; 4 : ^
pred    BYTE    8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        BYTE    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        BYTE    0,0,0,0,0,3,0,0,1,1,3,2,0,2,0,3
        BYTE    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        BYTE    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        BYTE    0,0,0,0,0,0,0,0,0,0,0,0,0,0,4

; Stacks
st_op   BYTE    STSZ DUP(?)
st_num  BIGINT  STSZ DUP(<>)
op_top  SDWORD  -1
num_top SDWORD  -1

isopr   BYTE    0

;--------------------------------------------------------------------------------
mTopOp MACRO op:REQ
; Gets the top of the operator stack.
;
; Receives: op, where to put the operator.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    edx
        mov     edx, op_top
        add     edx, OFFSET st_op
        mov     op, BYTE PTR [edx]
        pop     edx
ENDM
;--------------------------------------------------------------------------------
mPopOp MACRO
; Pops a operator.
;
; Receives: Nothing.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        dec     op_top
ENDM

;--------------------------------------------------------------------------------
mPushOp MACRO op:REQ
; Pushs a operator.
;
; Receives: op, the operator to push.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    edx
        inc     op_top
        mov     edx, op_top
        mov     BYTE PTR st_op[edx], op
        pop     edx
ENDM

;--------------------------------------------------------------------------------
mIsOpEmpty MACRO
; Checks if the op stack is empty.
;
; Receives: Nothing.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        cmp     op_top, -1
ENDM

;--------------------------------------------------------------------------------
mOpIs MACRO char:REQ
; Checks if the top of op stack is char.
;
; Receives: char, the char to check.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    edx
        mov     edx, op_top
        mov     dl, st_op[edx]
        cmp     dl, char
        pop     edx
ENDM

;--------------------------------------------------------------------------------
mTopN MACRO N:REQ
; Gets the top of the operand stack.
;
; Receives: op, where to put the operator.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        pushad
        mov     eax, num_top
        mov     ebx, TYPE BIGINT
        mul     ebx
        add     eax, OFFSET st_num
        mov     edi, N
        INVOKE  IntCopy, edi, eax
        popad
ENDM

;--------------------------------------------------------------------------------
mPopN MACRO
; Pops a operand.
;
; Receives: Nothing.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        dec     num_top
ENDM

;--------------------------------------------------------------------------------
mPushN MACRO ptInt:REQ
; Pushs a BIGINTEGR.
;
; Receives: ptInt, the offset of the BIGINTEGER to push.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    eax
        push    ebx
        push    edx
        inc     num_top
        mov     eax, num_top
        mov     ebx, TYPE BIGINT
        mul     ebx
        add     eax, OFFSET st_num
        mov     ebx, ptInt
        INVOKE  IntCopy, eax, ebx
        pop     edx
        pop     ebx
        pop     eax
ENDM


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
        popad
        ret
l_ID_ZERO:
        INVOKE  IntCopy, OFFSET bintbud, ptA
        INVOKE  IntZero, ptC
l_ID_RET:
        mov     eax, ptA
        INVOKE  SetSg, ptC, esi
        INVOKE  IntIsZero, OFFSET bintbud
        jne @F
        INVOKE  SetSg, OFFSET bintbud, 0
@@:
        popad
        clc
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
        mov     esi, OFFSET bintbud
        mov     edi, STREND

l_IP_P:

        INVOKE  IntDiv, OFFSET bintbuq, OFFSET bint10, OFFSET bintbuq

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
        INVOKE  IntIsZero, ptI
        jne     @f
        INVOKE  SetSg, ptI, 0
@@:
        ret
IntRead ENDP



;--------------------------------------------------------------------------------
CheckPair PROC uses edx eax
; Checks if the parentheses are correctly paired in the input.
; Receives: EDX, the offset to the input string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        mov     eax, 0
l_CP:
        mIs     0
        je      l_CP_RET
        mIs     '('
        jne     @F
        inc     eax
        jmp     l_CP@
@@:
        mIs     ')'
        jne     l_CP@
        dec     eax
        cmp     eax, 0
        jl      l_CP_ERR
l_CP@:
        mNext
        jmp     l_CP

l_CP_ERR:
        stc
        ret
l_CP_RET:
        clc
        ret
CheckPair ENDP
LX_END = 0
LX_NUM = 256
LX_ERROR = 257

;--------------------------------------------------------------------------------
Lex PROC uses ebx esi
; Lexer.
; Receives: EDX, the offset to the input string.
;
; Returns: The token as EAX.
;--------------------------------------------------------------------------------
        call    SkipSpaces
        mov     al, [edx]
        movzx   si, al
        mov     bl, pred[si]


        cmp     isopr, 0
        je      @F
        mov     isopr, 0
        cmp     bl, 0
        je      l_LX_ERR
        movzx   eax, al
        jmp     l_LX_RET
@@:
        mIs     '('
        jne     @F
        mov     eax, '('
        jmp     l_LX_RET
@@:
        cmp     al, '-'
        je      @F
        cmp     al, '+'
        je      @F
        cmp     bl, 0
        je      @F
        jmp     l_LX_ERR
@@:
        mov     esi, edx
        INVOKE  IntRead, OFFSET tkd
        cmp     esi, edx
        jne     @F
        jmp     l_LX_ERR
@@:
        mov     isopr, 1
        mov     eax, LX_NUM
        ret
l_LX_ERR:
        mov     eax, LX_ERROR
l_LX_RET:
        mNext
        ret
Lex ENDP

;--------------------------------------------------------------------------------
Eval PROC uses ebx ecx
; Evaluates two BIGINTEGERs in the stack.
; Receives: Nothing.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------

; Get the two operands, assuming that they exist.
        mTopN   OFFSET bintbuy
        mPopN
        mTopN   OFFSET bintbux
        mPopN
        mTopOp  bl

        cmp     bl, '+'
        jne     @F
        INVOKE  IntAdd, OFFSET bintbux, OFFSET bintbuy, OFFSET bintbuz
        jmp     l_E_RET
@@:
        cmp     bl, '-'
        jne     @F
        INVOKE  IntSub, OFFSET bintbux, OFFSET bintbuy, OFFSET bintbuz
        jmp     l_E_RET
@@:
        cmp     bl, '*'
        jne     @F
        INVOKE  IntMul, OFFSET bintbux, OFFSET bintbuy, OFFSET bintbuz
        jmp     l_E_RET
@@:
        cmp     bl, '/'
        jne     @F
        INVOKE  IntDiv, OFFSET bintbux, OFFSET bintbuy, OFFSET bintbuz
        jmp     l_E_RET
@@:
        cmp     bl, '%'
        jne     @F
        INVOKE  IntDiv, OFFSET bintbux, OFFSET bintbuy, OFFSET bintbuz
        jc      l_E_RET
        INVOKE  IntCopy, OFFSET bintbuz, OFFSET bintbud
        jmp     l_E_RET
@@:
        ; The '^' operator.
        INVOKE  IntZero, OFFSET bintbuz
        INVOKE  IntIsZero, OFFSET bintbux
        je      l_E_RET
        mov     DWORD PTR [bintbuz], 1
        INVOKE  IntIsZero, OFFSET bintbuy
        je      l_E_RET                                                 ; x^0 = 1
        INVOKE  IntS, OFFSET bint10, OFFSET bintbuy
        jne     @F
l_E_P:
        ; y is positive.
        INVOKE  IntMul, OFFSET bintbuz, OFFSET bintbux, OFFSET bintbuz
        INVOKE  IntSub, OFFSET bintbuy, OFFSET bint1, OFFSET bintbuy
        INVOKE  IntIsZero, OFFSET bintbuy
        jne     L_E_P
        jmp     l_E_RET
@@:
        ; y is negative.
        INVOKE  IntCmp, OFFSET bintbux, OFFSET bint1
        ja      @F                                                      ; when x>1, x^-? = 0
        INVOKE  IntDiv, OFFSET bintbuy, OFFSET bint2, OFFSET bintbuy
        cmp     DWORD PTR [bintbud], 0
        je      l_E_RET                                                 ; when y is odd, the sign of z is +
        INVOKE  SetSg, OFFSET bintbuz, 1
        jmp     l_E_RET
@@:
        INVOKE  IntZero, OFFSET bintbuz
l_E_RET:
        pushfd
        mPushN  OFFSET bintbuz
        popfd
        ret
Eval ENDP

;--------------------------------------------------------------------------------
Evaluate PROC
; Evaluates a BIGINTEGER expression.
; Receives: EDX, the offset to the input string.
;
; Returns: The result as bintbuz.
;--------------------------------------------------------------------------------
        pushad
        mov     edx, OFFSET buffer
        call    CheckPair
        jc      l_EV_CP
        mov     isopr, 0

        ; Clears both stacks.
        mov     op_top, -1
        mov     num_top, -1

l_EV_LEX:
        call    Lex

        ; Checks if it's the end.
        cmp     eax, 0
        je      l_EV_DONE

        ; Checks if it's the number.
        cmp     eax, LX_NUM
        jne     @F
        mPushN  OFFSET tkd
        jmp     l_EV_LEX

@@:
        ; Checks if it's the error.
        cmp     eax, LX_ERROR
        jne     @F
        jmp     l_EV_ERR
@@:
        cmp     eax, '('
        jne     @F
        mPushOp '('
        jmp     l_EV_LEX
@@:
        cmp     eax, ')'
        jne     @F

l_EV_EV:
        mOpIs   '('
        je      l_EV_EV@
        call    Eval
        jc      l_EV_DZ
        mPopOp
        jmp     l_EV_EV
l_EV_EV@:
        mPopOp
        mov     isopr, 1
        jmp     l_EV_LEX
@@:
        ; Operator found
        mIsOpEmpty
        je      @F
        mTopOp  bl
        movzx   si, bl
        mov     bl, pred[si]
        mov     cl, pred[eax]
        cmp     bl, cl
        jb      @F
        cmp     eax, '^'
        je      @F
        call    Eval
        jc      l_EV_DZ
        mPopOp
@@:
        mPushOp al
        jmp     l_EV_LEX
        
        ; Evaluates remaing operands.
l_EV_DONE:
        mIsOpEmpty
        je      l_EV_RET
        call    Eval
        jc      l_EV_DZ
        mPopOp
        jmp     l_EV_DONE
l_EV_DZ:
        mWrite  "Error: divide by zero."
        call    Crlf
        stc
        jmp     l_EV_RET
l_EV_CP:
        mWrite  "Error: mismatched parentheses."
        call    Crlf
        stc
        jmp     l_EV_RET
l_EV_ERR:
        mWrite  "Error: syntax error."
        call    Crlf
        stc
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
        call    Evaluate                                                ; Starts evaluation.
        jc      l_1
        
        comment / The test suite for BIGINT.
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
/
        mTopN   OFFSET bintbuz
        INVOKE  IntPrint, OFFSET bintbuz
        jmp     l_1
l_RET:
        exit
main ENDP

END main

; vim: set et:
