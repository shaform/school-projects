TITLE   Finite State Machine to Read Assembly Code (fsm.asm)

INCLUDE Irvine32.inc

MAX = 1000                                               ; Maximum size for input buffer.
ADDON = 1                                                ; Special check for syntax error.

.data
PROMPT          BYTE    "Input the assembly source code=> ", 0

STRLABEL        BYTE    "LABEL             ", 0
STRINC          BYTE    "INSTRUCTION       ", 0
STRREG          BYTE    "REGISTER          ", 0
STRID           BYTE    "IDENTIFIER        ", 0
STRCOMMENT      BYTE    "COMMENT           ", 0
STRINTEGER      BYTE    "INTEGER CONSTANT  ", 0
STRUNKNOWN      BYTE    "UNKNOWN           ", 0

STRERRMLB       BYTE    "[Error] Multiple labels detected.", 0
STRERRID        BYTE    "[Error] Reserved word cannot name a identifier.", 0
STRERRINC       BYTE    "[Error] Invalid instruction.", 0
STRERRIC        BYTE    "[Error] Invalid character.", 0
STRERRNOOPER    BYTE    "[Error] Missing operand.", 0
STRERROPER      BYTE    "[Error] Invalid operand.", 0
STRERRDIGIT     BYTE    "[Error] Invalid integer representation.", 0
STRERRNUM       BYTE    "[Error] Wrong number of operands.", 0
IFDEF ADDON
RESERVED        BYTE    "mov", 5 DUP(0),"movsx", 3 DUP(0),"add", 5 DUP(0),"sub", 5 DUP(0),"mul", 5 DUP(0)
                BYTE    "div", 5 DUP(0),"inc", 5 DUP(0),"dec", 5 DUP(0),"jmp", 5 DUP(0),"loop", 4 DUP(0)
                BYTE    "ret", 5 DUP(0),"eax", 5 DUP(0),"ebx", 5 DUP(0),"ecx", 5 DUP(0),"edx", 5 DUP(0)
                BYTE    "esi", 5 DUP(0),"edi", 5 DUP(0),"ebp", 5 DUP(0),"esp", 5 DUP(0)
                BYTE    "ax", 6 DUP(0),"ah", 6 DUP(0),"al", 6 DUP(0)
                BYTE    "bx", 6 DUP(0),"bh", 6 DUP(0),"bl", 6 DUP(0)
                BYTE    "cx", 6 DUP(0),"ch", 6 DUP(0),"cl", 6 DUP(0)
                BYTE    "dx", 6 DUP(0),"dh", 6 DUP(0),"dl", 6 DUP(0)
                BYTE    "si", 6 DUP(0),"di", 6 DUP(0),"bp", 6 DUP(0),"sp", 6 DUP(0)
                BYTE    "cs", 6 DUP(0),"ds", 6 DUP(0),"ss", 6 DUP(0),"es", 6 DUP(0),"fs", 6 DUP(0),"gs", 6 DUP(0)
                BYTE    "comment",0
SIZER = TYPE BYTE * 8
LENR = ($-RESERVED)/SIZER

ENDIF

src             BYTE    MAX DUP(0), 0
buffer          BYTE    MAX DUP(0), 0

.code


; main program to process input.
main PROC
L1:
        mov     edx, OFFSET PROMPT
        call    WriteString
        mov     edx, OFFSET src
        mov     ecx, LENGTHOF src
        call    ReadString
        cmp     eax, 0                                   ; Breaks if there is no input.
        je      DONE
        call    FSM

        jmp     L1
DONE:
        exit
main ENDP

;--------------------------------------------------------------------------------
mPuts MACRO pts
; Prints a string to the terminal.
;
; Receives: pts, the offset to the string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    edx
        mov     edx, pts
        call    WriteString
        pop     edx
ENDM

;--------------------------------------------------------------------------------
mNext MACRO
; Advance the pointer to the next char.
;
; Receives: EDX, the offset to the string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        inc     edx
ENDM

;--------------------------------------------------------------------------------
mEnd MACRO
; Ends the buffer string.
;
; Receives: EDI, the offset to the buffer.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        mov     BYTE PTR [edi], 0
        mov     edi, OFFSET buffer
ENDM

