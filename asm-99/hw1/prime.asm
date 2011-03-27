TITLE	Display the prime numbers smaller than N (prime.asm)

INCLUDE Irvine32.inc

.data
PROMPT	BYTE	"Enter a positive number N < 10000 to get prime numbers smaller than N,",0dh,0ah,
		"or enter 0 to terminate the process.",0dh,0ah,0
PRN	BYTE	"N = ",0
NX	BYTE	", ",0
RESULT	BYTE	"Primes: ",0
N	DWORD	?

MAXP	=	10000

cp_tab	BYTE	MAXP	DUP(0)		; cp_tab[i] is true if i is composite.
primes	DWORD	MAXP	DUP(0)		; Table of primes, terminated by 0

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
GenPrimes PROC USES eax ecx edx esi edi
; Generates prime numbers and store them in primes[].
;
; Receives: Nothing.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
	mov	esi, OFFSET cp_tab + 2	; We checks primes from 2.
	mov	ecx, 2

	mov	edi, OFFSET primes
	mov	eax, 4			; eax = ecx^2 < MAXP.


	; Using Sieve of Eratosthenes to determine primes P, 1<P<MAXP.

l_GP_LOOP:
	cmp	eax, MAXP
	jae	l_GP_LOOP_END		; Jumps if eax >= MAXP.
	
	test	BYTE PTR [esi], 1
	jnz	l_GP_LOOP_CON		; Jumps if it is composite.

	mov	[edi], ecx		; Stores the prime in primes[].
	add	edi, TYPE primes


	; Crosses out composites.
	add	eax, OFFSET cp_tab
l_GP_CROSS:
	mov	BYTE PTR [eax], 1	; Marks it as composite.
	add	eax, ecx
	cmp	eax, MAXP + OFFSET cp_tab
	jb	l_GP_CROSS		; Breaks if eax >= &cp_tab[MAXP].
; End of l_GP_CROSS.


l_GP_LOOP_CON:
	inc	ecx			; Checks next number.
	inc	esi
	mov	eax, ecx
	mul	eax

	jmp	l_GP_LOOP
l_GP_LOOP_END:


	; Increases ecx if it is even.
	test	ecx, 1
	jnz	l_GP_ODD
	inc	ecx
	inc	esi
l_GP_ODD:


	; Stores remaining primes into prime[].
l_GP_ENDLOOP:
	test	BYTE PTR [esi], 1
	jnz	l_GP_ENDLOOP_CON	; Jumps if not prime.
	mov	[edi], ecx		; Stores the prime in [edi];
	add	edi, TYPE primes

l_GP_ENDLOOP_CON:
	add	ecx, 2
	add	esi, 2			; Checks next odd number.
	cmp	ecx, MAXP
	jb	l_GP_ENDLOOP		; Breaks if ecx >= MAXP.

	ret
GenPrimes ENDP


main PROC

	call GenPrimes
l_MLOOP:
	mPuts	OFFSET PROMPT

	; Read N
	mPuts	OFFSET PRN
	call	ReadDec

	cmp	eax, 0
	je	l_TERM			; Terminates if N = 0.
	mov	edi, eax

	mPuts	OFFSET RESULT

	; Prints out all primes < N
	mov	esi, OFFSET primes + TYPE primes
	cmp	edi, 2
	jbe	l_PRINT_END
	mov	eax, 2
	call	WriteDec		; Prints 2 first.

l_PRINT:
	cmp	DWORD PTR [esi], 0
	je	l_PRINT_END		; Jumps if there are no primes left.
	cmp	[esi], edi
	jae	l_PRINT_END		; Jumps if the prime >= N.

	mPuts	OFFSET NX
	
	mov	eax, [esi]
	call	WriteDec		; Prints the prime in eax.
	add	esi, TYPE primes	; Advances to the next prime.
	
	jmp	l_PRINT

l_PRINT_END:
	call	Crlf
	jmp	l_MLOOP			; Loops to read next N.

l_TERM:
	exit
main ENDP



END main
