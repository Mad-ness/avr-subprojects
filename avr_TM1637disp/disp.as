;
; This firmware measures a resistance of a connected to pin_ADCin pin device,
; performs ADC converting, next it converts the value into percent value,
; where 100% = 255, 0% = 0 and displays it at TM1637 based digital display.
;
; If got value less than MIN_LEVEL it set HIGH value on pin_HLower and starts blinking on pin_LED.
; If got value more than MAX_LEVEL it set LOW value on pin_HLower and stops blinking on pin_LED.
; If the value is between MIN_LEVEL and MAX_LEVEL, it does nothing. Actually
; current action depends on previosly set HIGH or LOW value.
;
; The firmware is written to measure the soil moisture and activate watering device.
; It keeps the soil moisture inside of the configured range MIN_LEVEL and MAX_LEVEL.
;
; Features: 
; - in order to translate raw ADC value to percent value it uses a map table. 
;   Such trick adds around 100 bytes to the code (see ADC_degrees), but it allowed to avoid divide operations 
;   which are not implemented into hardware.
; - in order to display more letters that are not presented in the TM1637_digits table, just add required
;   letters and call fill desired digital with appropriate index number. Like this, ldi TM1637_d1, 11 (displays 'H').
; - small firware size: 470 bytes. For ATtiny13 is it very good.
; 
; Notes: 
; - almost same functionality I coded on C language but firmware size weight 700-800 bytes.
; - if you will do same thing as me (measure soil moisture) then avoid it your device be under current
;   all of the time. It 1) saves some power, 2) lets your device working more time because of corrosion, 
;   3) I don't know by maybe something in soil doesn't like current.
; - all calls use reg_1, reg_2 and so on as input parameters and reg_1 as output parameters.
;
#define  F_CPU      1200000
.INCLUDE "/usr/include/avr/tn13def.inc"
.def reg_1        = r16
.def reg_2        = r17
.def reg_3        = r18
.def reg_4	      = r19
.def reg_5        = r20

.def reg_01       = r0
.def reg_02       = r1
.def reg_03       = r2

.def TM1637_d1    = r21
.def TM1637_d2    = r22
.def TM1637_d3    = r23
.def TM1637_d4    = r24

.equ TM1637_CLK   = PB3
.equ TM1637_DATA  = PB4
.equ pin_LED      = PORTB0
.equ pin_ADCin    = PORTB2
.equ pin_HLower   = PORTB1      ; this line indicates whether humidity lower than minimum
.equ MIN_LEVEL    = 15          ; humidity acceptable range
.equ MAX_LEVEL    = 25


.MACRO TM1637_init              ; Init TM1637 digital display
    sbi DDRB, TM1637_CLK
    sbi DDRB, TM1637_DATA
.ENDMACRO

.MACRO ADC_init                 ; Init ADC convertor
    ldi reg_1, (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN) | (1 << ADSC)
    out ADCSRA, reg_1
    ldi reg_1, (1 << MUX0) | (0 << MUX1) | (1 << ADLAR)
    out ADMUX, reg_1
    sbic DDRB, pin_ADCin
    sbic PORTB, pin_ADCin
.ENDMACRO

.MACRO ADC_read                 ; Save into reg_1 ADC value
    sbi ADCSRA, ADSC
    nop
ADC_read_1:
    sbic ADCSRA, ADSC
    rjmp ADC_read_1
    nop
    in reg_1, ADCH
.ENDMACRO

.MACRO ADC_pause                ; Pause ADC conventor
    cbi ADCSRA, ADEN
.ENDMACRO

.MACRO ADC_resume               ; Resume ADC convertor
    sbi ADCSRA, ADEN
.ENDMACRO


prg_entry_point:
    ldi reg_1, low(RAMEND) 
    out SPL, reg_1

    ldi reg_1, (1 << pin_LED) | (1 << pin_HLower)
    out DDRB, reg_1
	;sbi DDRB, pin_LED
	;;cbi PORTB, pin_LED
    clr reg_1
    out PORTB, reg_1

    TM1637_init
    ADC_init
    ldi reg_1, 0x0
    mov reg_03, reg_1


    ldi TM1637_d4, 11               ; 'H'
    ldi TM1637_d3, 12               ; 'E'
    ldi TM1637_d2, 13               ; 'L'
    ldi TM1637_d1, 0                ; 'O'
    rcall TM1637_display

    ldi reg_1, 5