;--------------------------------------------------------------------------------
mCopy MACRO
; Copies the character in the string to the buffer.
;
; Receives: EDX, the offset to the string.
;           EDI, the offset to the buffer.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    eax
        mov     al, BYTE PTR [edx]
        mov     BYTE PTR [edi], al
        inc     edi
        inc     edx
        pop     eax
ENDM

;--------------------------------------------------------------------------------
mIs MACRO char
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
mIsI MACRO char
; Checks if the character is the given character (case-insensitive).
;
; Receives: char, the character to check (must be in lower case).
;           EDX, the offset to the string.
;
; Returns: Zero flag is set if matched.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        push    eax
        mov     al, [edx]
        or      al, 00100000b                            ; Transforms al to lower case.
        cmp     al, char
        pop     eax
ENDM

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

IFDEF   ADDON
;--------------------------------------------------------------------------------
IsID PROC uses ebx edx ecx esi
; Checks if the buffer string is a reserved word.
;
; Receives: Nothing.
;
; Returns: Zero flag is set if it is.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        mov     ecx, LENR
        mov     ebx, 0

l_II:                                                    ; Loops for every reserved word.
        mov     edx, OFFSET buffer
        mov     esi, 0
l_II2:                                                   ; Loops to check character by character.
        cmp     BYTE PTR [edx], 0
        jne     l_II2@
        cmp     RESERVED[ebx + esi], 0
        je      l_II_RET
l_II2@:
        mIsI    RESERVED[ebx + esi]
        jne     l_II2BK

        inc     esi
        inc     edx
        jmp     l_II2
l_II2BK:

        add     ebx, SIZER
        loop    l_II

        or      edx, edx

l_II_RET:
        ret
IsID ENDP

ENDIF

;--------------------------------------------------------------------------------
IsBreak PROC
; Checks if the character is a demliminator.
;
; Receives: EDX, the offset to the character.
;
; Returns: Zero flag is set if it is.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        cmp     BYTE PTR [edx], 0
        je      l_IB_RET
        cmp     BYTE PTR [edx], ' '
        je      l_IB_RET
        cmp     BYTE PTR [edx], ','
        je      l_IB_RET
        cmp     BYTE PTR [edx], ';'

l_IB_RET:
        ret
IsBreak ENDP
;--------------------------------------------------------------------------------
IsAlphabet PROC
; Checks if the character is a digit number.
;
; Receives: EDX, the offset to the character.
;
; Returns: Zero flag is set if it is valid.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        cmp     BYTE PTR [edx], 'a'
        jb      l_IA_RET1
        cmp     BYTE PTR [edx], 'z'
        ja      l_IA_RET1

        cmp     eax, eax
l_IA_RET1:
        je      l_IA_RET2

        cmp     BYTE PTR [edx], 'A'
        jb      l_IA_RET2
        cmp     BYTE PTR [edx], 'Z'
        ja      l_IA_RET2

        cmp     eax, eax
l_IA_RET2:
        ret
IsAlphabet ENDP

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
IsBeg PROC
; Checks if the character is a valid beginning character of an identifier.
;
; Receives: EDX, the offset to the character.
;
; Returns: Zero flag is set if it is valid.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        cmp     BYTE PTR [edx], '_'
        je      l_IB_RET
        cmp     BYTE PTR [edx], '@'
        je      l_IB_RET
        cmp     BYTE PTR [edx], '?'
        je      l_IB_RET
        cmp     BYTE PTR [edx], '$'
        je      l_IB_RET

        call    IsAlphabet
l_IB_RET:
        ret
IsBeg ENDP

;--------------------------------------------------------------------------------
IsNext PROC
; Checks if the character is a valid character of an identifier.
;
; Receives: EDX, the offset to the character.
;
; Returns: Zero flag is set if it is valid.
;          Carry flag may be modified.
;--------------------------------------------------------------------------------
        call    IsBeg
        je      l_NX_RET
        mIsDigit
l_NX_RET:
        ret
IsNext ENDP



;--------------------------------------------------------------------------------
FSM PROC uses edx edi eax
; A finite state machine to recognize assembly code.
;
; Receives: EDX, the pointer to the source code string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        
IFDEF   ADDON
        ; Auxiliary variable for additional error checking.
        LOCAL   lb:BYTE, incn:BYTE, nown:BYTE
        mov     lb, 0
        mov     incn, 0
