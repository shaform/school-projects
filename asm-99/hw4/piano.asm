TITLE Assembly Piano (piano.asm)
; This program is a assembly piano.
; It uses the INT 9 keyboard hardware interrupt
; to check for the key pressed and output the sound.
.model tiny, STDCALL
.386

;-----------------------------------------------------------
; Music notes
;-----------------------------------------------------------
t_1C = 9121
t_1Cu = 8609
t_1D = 8126
t_1Du = 7670
t_1E = 7239
t_1F = 6833
t_1Fu = 6449
t_1G = 6087
t_1Gu = 5746
t_1A = 5423
t_1Au = 5119
t_1B = 4831
t_2C =4560
t_2Cu = 4304
t_2D = 4063
t_2Du = 3834
t_2E = 3619
t_2F = 3416
t_2Fu = 3224
t_2G = 3043
t_2Gu = 2873
t_2A = 2711
t_2Au = 2559
t_2B = 2415
t_3C = 2280
t_3Cu = 2152
t_3D = 2031
t_3Du = 1917
t_3E = 1809
t_3F = 1715
t_3Fu = 1612
t_3G = 1521
t_3Gu = 1436
t_3A = 1355
t_3Au = 1292
t_3B = 1207
t_4C = 1140
;-----------------------------------------------------------

SPEAKER = 61h
TIMER = 42h
kybd_port = 60h
deftime = 100
SpeakerOn MACRO
        push    ax
        in      al, SPEAKER
        mov     cs:[spkst], ax
        or      al, 00000011b
        out     SPEAKER, al
        pop     ax
ENDM
SpeakerOff MACRO
        push    ax
        mov     ax, cs:[spkst]
        and     al, 11111100b
        out     SPEAKER, al
        pop     ax
ENDM

.code
        ORG   100h                                   ; Start address of the COM program.
start:
        jmp   setup

; Copied from Irvine16.
;-----------------------------------------------------------
Delay PROC FAR
;
; Create an n-millisecond delay.
; Receives: EAX = milliseconds
; Returns: nothing
; Remarks: May only used under Windows 95, 98, or ME. Does
; not work under Windows NT, 2000, or XP, because it
; directly accesses hardware ports.
; Source: "The 80x86 IBM PC & Compatible Computers" by
; Mazidi and Mazidi, page 343. Prentice-Hall, 1995.
;-----------------------------------------------------------

MsToMicro = 1000000     ; convert ms to microseconds
ClockFrequency = 15085  ; microseconds per tick
        pushad
; Convert milliseconds to microseconds.
        mov  ebx,MsToMicro
        mul  ebx

; Divide by clock frequency of 15.085 microseconds,
; producing the counter for port 61h.
        mov  ebx,ClockFrequency
        div  ebx        ; eax = counter
        mov  ecx,eax

; Begin checking port 61h, watching bit 4 toggle
; between 1 and 0 every 15.085 microseconds.
DL1:
        in  al,61h      ; read port 61h
        and al,10h      ; clear all bits except bit 4
        cmp al,ah       ; has it changed?
        je  DL1 ; no: try again
        mov ah,al       ; yes: save status
        dec ecx
        cmp ecx,0       ; loop finished yet?
        ja  DL1

quit:
        popad
        ret
Delay ENDP



;--------------------------------------------------------------------------------
PlaySound PROC FAR uses ax,
          nt:WORD,
          t:WORD
; Plays a sound.
;
; Receives: nt, the sound..
;           t, the time.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        SpeakerOn
        mov     ax, nt
        out     42h, al
        mov     al, ah
        out     42h, al
        movzx   eax, t
        call    Delay
        SpeakerOff
        ret
PlaySound ENDP

;--------------------------------------------------------------------------------
MusicBox PROC uses cx eax
; Plays a music.
;
; Receives: Nothing.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    si
        push    di
        mov     cx, LENGTHOF music
        lea     si, cs:[music]
        lea     di, cs:[speed]
@@:
        mov     ax, deftime*10
        movzx   bx, BYTE PTR cs:[di]
        mul     bx
        mov     bx, cs:[si]
        INVOKE  PlaySound, bx, deftime
        mov     eax, deftime*2
        call    Delay
        add     si, TYPE WORD
        inc     di
        loop    @B

        pop     di
        pop     si
        ret
MusicBox ENDP

;--------------------------------------------------------------------------------
MusicBox2 PROC uses cx eax
; Plays a music.
;
; Receives: Nothing.
;
; Returns: Nothing.
;--------------------------------------------------------------------------------
        push    si
        push    di
        mov     cx, LENGTHOF music2
        lea     si, cs:[music2]
        lea     di, cs:[speed2]
@@:
        mov     ax, deftime*10
        movzx   bx, BYTE PTR cs:[di]
        mul     bx
        mov     bx, cs:[si]
        INVOKE  PlaySound, bx, deftime
        mov     eax, deftime*2
        call    Delay
        add     si, TYPE WORD
        inc     di
        loop    @B

        pop     di
        pop     si
        ret
MusicBox2 ENDP

int9_handler PROC FAR
        push    ax
        push    bx
        in      al, kybd_port                           ; read keyboard port
        push    ax
        pushf
        BYTE    9ah
old_interrupt9 DWORD ?
        pop     ax
        cli
        
        .IF     al == 02h                               ; 1
        mov     bx, t_1Cu
        .ELSEIF al == 02h                               ; 2
        mov     bx, t_1Du
        .ELSEIF al == 05h                               ; 4
        mov     bx, t_1Fu
        .ELSEIF al == 06h                               ; 5
        mov     bx, t_1Gu
        .ELSEIF al == 07h                               ; 6
        mov     bx, t_1Au

        .ELSEIF al == 10h                               ; Q
        mov     bx, t_1C
        .ELSEIF al == 11h                               ; W
        mov     bx, t_1D
        .ELSEIF al == 12h                               ; E
        mov     bx, t_1E
        .ELSEIF al == 13h                               ; R
        mov     bx, t_1F
        .ELSEIF al == 14h                               ; T
        mov     bx, t_1G
        .ELSEIF al == 15h                               ; Y
        mov     bx, t_1A
        .ELSEIF al == 16h                               ; U
        mov     bx, t_1B


        .ELSEIF al == 1eh                               ; A
        mov     bx, t_2Cu
        .ELSEIF al == 1fh                               ; S
        mov     bx, t_2Du
        .ELSEIF al == 21h                               ; F
        mov     bx, t_2Fu
        .ELSEIF al == 22h                               ; G
        mov     bx, t_2Gu
        .ELSEIF al == 23h                               ; H
        mov     bx, t_2Au

        .ELSEIF al == 2ch                               ; Z
        mov     bx, t_2C
        .ELSEIF al == 2dh                               ; X
        mov     bx, t_2D
        .ELSEIF al == 2eh                               ; C
        mov     bx, t_2E
        .ELSEIF al == 2fh                               ; V
        mov     bx, t_2F
        .ELSEIF al == 30h                               ; B
        mov     bx, t_2G
        .ELSEIF al == 31h                               ; N
        mov     bx, t_2A
        .ELSEIF al == 32h                               ; M
        mov     bx, t_2B


        .ELSEIF al == 17h                               ; I
        mov     bx, t_2Cu
        .ELSEIF al == 18h                               ; O
        mov     bx, t_2Du
        .ELSEIF al == 19h                               ; P
        mov     bx, t_2Fu
        .ELSEIF al == 1ah                               ; [
        mov     bx, t_2Gu
        .ELSEIF al == 1bh                               ; ]
        mov     bx, t_2Au

        .ELSEIF al == 24h                               ; J
        mov     bx, t_2C
        .ELSEIF al == 25h                               ; K
        mov     bx, t_2D
        .ELSEIF al == 26h                               ; L
        mov     bx, t_2E
        .ELSEIF al == 27h                               ; ;
        mov     bx, t_2F
        .ELSEIF al == 28h                               ; '
        mov     bx, t_2G
        .ELSEIF al == 34h                               ; .
        mov     bx, t_2A
        .ELSEIF al == 35h                               ; /
        mov     bx, t_2B


        .ELSEIF al == 29h                               ; `
        mov     bx, t_4C

        .ELSEIF al == 01h                               ; ESC
        call    MusicBox
        .ELSEIF al == 3bh                               ; ESC
        call    MusicBox2
        jmp     to_int9

        .ELSE
        jmp     to_int9
        .ENDIF

        INVOKE  PlaySound, bx, deftime

to_int9:
        pop     bx
        pop     ax
        iret
spkst   WORD    ?
music2  WORD    t_2C, t_2C, t_2G, t_2G, t_2A, t_2A, t_2G,
                t_2F, t_2F, t_2E, t_2E, t_2D, t_2D, t_2C,
                t_2G, t_2G, t_2F, t_2F, t_2E, t_2E, t_2D,
                t_2G, t_2G, t_2F, t_2F, t_2E, t_2E, t_2D,
                t_2C, t_2C, t_2G, t_2G, t_2A, t_2A, t_2G,
                t_2F, t_2F, t_2E, t_2E, t_2D, t_2D, t_2C
speed2  BYTE    1,1,1,1,1,1,2
        BYTE    1,1,1,1,1,1,2
        BYTE    1,1,1,1,1,1,2
        BYTE    1,1,1,1,1,1,2
        BYTE    1,1,1,1,1,1,2
        BYTE    1,1,1,1,1,1,2
music   WORD    t_2G, t_2E, t_2E, t_2F, t_2D, t_2D,
                t_2C, t_2E, t_2G, t_2G, t_2E,
                t_2D, t_2D, t_2D, t_2D, t_2D, t_2E, t_2F,
                t_2E, t_2E, t_2E, t_2E, t_2E, t_2F, t_2G,
                t_2G, t_2E, t_2E, t_2F, t_2D, t_2D,
                t_2C, t_2E, t_2G, t_2G, t_2C
speed   BYTE    2,2,4,2,2,4,1,1,1,1,8
        BYTE    1,1,1,1,2,2,4,1,1,1,1,2,2,4
        BYTE    2,2,4,2,2,4,1,1,1,1,8
comment *
music   WORD    t_1A, t_2D, t_2D, t_1A, t_2E, t_2E,
                t_2D, t_2D, t_2D, t_3D, t_2E,
                t_2D, t_2D, t_2D, t_2D, t_2D, t_2E, t_2F,
                t_2E, t_2E, t_2E, t_2E, t_2E, t_2F, t_2G,
                t_2G, t_2E, t_2E, t_2F, t_2D, t_2D,
                t_2C, t_2E, t_2G, t_2G, t_2C
speed   BYTE    2,2,4,2,2,4,1,1,1,1,8
        BYTE    1,1,1,1,2,2,4,1,1,1,1,2,2,4
        BYTE    2,2,4,2,2,4,1,1,1,1,8
*
int9_handler ENDP
end_ISR label BYTE

setup:
        ; Gets interrupt vector of 9h.
        mov     ax, 3509h
        int     21h
        mov     WORD PTR old_interrupt9, bx
        mov     WORD PTR old_interrupt9+2, es

        ; Sets interrupt vector of 9h.
        mov     ax, 2509h
        mov     dx, OFFSET int9_handler
        int     21h

        ; Terminates and stays resident.
        mov     ax, 3100h
        mov     dx, OFFSET end_ISR
        shr     dx, 4
        inc     dx
        int     21h
END start
; vim: set et:
