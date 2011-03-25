TITLE	Display the prime numbers smaller than N (prime.asm)

INCLUDE Irvine32.inc

.data
PROMPT	BYTE	"Enter a positive number N <= 9999 to get primes numbers smaller than N,",0dh,0ah,
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
GenPrimes PROC
; Generates prime numbers and store them in primes[].
;
; Receives: nothing.
;
; Returns: nothing.
;--------------------------------------------------------------------------------
	mov	esi, OFFSET cp_tab + 2	; We checks primes from 2.
	mov	ecx, 2

	mov	edi, OFFSET primes
	mov	eax, 4			; eax = ecx^2 < MAXP.


	; Using Sieve of Eratosthenes to determine primes P, 1<P<MAXP.

l_GP_LOOP:
	cmp	eax, MAXP
	jae	l_GP_LOOP_END	; Breaks if eax >= MAXP.
	
	test	BYTE PTR [esi], 1
	jnz	l_GP_LOOP_CON

	mov	[edi], ecx		; Stores the prime in primes[].
	add	edi, TYPE primes


	; Crosses out composites.
	mov	ebx, eax
	add	eax, OFFSET cp_tab
l_GP_CROSS:
	mov	BYTE PTR [eax], 1

	add	eax, ecx
	cmp	eax, MAXP + OFFSET cp_tab
	jb	l_GP_CROSS		; Breaks if eax >= cp_tab[MAXP].
; End of l_GP_CROSS.


l_GP_LOOP_CON:
	inc	ecx
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
	mov	[edi], ecx		; Stores the prime in [ebx];
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
	mov	edx, OFFSET PROMPT
	call	WriteString 

	; Read N
	mov	edx, OFFSET PRN
	call	WriteString 
	call	ReadDec

	cmp	eax, 0
	jz	l_TERM			; Terminates if N = 0.
	mov	edi, eax

	mov	edx, OFFSET RESULT
	call	WriteString 


	; Prints out all primes < edi
	mov	esi, OFFSET primes + TYPE primes
	cmp	edi, 2
	jbe	l_PRINT_END
	mov	eax, 2
	call	WriteDec		; Prints 2 first.
l_PRINT:
	cmp	DWORD PTR [esi], 0
	jz	l_PRINT_END
	cmp	[esi], edi
	jae	l_PRINT_END

	mov	edx, OFFSET NX
	call	WriteString 
	
	mov	eax, [esi]
	call	WriteDec		; Prints the prime in eax.
	add	esi, TYPE primes
	
	jmp	l_PRINT

l_PRINT_END:
	call Crlf


	mov	ecx, edi
	mov	esi, OFFSET cp_tab
	add	esi, edi

	jmp	l_MLOOP			; Loops to read next N.

l_TERM:
	exit
main ENDP



END main