ENDIF

; Initialization
        mov     edi, OFFSET buffer

; Initial state, skips leading spaces and checks if the next is comment or label/operator.
ST_BEGIN:
        call    SkipSpaces
        mIs     ';'
        je      ST_BCOMMENT
        mIs     0
        je      ST_END

; Checks label and operator.
ST_FIRST:
        mIsI    'c'
        je      ST_C
        mIsI    'm'
        je      ST_M
        mIsI    'a'
        je      ST_A
        mIsI    's'
        je      ST_S
        mIsI    'd'
        je      ST_D
        mIsI    'i'
        je      ST_I
        mIsI    'j'
        je      ST_J
        mIsI    'l'
        je      ST_L
        mIsI    'r'
        je      ST_R

        ; Checks if the first character is valid.
        call    IsBeg
        je      ST_FIRSTC
        mPuts   OFFSET STRERRINC
        call    Crlf
        mIsDigit
        je      ST_INT
        jmp     ST_LB

; No operator is possible, reads the identifier.
ST_FIRSTC:
        mCopy
        call    IsNext
        je      ST_FIRSTC

        mIs     ':'
        je      ST_LABEL
        mov     eax, edx
        call    SkipSpaces
        mIs     ':'
        je      ST_LABEL
        mov     edx, eax
        mPuts   OFFSET STRERRINC
        call    Crlf
        call    IsBreak
        jne     ST_LB
ST_FIRSTC@NOL:
        dec     edi
        dec     edx
        cmp     edi, OFFSET buffer
        jne     ST_FIRSTC@NOL
        jmp     ST_OPER


; Common check for indentifier.
ST_CK1:
        call    IsNext
        je      ST_FIRSTC

        mIs     ':'
        je      ST_LABEL

        call    IsBreak
        jne     ST_ERRIC
        call    SkipSpaces
        mIs     ':'
        je      ST_LABEL

        jmp     ST_ERRINC

; mov movsx add sub mul div inc dec jmp loop ret
ST_C:
IFDEF   ADDON
        ; Additional check for invaild comment.
        cmp     lb, 1
        je      ST_FIRSTC
ENDIF
        mCopy
        mIsI    'o'
        je      ST_CO
        jmp     ST_CK1
ST_CO:
        mCopy
        mIsI    'm'
        je      ST_COM
        jmp     ST_CK1
ST_COM:
        mCopy
        mIsI    'm'
        je      ST_COMM
        jmp     ST_CK1
ST_COMM:
        mCopy
        mIsI    'e'
        je      ST_COMME
        jmp     ST_CK1
ST_COMME:
        mCopy
        mIsI    'n'
        je      ST_COMMEN
        jmp     ST_CK1
ST_COMMEN:
        mCopy
        mIsI    't'
        je      ST_COMMENT
        jmp     ST_CK1
ST_COMMENT:
        mCopy
        mIs     ' '
        je      ST_SCOMMENT
        jmp     ST_CK1

; States for instructions recognization.
ST_M:
        mCopy
        mIsI    'o'
        je      ST_MO
        mIsI    'u'
        je      ST_MU
        jmp     ST_CK1
ST_MU:
        mCopy
        mIsI    'l'
        je      ST_INC1
        jmp     ST_CK1
ST_MO:
        mCopy
        mIsI    'v'
        je      ST_MOV
        jmp     ST_CK1
ST_MOV:
        mCopy
        mIsI    's'
        je      ST_MOVS
        
        ; mov operator may be detected.
        dec     edx
        dec     edi
        jmp     ST_INC2
ST_MOVS:
        mCopy
        mIsI    'x'
        je      ST_INC2
        jmp     ST_CK1
ST_A:
        mCopy
        mIsI    'd'
        je      ST_AD
        jmp     ST_CK1
ST_AD:
        mCopy
        mIsI    'd'
        je      ST_INC2
        jmp     ST_CK1
ST_S:
        mCopy
        mIsI    'u'
        je      ST_SU
        jmp     ST_CK1
ST_SU:
        mCopy
        mIsI    'b'
        je      ST_INC2
        jmp     ST_CK1
ST_D:
        mCopy
        mIsI    'i'
        je      ST_DI
        mIsI    'e'
        je      ST_DE
        jmp     ST_CK1
