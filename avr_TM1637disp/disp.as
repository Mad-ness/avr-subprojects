#define  F_CPU      1200000
.INCLUDE "/usr/include/avr/tn13def.inc"
.def reg_1        = r16
.def reg_2        = r17
.def reg_3        = r18

.def TM1637_d1    = r19
.def TM1637_d2    = r20
.def TM1637_d3    = r21
.def TM1637_d4    = r22

.equ TM1637_CLK   = PB3
.equ TM1637_DATA  = PB4


.MACRO TM1637_init
    sbi DDRB, TM1637_CLK
    sbi DDRB, TM1637_DATA
.ENDMACRO


prg_entry_point:
    ldi reg_1, low(RAMEND) 
    out SPL, reg_1

	sbi DDRB, PINB0
	cbi PORTB, PINB0

    ldi TM1637_d1, 0x76
    ldi TM1637_d2, 0x40
    ldi TM1637_d3, 0x5c
    ldi TM1637_d4, 0x6f
    TM1637_init

main_loop:
    rcall TM1637_display
	rcall util_delay_100ms
	inc TM1637_d4
	inc TM1637_d3
	inc TM1637_d2
	inc TM1637_d1

	cpi TM1637_d4, 10
	brne main_1
	ldi TM1637_d4, 0
main_1:

	ldi zl, LOW(TM1637_digits)
;	lpm TM1637_d4, zl
	
	sbis PINB, PINB0
	rjmp led_on

led_off:
	cbi PORTB, PINB0
	rjmp main_loop_end

led_on:
	sbi PORTB, PINB0
	rjmp main_loop_end

main_loop_end:
    rjmp main_loop


; Display something useful on a display
; params: TM1637_d[1:4] should contain letters for output
TM1637_display:
    rcall TM1637_start
    ldi reg_1, 0x40
    rcall TM1637_writeByte
    rcall TM1637_stop
    rcall TM1637_start
    ldi reg_1, 0xC0
    rcall TM1637_writeByte

    mov reg_1, TM1637_d1
    rcall TM1637_writeByte

    mov reg_1, TM1637_d2
    rcall TM1637_writeByte

    mov reg_1, TM1637_d3
    rcall TM1637_writeByte

    mov reg_1, TM1637_d4
    rcall TM1637_writeByte

    rcall TM1637_stop
    rcall TM1637_start
    ldi reg_1, 0x8F
    rcall TM1637_writeByte
    rcall TM1637_stop
    nop
    ;ldi reg_1, 5
    ;rcall TM1637_delay
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


util_delay_1s:
    ldi  r18, 7
    ldi  r19, 23
    ldi  r20, 106
L1_1s: 
	dec  r20
    brne L1_1s
    dec  r19
    brne L1_1s
    dec  r18
    brne L1_1s
    rjmp PC+1
	ret

util_delay_5s:
    ldi  r18, 31
    ldi  r19, 113
    ldi  r20, 30
L1_5s: 
	dec  r20
    brne L1_5s
    dec  r19
    brne L1_5s
    dec  r18
    brne L1_5s
    rjmp PC+1
	ret

util_delay_100ms:
    ldi  r18, 156
    ldi  r19, 215
L1_100ms: 
	dec  r19
    brne L1_100ms
    dec  r18
    brne L1_100ms
	ret

TM1637_digits:
	.db	0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f