pause_on_helo:
	rcall util_delay_500ms
    dec reg_1
    cpi reg_1, 0
    brne pause_on_helo

main_loop:


     
    ldi TM1637_d4, 11               ; 'H'   fill only _d4, rest digits are filled by ADC_substruct

    ADC_resume
    ADC_read
;    dec reg_1                        ; we had a trouble in a convertion, so 1 point does do not too much
    ADC_pause                       ; stop waste current

    rcall ADC_dig2perc2             ; comment this call if you want to see analog-digital converted value

    cpi reg_1, MIN_LEVEL            ; checks if percent is lower than acceptable, if yes it jumps to ON signal
    brlo main_on_signal
    
    cpi reg_1, (MAX_LEVEL+1)        ; if level is higher than possible then go to OFF signal
    brge main_off_signal             

    ;sbrs reg_status, pin_HLower     ; if MIN_LEVEL < level < MAX_LEVEL then do nothing
    sbis PINB, pin_HLower
    rjmp main_1                     ; it indicates any of two states: either signal is ON, or OFF

main_on_signal:
    sbi PORTB, pin_HLower
;    sbr reg_status, pin_HLower
    rjmp main_1

main_off_signal:
    cbi PORTB, pin_HLower
;    cbr reg_status, pin_HLower

main_1:
    rcall DIV_substruct             ; it fills _d1, _d2 and _d3 with numbers for displaying
    rcall TM1637_display
    rcall util_delay_500ms

    sbic PINB, pin_HLower     ; if no pin_HLower activated then skip led blinking
    rjmp main_loop_end


led_off:
	cbi PORTB, pin_LED
	rjmp main_loop_end

led_on:
	sbi PORTB, pin_LED
	rjmp main_loop_end

main_loop_end:
;    inc TM1637_d1



;    inc reg_03
;    ldi reg_1, 99
;    cp reg_03, reg_1
;    brne main_loop
;    clr reg_03
    rjmp main_loop
;
; End main loop
;


    

;; It runs increased d1, after d1>9, it increases d2 and so on. It is a just a counter
;;
;;TM1637_increase:
;;;    push reg_2
;;    ldi reg_2, 0x0a
;;    inc TM1637_d1
;;    cpse TM1637_d1, reg_2
;;    rjmp TM1637_increase_end
;;d1_inc:
;;    clr TM1637_d1
;;    inc TM1637_d2
;;    cpse TM1637_d2, reg_2
;;    rjmp TM1637_increase_end
;;d2_inc:
;;    clr TM1637_d2
;;    inc TM1637_d3
;;    cpse TM1637_d3, reg_2
;;    rjmp TM1637_increase_end
;;d3_inc:
;;    clr TM1637_d3
;;    inc TM1637_d4
;;    cpse TM1637_d4, reg_2
;;    rjmp TM1637_increase_end
;;d4_inc:
;;    clr TM1637_d1
;;    clr TM1637_d2
;;    clr TM1637_d3
;;    clr TM1637_d4
;;TM1637_increase_end:
;;;    pop reg_2
;;    ret

; Display something useful on a display
; params: TM1637_d[1:4] should contain letters for output
TM1637_display:
	push reg_1
    rcall TM1637_start
    ldi reg_1, 0x40
    rcall TM1637_writeByte
    rcall TM1637_stop
    rcall TM1637_start
    ldi reg_1, 0xC0
    rcall TM1637_writeByte

    mov reg_1, TM1637_d4
	rcall TM1637_dtrans
    rcall TM1637_writeByte

    mov reg_1, TM1637_d3
	rcall TM1637_dtrans
    rcall TM1637_writeByte

    mov reg_1, TM1637_d2
    rcall TM1637_dtrans
    rcall TM1637_writeByte

	mov reg_1, TM1637_d1
	rcall TM1637_dtrans
    rcall TM1637_writeByte

    rcall TM1637_stop
    rcall TM1637_start
    ldi reg_1, 0x8F
    rcall TM1637_writeByte
    rcall TM1637_stop
    nop
	pop reg_1
    ret

; translates human-readable numbers into machine code
; input digit in reg_1
; output digit in reg_1
TM1637_dtrans:
	ldi zl, low(2*TM1637_digits)
    ldi zh, high(2*TM1637_digits)
	add zl, reg_1
	lpm reg_1, z 
	ret

; Expected byte in reg_1
; reg_1 contains a char to be written is to be passed as an argument of the call
; used temp registers: reg_1, reg_2, reg_3
; params: reg_1 - incoming char (8-bit) that to be sent out
TM1637_writeByte:
    ldi reg_2, 8
TM1637_writeByte_1:
    cbi PORTB, TM1637_CLK

; starting if condition
    mov reg_3, reg_1
    cbr reg_3, 0xfe
    cpi reg_3, 0x01
    brne TM1637_writeByte_send_low

TM1637_writeByte_send_high:
    sbi PORTB, TM1637_DATA
    rjmp TM1637_writeByte_sync

TM1637_writeByte_send_low:
    cbi PORTB, TM1637_DATA 
    rjmp TM1637_writeByte_sync

TM1637_writeByte_sync:
    nop
    lsr reg_1 
    sbi PORTB, TM1637_CLK                    ; to be fixed, it is not correct
    nop

    dec reg_2
    cpi reg_2, 0                            ; end of 8-bit loop
    brne TM1637_writeByte_1


    cbi PORTB, TM1637_CLK
    nop
    cbi DDRB, TM1637_DATA

TM1637_writeByte_wait_ACK:
    sbic PINB, TM1637_DATA
    rjmp TM1637_writeByte_wait_ACK          ; wait for acknowledgment
    
    sbi DDRB, TM1637_DATA
    sbi PORTB, TM1637_CLK
    nop
    cbi PORTB, TM1637_CLK
    ret


; pass reg_1 number of null cycles
TM1637_delay:
    dec reg_1
    nop
    cpi reg_1, 0
    brne TM1637_delay
    ret

TM1637_start:
    sbi PORTB, TM1637_CLK
    sbi PORTB, TM1637_DATA
    nop
    cbi PORTB, TM1637_DATA
    ret


TM1637_stop:
    cbi PORTB, TM1637_CLK
    nop
    cbi PORTB, TM1637_DATA
    nop
    sbi PORTB, TM1637_CLK
    nop 
    sbi PORTB, TM1637_DATA
    ret


;;util_delay_1s:
;;    ldi  r18, 7
;;    ldi  r19, 23
;;    ldi  r20, 106
;;L1_1s: 
;;	dec  r20
;;    brne L1_1s
;;    dec  r19
;;    brne L1_1s
;;    dec  r18
;;    brne L1_1s
;;    rjmp PC+1
;;	ret
;;
;;util_delay_5s:
;;    ldi  r18, 31
;;    ldi  r19, 113
;;    ldi  r20, 30
;;L1_5s: 
;;	dec  r20
;;    brne L1_5s
;;    dec  r19
;;    brne L1_5s
;;    dec  r18
;;    brne L1_5s
;;    rjmp PC+1
;;	ret
;;
;;util_delay_100ms:
;;    ldi  r18, 156
;;    ldi  r19, 215
;;L1_100ms: 
;;	dec  r19
;;    brne L1_100ms
;;    dec  r18
;;    brne L1_100ms
;;	ret


util_delay_500ms:
    ldi  r18, 4
    ldi  r19, 12
    ldi  r20, 52
L1_500ms: 	
	dec  r20
    brne L1_500ms
    dec  r19
    brne L1_500ms
    dec  r18
    brne L1_500ms
	ret


;
; It splits number into two numbers - tens, and simple number
; Ex. 53 will be split into 5 and 3, 67 - into 6 and 7, 5 - into 0 and 5
; It is necessary for displaying it by TM1637
; input: reg_1 is number to be splitted
; output: TM1637_d2 and TM1637_d1 will be filled properly
DIV_substruct:
    push reg_2
    clr reg_2
    clr reg_3

DIV_substruct_4:
    cpi reg_1, 100
    brlo DIV_substruct_3
    subi reg_1, 100
    inc reg_3
    rjmp DIV_substruct_4


DIV_substruct_3:
    cpi reg_1, 10
    brlo DIV_substruct_2            ; need to substruct 10
    subi reg_1, 10
    inc reg_2
    rjmp DIV_substruct_3            ; reg_1 is less than 10

DIV_substruct_2:
    cpi reg_3, 0
    brne DIV_substruct_5
    ldi reg_3, 10                   ; '-'

DIV_substruct_5:
    mov TM1637_d3, reg_3
    mov TM1637_d1, reg_1
    mov TM1637_d2, reg_2
    pop reg_2
    ret

;;;
;;; it divides two single-byte numbers
;;; input: reg_1 is a dividend
;;;        reg_2 is a divisor
;;; output: reg_1 into there passed the result
;;; ANS = reg_01
;;; REM = reg_02
;;; A = reg_1
;;; B = reg_2
;;; C = reg_3
;;DIV_byte:
;;
;;    ldi reg_3, 9 
;;    sub reg_02, reg_02
;;    mov reg_01, reg_1
;;DIV_byte_loop:
;;    rol reg_01
;;    dec reg_3
;;    breq DIV_byte_done
;;    rol reg_02
;;    sub reg_02, reg_2
;;    brcc DIV_byte_skip
;;    add reg_02, reg_2
;;    clc
;;    rjmp DIV_byte_loop
;;
;;DIV_byte_skip:
;;    rjmp DIV_byte_loop
;;DIV_byte_done:
;;    mov reg_1, reg_01 
;;    ret


;;;
;;; it multiplies two single-byte numbers
;;; input: reg_1, reg_2 are multiplied numbers
;;; output: reg_1 contains a result
;;; note: result must be not more than 255
;;MUL_byte:
;;    cpi reg_1, 0                ; check if one of two's has zero
;;    breq MUL_byte_done_0
;;    cp reg_3, reg_1             ; save save one of the operands
;;MUL_byte_check_zero:
;;    cpi reg_2, 0
;;    breq MUL_byte_done_0
;;
;;MUL_byte_again:
;;    add reg_1, reg_3
;;    dec reg_2
;;    rjmp MUL_byte_check_zero
;;    cpi reg_2, 0
;;    brne MUL_byte_again
;;    rjmp MUL_byte_done
;;MUL_byte_done_0:
;;    clr reg_1, reg_1
;;MUL_byte_done:
;;    ret
;;


;; it works badly, don't use it
;ADC_dig2perc:
;    ldi zl, low(2*ADC_degrees)      ; multiply by 2 
;    ldi zh, high(2*ADC_degrees)     ; multiply by 2
;    clr reg_3                       ; set starting value as 0xff, next increase will drop it to zero
;
;ADC_dig2perc_1:
;    add zl, reg_3
;    lpm reg_2, z+
;    inc reg_3
;    cpi reg_3, 102                  ; don't try to understand why here 102 not just 100
;    brsh ADC_dig2perc_2             ; just accept it as is
;    cp reg_1, reg_2
;    brlo ADC_dig2perc_2
;    rjmp ADC_dig2perc_1
;
;ADC_dig2perc_2:
;    subi reg_3, 2                   ; same as 102 explanation
;    mov reg_1, reg_3
;    ret



ADC_dig2perc2:
    ldi zl, low(2*ADC_degrees)      ; multiply by 2 
    ldi zh, high(2*ADC_degrees)     ; multiply by 2
    ser reg_3                       ; set starting value as 0xff, first increasing will drop it to zero
    mov reg_4, zl                   ; start point for looking up the data table
    mov reg_5, zh

ADC_dig2perc2_1:
    inc reg_3
    lpm reg_2, z+
    cpi reg_3, 100                  ; don't try to understand why here 102 not just 100
    brsh ADC_dig2perc2_2             ; just accept it as is
    cp reg_1, reg_2
    brlo ADC_dig2perc2_2
    rjmp ADC_dig2perc2_1

ADC_dig2perc2_2:
    dec reg_3
    mov reg_1, reg_3
    ret


TM1637_digits:
	.db	0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f   ; numbers 0..9
    .db 0x40,0x76,0x79,0x38                                 ; letters -,H,E,L
ADC_degrees:        ; Numbers counted from 0 to 100 means a percent
    .db 0,2,5,7,10,12,15,17,20,22,25,27,30,33,35,38,40,43,45,48,50,53,55,58,60,63,66,68,71,73,76,78,81,83,86,88,91,93,96,99
    .db 101,104,106,109,111,114,116,119,121,124,127,129,132,134,137,139,142,144,147,149,152,154,157,160,162,165,167,170,172,175,177,180,182,185,187,190,193,195,198,200
    .db 203,205,208,210,213,215,218,220,223,226,228,231,233,236,238,241,243,246,248,251,254,255