ST_DI:
        mCopy
        mIsI    'v'
        je      ST_INC1
        jmp     ST_CK1
ST_DE:
        mCopy
        mIsI    'c'
        je      ST_INC1
        jmp     ST_CK1
ST_I:
        mCopy
        mIsI    'n'
        je      ST_IN
        jmp     ST_CK1
ST_IN:
        mCopy
        mIsI    'c'
        je      ST_INC1
        jmp     ST_CK1
ST_J:
        mCopy
        mIsI    'm'
        je      ST_JM
        jmp     ST_CK1
ST_JM:
        mCopy
        mIsI    'p'
        je      ST_INC1
        jmp     ST_CK1
ST_L:
        mCopy
        mIsI    'o'
        je      ST_LO
        jmp     ST_CK1
ST_LO:
        mCopy
        mIsI    'o'
        je      ST_LOO
        jmp     ST_CK1
ST_LOO:
        mCopy
        mIsI    'p'
        je      ST_INC1
        jmp     ST_CK1
ST_R:
        mCopy
        mIsI    'e'
        je      ST_RE
        jmp     ST_CK1
ST_RE:
        mCopy
        mIsI    't'
        je      ST_RET
        jmp     ST_CK1


; Instructions with only 1 operand.
ST_INC1:
IFDEF   ADDON
        mov     al, 1
        jmp     ST_INC
ENDIF
; Instructions with only 2 operands.
ST_INC2:
IFDEF   ADDON
        mov     al, 2
ENDIF

ST_INC:
        mCopy
        mIs     ':'
        jne     ST_INC@NOID
        mPuts   OFFSET STRERRID
        call    Crlf
        jmp     ST_LABEL
ST_INC@NOID:

        call    IsBreak
        jne     ST_CK1
        call    SkipSpaces
        mIs     ':'
        jne     ST_INC@NOID2
        mPuts   OFFSET STRERRID
        call    Crlf
        jmp     ST_LABEL
ST_INC@NOID2:

IFDEF   ADDON
        mov     incn, al
ENDIF
        mPuts   OFFSET STRINC
        mEnd
        mPuts   OFFSET buffer
        call    Crlf

IFDEF   ADDON
        mov     nown, 0
ENDIF
        jmp     ST_OPER
; Recognized registers:
; eax ax ah al
; ebx bx bh bl
; ecx cx ch cl
; edx dx dh dl
; esi si
; edi di
; ebp bp
; esp sp
; cs ds ss es fs gs
ST_SECOND:
        call    SkipSpaces
        mIs     ';'
        je      ST_BCOMMENT
        mIs     0
        je      ST_END
        mIs     ','
        je      ST_SECOND@NOERROPER
        mPuts   OFFSET STRERROPER
        call    Crlf
        jmp     ST_BEGIN
ST_SECOND@NOERROPER:
        mNext

ST_OPER:
        ; Checks for registers.
        call    SkipSpaces
        mIsI    'e'
        je      ST_IE
        mIsI    'a'
        je      ST_IA
        mIsI    'b'
        je      ST_IB
        mIsI    'c'
        je      ST_IC
        mIsI    'd'
        je      ST_ID
        mIsI    's'
        je      ST_IS
        mIsI    'e'
        je      ST_IZ
        mIsI    'f'
        je      ST_IZ
        mIsI    'g'
        je      ST_IZ
        call    IsBeg
        je      ST_ID
        mIsDigit
        je      ST_INT
        call    IsBreak
        je      ST_ERRNOOPER                             ; No operands found, error!
        jmp     ST_ERRIC                                 ; Strange character is found!

ST_IDEN:
        mCopy
        call    IsNext
        je      ST_IDEN

        mIs     ':'
        je      ST_ERROPER
        call    IsBreak
        jne     ST_ERRIC                                 ; Strange character is found!

        mov     BYTE PTR [edi], 0
IFDEF   ADDON
        ; Special checks for reserved words.
        call    IsID
        jne     ST_IDEN@NOID
        mPuts   OFFSET STRERRID
        call    Crlf
ST_IDEN@NOL:
        dec     edi
        dec     edx
        cmp     edi, OFFSET buffer
        jne     ST_IDEN@NOL
        jmp     ST_BEGIN

