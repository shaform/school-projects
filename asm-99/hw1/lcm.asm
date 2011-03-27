TITLE	Find the least common multiple of two integers (lcm.asm)

INCLUDE Irvine32.inc

.data
PROMPT	BYTE	"Enter two non-zero numbers to get the LCM,",0dh,0ah,
		"or enter 0 to terminate the process.",0dh,0ah,0
RESULT	BYTE	"LCM is: ",0
PRN	BYTE	"N = ",0
PRM	BYTE	"M = ",0
OFLOW	BYTE	" (HEX notation.)",0
N	DWORD	?
M	DWORD	?



.code

;--------------------------------------------------------------------------------
mPuts MACRO pts
; Prints a string to the terminal.
;
; Receives: pts, the offset to the string.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
	push	edx
	mov	edx, pts
	call	WriteString
	pop	edx
ENDM

;--------------------------------------------------------------------------------
LCM PROC USES ebx,
	   X:DWORD,
	   Y:DWORD
; Finds the least common multiple of X and Y.
;
; Receives: X, Y, 32-bit unsigned integers.
;
; Returns: (EDX) EAX, the LCM of X and Y.
;          Carry bit = 1 if the result is larger the 2^32-1, 0 otherwise.
;--------------------------------------------------------------------------------
	mov	eax, X
	mov	ebx, Y

	cmp	eax, ebx
	jb	l_LCM_LOOP
	xchg	eax, ebx			; Exchanges eax, ebx so that eax >= ebx.
	

	; Finds gcd(X, Y) by Euclidean algorithm.
l_LCM_LOOP:
	cmp	ebx, 0
	je	l_LCM_RET			; Loops until ebx = 0.

	mov	edx, 0
	div	ebx

	mov	eax, ebx			; Sets eax = ebx.
	mov	ebx, edx			; Sets ebx = eax % ebx.


	jmp	l_LCM_LOOP

l_LCM_RET:
	; Sets eax = X*Y/gcd(X, Y).
	mov	edx, 0
	mov	ebx, eax
	mov	eax, X
	div	ebx
	mul	Y

	ret
LCM ENDP


ReadMN PROC
	mPuts	OFFSET PROMPT

	; Read N
	mPuts	OFFSET PRN
	call	ReadDec
	cmp	eax, 0
	je	l_READ_END		; Jumps if a N = 0.
	mov	N, eax

	; Read M
	mPuts	OFFSET PRM
	call	ReadDec
	cmp	eax, 0
	je	l_READ_END		; Jumps if a M = 0.
	mov	M, eax

	clc
	ret
	
l_READ_END:				; Sets carry flag if a zero is entered.
	stc
	ret
ReadMN ENDP


main PROC
l_MLOOP:
	call	ReadMN
	jc	l_TERM			; Terminates if a zero is entered.

	mPuts	OFFSET RESULT

	INVOKE	LCM, N, M		; Calculates LCM and store it in eax.

	jnc	l_NLARGE		; Special case for large result.

	xchg	eax, edx
	call	WriteHex
	mov	eax, edx
	call	WriteHex

	mPuts	OFFSET OFLOW
	jmp	l_LB

l_NLARGE:
	call	WriteDec		; Prints LCM in eax.

l_LB:
	call	Crlf
	call	Crlf

	jmp	l_MLOOP			; Loops to read next N, M.
l_TERM:
	exit
main ENDP

END main
