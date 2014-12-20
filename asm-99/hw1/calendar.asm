TITLE	Get the day of the week for a given date (calendar.asm)

INCLUDE Irvine32.inc

.data
PROMPT	BYTE	"Enter a date between year 1900 to 2100 get the day of week,",0dh,0ah,
		"or enter 0 to terminate the process.",0dh,0ah,0
RESULT	BYTE	"The day of the week is ",0
ERROR	BYTE	"[Error], The date is incorrect.",0dh,0ah,0
PRY	BYTE	"Year = ",0
PRM	BYTE	"Month = ",0
PRD	BYTE	"Day = ",0
DAYSOM	DWORD	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
;		0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12
SUN	BYTE	"Sunday.",0
MON	BYTE	"Monday.",0
TUE	BYTE	"Tuesday.",0
WED	BYTE	"Wednesday.",0
THU	BYTE	"Thursday.",0
FRI	BYTE	"Friday.",0
SAT	BYTE	"Saturday.",0
day	DWORD	?
month	DWORD	?
year	DWORD	?

.code

;--------------------------------------------------------------------------------
mDiv32 MACRO a, n
; Calculates a / n, a % n.
;
; Receives: a, n, two 32-bit unsigned integers.
;
; Returns: EAX = a / n
;          EDX = a % n
;--------------------------------------------------------------------------------
	mov	eax, a
	mov	edx, 0
	div	n
ENDM

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
CheckDate PROC uses eax edx esi,
	  Y:DWORD,
	  M:DWORD,
	  D:DWORD
; Checks if a given date is right.
;
; Receives: Y:Year, M:Month and D:Day, all are 32-bit unsigned integers.
;           Assuming the three are nonzero.
;
; Returns: Carry bit = 1 if the date is wrong, 1 otherwise.
;--------------------------------------------------------------------------------

	cmp	M, 0
	je	l_CHECK_ERROR		; M cannot = 0.

	cmp	D, 0
	je	l_CHECK_ERROR		; D cannot = 0.

	cmp	M, 12
	ja	l_CHECK_ERROR		; M cannot > 12.

	cmp	M, 2
	je	l_CHECK_FEB		; Special case for February.

	mov	eax, M
	mov	esi, TYPE DWORD
	mul	esi
	mov	edx, DWORD PTR [DAYSOM+eax]

	cmp	D, edx
	ja	l_CHECK_ERROR
	jmp	l_CHECK_CORRECT

l_CHECK_FEB:
	mov	esi, 4
	mDiv32	Y, esi
	cmp	edx, 0
	jnz	l_CHECK_NLEAP

	mov	esi, 100
	mDiv32	Y, esi
	cmp	edx, 0
	jnz	l_CHECK_LEAP

	mov	esi, 400
	mDiv32	Y, esi
	cmp	edx, 0
	jnz	l_CHECK_NLEAP

l_CHECK_LEAP:
	cmp	D, 29
	ja	l_CHECK_ERROR
	jmp	l_CHECK_CORRECT

l_CHECK_NLEAP:
	cmp	D, 28
	ja	l_CHECK_ERROR

l_CHECK_CORRECT:
	clc
	ret

l_CHECK_ERROR:
	stc
	ret
CheckDate ENDP

;--------------------------------------------------------------------------------
GetDayOfWeek PROC USES edi esi edx,
	     Y:DWORD,
	     M:DWORD,
	     D:DWORD
; Get the day of the week.
;
; Receives: Y:Year, M:Month and D:Day, all are 32-bit unsigned integers.
;           The date must be between 1900 and 2100.
;
; Returns: EAX, the day of the week of the date.
;          0: Sunday, 1: Monday, 2:Tuesday and so on.
;          Carry bit = 1 if the date is wrong, 0 otherwise.
;--------------------------------------------------------------------------------
	INVOKE	CheckDate, Y, M, D
	jnc	l_GETDAY_START
	ret

l_GETDAY_START:
	
	mov	edi, 1-1		; 1/1/1 is Monday (We counts from 0/12/31.)

	sub	M, 1			; We counts months from 1 to M-1.
	mov	esi, 2
	mDiv32	M, esi

	cmp	edx, 0
	je	l_GETDAY_EVEN		; Jumps if M is even.
	add	edi, 31 MOD 7		; Counts the days in the last month.

l_GETDAY_EVEN:
	sub	Y, 1			; We counts Y-1 years.

	cmp	eax, 0
	je	l_GETDAY_DAY		; Jumps if M < 2.
	add	edi, 7-3		; February has only 30-2 days. (Substacts it by 3 and adds 1 latter.)
	add	Y, 1			; Addes the day substracted in the previous line.
					; This also adds an additional day in leap year.

	cmp	eax, 4
	jl	l_GETDAY_L8		; Jumps if M < 8.
	add	edi, 1			; August has 31 days.

l_GETDAY_L8:
	mov	esi, 61 MOD 7
	mul	esi			; Every two month have 61 days.
	add	edi, eax

l_GETDAY_DAY:
	add	edi, D			; Counts the days in the given month.

	add	edi, Y			; Every year has 365 days (without 2/29).

	mov	esi, 4
	mDiv32	Y, esi			; A leap year for every 4 years.
	add	edi, eax

	mov	esi, 100
	mDiv32	Y, esi			; The years divisible by 100 are not leap years.
	sub	edi, eax

	mov	esi, 400
	mDiv32	Y, esi			; The years divisible by 400 are still leap years.
	add	edi, eax

	mov	esi, 7
	mDiv32	edi, esi		; Gets the final result.
	mov	eax, edx

	ret
GetDayOfWeek ENDP

;--------------------------------------------------------------------------------
PrintDayOfWeek PROC USES eax,
	       week:DWORD
; Prints the day of the week.
;
; Receives: The day of the week.
;           0: Sunday, 1: Monday, 2:Tuesday and so on.
; Returns: Nothing.
;--------------------------------------------------------------------------------
	mPuts	OFFSET RESULT
	mov	eax, week
	cmp	eax, 1
	je	l_PRINT_MON
	cmp	eax, 2
	je	l_PRINT_TUE
	cmp	eax, 3
	je	l_PRINT_WED
	cmp	eax, 4
	je	l_PRINT_THU
	cmp	eax, 5
	je	l_PRINT_FRI
	cmp	eax, 6
	je	l_PRINT_SAT

l_PRINT_SUN:
	mPuts	OFFSET SUN
	jmp	l_PRINT_END
l_PRINT_MON:
	mPuts	OFFSET MON
	jmp	l_PRINT_END
l_PRINT_TUE:
	mPuts	OFFSET TUE
	jmp	l_PRINT_END
l_PRINT_WED:
	mPuts	OFFSET WED
	jmp	l_PRINT_END
l_PRINT_THU:
	mPuts	OFFSET THU
	jmp	l_PRINT_END
l_PRINT_FRI:
	mPuts	OFFSET FRI
	jmp	l_PRINT_END
l_PRINT_SAT:
	mPuts	OFFSET SAT

l_PRINT_END:
	call	Crlf
	ret
PrintDayOfWeek ENDP

;--------------------------------------------------------------------------------
ReadDate PROC USES eax
; Reads a date from the standard input.
;
; Receives: Nothing.
;
; Returns: year, month, day are set.
;          Carry bit = 1 if a zero is enterd, 0 otherwise.
;--------------------------------------------------------------------------------
	mPuts	OFFSET PROMPT

	; Reads year.
	mPuts	OFFSET PRY
	call	ReadDec
	cmp	eax, 0
	je	l_READ_END		; Jumps if year = 0.
	mov	year, eax

	; Reads month.
	mPuts	OFFSET PRM
	call	ReadDec
	cmp	eax, 0
	je	l_READ_END		; Jumps if month = 0.
	mov	month, eax

	; Reads day.
	mPuts	OFFSET PRD
	call	ReadDec
	cmp	eax, 0
	je	l_READ_END		; Jumps if day = 0.
	mov	day, eax

	clc				; No error is detect, clear the carry bit.
	ret

l_READ_END:
	stc				; An error is detect, set the carry bit.
	ret
ReadDate ENDP

main PROC
l_MLOOP:
	call	ReadDate

	jc	l_TERM			; Jumps if a zero is entered.

	INVOKE	GetDayOfWeek, year, month, day

	jc	l_ERROR			; Jumps if an error is detected.

	call	Crlf
	INVOKE	PrintDayOfWeek, eax
	call	Crlf

	jmp	l_MLOOP			; Loops to read the next date.

l_ERROR:
	call	Crlf
	mPuts	OFFSET ERROR		; Notifies the error in the given date.
	call	Crlf
	jmp	l_MLOOP			; Loops to read the next date.

l_TERM:
	exit
main ENDP

END main