ST_IDEN@NOID:

        ; Special checks for number of operands.
        cmp     incn, 0
        jne     ST_IDEN@NOIOP
        mPuts   OFFSET STRERRNUM
        call    Crlf
ST_IDEN@NOIOP:
        dec     incn
        inc     nown

ENDIF
        mEnd

        mPuts   OFFSET STRID
        mPuts   OFFSET buffer
        call    Crlf

        jmp     ST_SECOND
ST_INT:
        mCopy
        mIsDigit
        je      ST_INT
        call    IsBreak
        jne     ST_ERRDIGIT

        mEnd
IFDEF ADDON
        ; Special checks for ingeter constant in the first operand.
        cmp     nown, 0
        jne      ST_INT@NOERROPER
        mPuts   OFFSET STRERROPER
        call    Crlf
ST_INT@NOERROPER:
        inc     nown
        dec     incn
ENDIF
        mPuts   OFFSET STRINTEGER
        mPuts   OFFSET buffer
        call    Crlf

        jmp     ST_SECOND

; Common check for indentifier.
ST_CK2:
        call    IsNext
        je      ST_IDEN

        mIs     ':'
        je      ST_ERROPER
        call    IsBreak
        jne     ST_ERRIC

        dec     edx
        dec     edi
        jmp     ST_IDEN

; States for registers recognization.
ST_IE:
        mCopy
        mIsI    'a'
        je      ST_IEX
        mIsI    'b'
        je      ST_IEB
        mIsI    'c'
        je      ST_IEX
        mIsI    'd'
        je      ST_IED
        mIsI    's'
        je      ST_IES

        jmp     ST_CK2
ST_IEX:
        mCopy
        mIsI    'x'
        je      ST_IREG

        jmp     ST_CK2
ST_IEB:
        mCopy
        mIsI    'x'
        je      ST_IREG
        mIsI    'p'
        je      ST_IREG

        jmp     ST_CK2
ST_IED:
        mCopy
        mIsI    'x'
        je      ST_IREG
        mIsI    'i'
        je      ST_IREG

        jmp     ST_CK2
ST_IES:
        mCopy
        mIsI    'i'
        je      ST_IREG
        mIsI    'p'
        je      ST_IREG

        call    IsBreak
        jne     ST_CK2
        dec     edx
        dec     edi
        jmp     ST_IREG
ST_IA:
        mCopy
        mIsI    'x'
        je      ST_IREG
        mIsI    'h'
        je      ST_IREG
        mIsI    'l'
        je      ST_IREG

        jmp     ST_CK2
ST_IB:
        mCopy
        mIsI    'x'
        je      ST_IREG
        mIsI    'h'
        je      ST_IREG
        mIsI    'l'
        je      ST_IREG
        mIsI    'p'
        je      ST_IREG

        jmp     ST_CK2
ST_IC:
        mCopy
        mIsI    'x'
        je      ST_IREG
        mIsI    'h'
        je      ST_IREG
        mIsI    'l'
        je      ST_IREG
        mIsI    's'
        je      ST_IREG

        jmp     ST_CK2
ST_ID:
        mCopy
        mIsI    'x'
        je      ST_IREG
        mIsI    'h'
        je      ST_IREG
        mIsI    'l'
        je      ST_IREG
        mIsI    'i'
        je      ST_IREG
        mIsI    's'
        je      ST_IREG

        jmp     ST_CK2
ST_IS:
        mCopy
        mIsI    'i'
        je      ST_IREG
        mIsI    'p'
        je      ST_IREG
        mIsI    's'
        je      ST_IREG

        jmp     ST_CK2
ST_IZ:
        mCopy
        mIsI    's'
        je      ST_IREG

        jmp     ST_CK2

ST_IREG:
        mCopy

        call    IsBreak
        jne     ST_CK2

        mPuts   OFFSET STRREG
        mEnd
        mPuts   OFFSET buffer
        call    Crlf

IFDEF ADDON
        inc     nown
        dec     incn
ENDIF
        jmp     ST_SECOND



; Treats ret instruction as a special case.
ST_RET:
        mCopy
        mIs     ':'
        je      ST_ERRID

        call    IsBreak
        jne     ST_CK1

        mPuts   OFFSET STRINC
        mEnd
        mPuts   OFFSET buffer
        call    Crlf

        call    SkipSpaces

        mIsDigit
        jne     ST_RET@END
ST_RET@OPER:
        mCopy
        mIsDigit
        je      ST_RET@OPER

        call    IsBreak
        jne     ST_ERRDIGIT

        mPuts   OFFSET STRINTEGER
        mEnd
        mPuts   OFFSET buffer
        call    Crlf

        call    SkipSpaces
ST_RET@END:
        mIs     0
        je     ST_END

        mIs     ';'
        je      ST_BCOMMENT

        mPuts   OFFSET STRERROPER
        call    Crlf
        jmp     ST_OPER


ST_LABEL:
        
IFDEF   ADDON
        ; Additional check for reserved words.

        mov     BYTE PTR [edi], 0
        call    IsID
        jne     ST_LABEL@NOID
        mPuts   OFFSET STRERRID
        call    Crlf
ST_LABEL@NOID:

        ; Additional check for multiple labels.
        cmp     lb, 1
        jne     ST_LABEL@NOMLB
        mPuts   OFFSET STRERRMLB
        call    Crlf
ST_LABEL@NOMLB:
        mov     lb, 1

ENDIF

        mPuts   OFFSET STRLABEL
        mEnd
        mPuts   OFFSET buffer
        call    Crlf

        mNext
        mIs     ':'
        jne     ST_BEGIN
        mNext
        jmp     ST_BEGIN

; All kinds of errors.
ST_ERRNOOPER:
        mPuts   OFFSET STRERRNOOPER
        jmp     ST_LB
ST_ERRNUM:
IFDEF   ADDON
        mov     incn, 0
ENDIF
        mPuts   OFFSET STRERRNUM
        jmp     ST_LB
ST_ERROPER:
        mPuts   OFFSET STRERROPER
        mIs     ':'
        jne     ST_LB
        call    Crlf
        jmp     ST_LABEL
ST_ERRDIGIT:
        mPuts   OFFSET STRERRDIGIT
        jmp     ST_LB
ST_ERRINC:
        mPuts   OFFSET STRERRINC
        jmp     ST_LB
ST_ERRIC:
        mPuts   OFFSET STRERRIC
        mIs     ':'
        jne     ST_LB
        call    Crlf
        jmp     ST_LABEL

ST_ERRID:
        mPuts   OFFSET STRERRID
        jmp     ST_LB

; Special comment treatments.
ST_SCOMMENT:
        mEnd
        call    SkipSpaces
ST_SCCOMMENT:
        mIs     0
        je      ST_SCCOMMENT@END                         ; Checks if there is no input.
        mCopy
        jmp     ST_SCCOMMENT

ST_SCCOMMENT@END:
        mEnd
        mov     dl, [buffer]
        mov     edi, OFFSET buffer                       ; Rescan the buffer to extract the comment.

ST_SCCOMMENT@ENDL:
        inc     edi
        cmp     dl, [edi]
        je      ST_SCCOMMENT@END2                        ; Stops at the ending character.
        cmp     BYTE PTR [edi], 0
        je      ST_SCCOMMENT@END2
        jmp     ST_SCCOMMENT@ENDL

ST_SCCOMMENT@END2:
        mov     BYTE PTR [edi], 0
        mov     edi, OFFSET buffer + 1
        mPuts   OFFSET STRCOMMENT
        mPuts   edi
        mEnd


        jmp     ST_LB

; Normal comment treatment.
ST_BCOMMENT:
        mNext
ST_CCOMMENT:
        mIs     0
        je      ST_CCOMMENT@END                          ; Checks if there is no input.
        mCopy
        jmp     ST_CCOMMENT
ST_CCOMMENT@END:
        mEnd
        mPuts   OFFSET STRCOMMENT
        mPuts   OFFSET buffer

ST_LB:
        call    Crlf

        call    IsBreak
        je      ST_END

; Reports unknown syntax error.
ST_UNKNOWN:
        mCopy
        call    IsBreak
        jne     ST_UNKNOWN
        mEnd
        mPuts   OFFSET STRUNKNOWN
        mPuts   OFFSET buffer
        call    Crlf
        jmp     ST_BEGIN

ST_END:
IFDEF   ADDON
        cmp     incn, 0
        jne     ST_ERRNUM
ENDIF
        ret
FSM ENDP

END main
; vim: set et:
