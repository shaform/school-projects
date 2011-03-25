TITLE	Find the least common multiple of two integers (lcm.asm)

INCLUDE Irvine32.inc

.data
PROMPT	BYTE	"Enter two non-zero numbers to get the LCM,",0dh,0ah,
		"or enter 0 to terminate the process.",0dh,0ah,0
RESULT	BYTE	"LCM is: ",0
OFLOW	BYTE	"[Overflow], the result is greater than 2^32-1.",0dh,0ah,0
PRN	BYTE	"N = ",0
PRM	BYTE	"M = ",0
LB	BYTE	0dh,0ah,0
N	DWORD	?
M	DWORD	?



.code
;--------------------------------------------------------------------------------
LCM PROC,
	   X:DWORD,
	   Y:DWORD
; Finds the least common multiple of X and Y.
;
; Receives: X, Y, 32-bit unsigned integers.
;
; Returns EAX, the LCM of X and Y.
;--------------------------------------------------------------------------------
	mov	eax, X
	mov	ebx, Y

	cmp	eax, ebx
	jnc	l_LCM_LOOP			; Carry bit is set if eba < ebx
	xchg	eax, ebx			; Exchanges eax, ebx so that eax >= ebx
	

	; Finds gcd(X, Y) by Euclidean algorithm.
l_LCM_LOOP:
	cmp	ebx, 0
	jz	l_LCM_RET			; Loops until ebx = 0

	mov	edx, 0
	div	ebx

	mov	eax, ebx			; Sets eax = ebx
	mov	ebx, edx			; Sets ebx = eax % ebx


	jmp	l_LCM_LOOP

l_LCM_RET:
	; Sets eax = X*Y/gcd(X, Y);
	mov	edx, 0
	mov	ebx, eax
	mov	eax, X
	div	ebx
	mul	Y

	; Reports if an overflow occurred.
	cmp	edx, 0
	jz	l_LCM_CORRECT
	mov	edx, OFFSET OFLOW
	call	WriteString 

l_LCM_CORRECT:
	ret
LCM ENDP


main PROC
l_MLOOP:
	mov	edx, OFFSET PROMPT
	call	WriteString 

	; Read N
	mov	edx, OFFSET PRN
	call	WriteString 
	call	ReadDec

	cmp	eax, 0
	jz	l_TERM			; Terminates if N = 0.
	mov	N, eax

	; Read M
	mov	edx, OFFSET PRM
	call	WriteString 
	call	ReadDec

	cmp	eax, 0
	jz	l_TERM			; Terminates if M = 0.
	mov	M, eax


	invoke	LCM, N, M		; Calculates LCM and store it in eax.

	mov	edx, OFFSET RESULT
	call	WriteString 

	call	WriteDec		; Prints LCM in eax.

	mov	edx, OFFSET LB
	call	WriteString 
	call	WriteString 


	jmp	l_MLOOP			; Loops to read next N, M.

l_TERM:
	exit
main ENDP



END main
