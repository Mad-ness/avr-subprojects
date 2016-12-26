;.INCLUDE "/usr/include/avr/tn2313def.inc"
#define F_CPU       1000000
.INCLUDE "/usr/include/avr/tn13def.inc"


.def temp           = r16
.def r_counter_L    = r17
.def r_ovfCounter   = r18
.def r_secCounter   = r19
.def r_ledCounter   = r20

.equ pin_buttonON   = PORTB0        ; button ENGINE_ON
.equ pin_engineLine = PORTB1        ; engine power line
.equ pin_engineLED  = PORTB3        ; engine power LED
.equ pin_buttonOFF  = PORTB4        ; button ENGINE_OFF
.equ pin_relayLine  = PORTB2        ; it behaves same as pin_engineLine but LOW means ON and HIGH means OFF
.equ runTime_secs   = 30            ; so many seconds engine should run
.equ overflow_max   = 61            ; so many TIMER0 overflows counts one second

.equ timerOn        = (1<<CS00)|(1<<CS01) ; prescaler/64 for timer0 overflow interrupt

.MACRO start_engine
    sbi PORTB, pin_engineLine
    cbi PORTB, pin_relayLine
    clr r_ovfCounter
    clr r_secCounter
    clr r_ledCounter
    clr r_counter_L
    out TCNT0, r_ovfCounter         ; clear TIMER0 overflow counter
    ldi temp, timerOn               ; set TIMER0 prescaler 1/64
    out TCCR0B, temp                ; get started TIMER0
.ENDMACRO

.MACRO stop_engine
    clr temp
    out PORTB, temp
    nop
    out TCCR0B, temp
    sbi PORTB, pin_relayLine
.ENDMACRO


.org 0x0000 
    rjmp entry_point
.org OVF0addr
    rjmp OVF0int_handler


entry_point:
    ldi temp, LOW(RAMEND)
    out spl, temp
    ldi temp, (1<<TOIE0)            ; enable overflow interrupt of TIMER0
    out TIMSK0, temp
    ldi temp, (1<<pin_engineLine)|(1<<pin_engineLED)|(1<<pin_relayLine)
    out DDRB, temp
    clr temp
    out PORTB, temp
    nop
;    sbi PORTB, pin_relayLine
    sei

main_loop:

    sbis PINB, pin_engineLine       ; test if the button ENGINE_ON pressed
    rjmp check_buttonON             ; go to check the run button state
    sbic PINB, pin_buttonOFF        ; test if the button ENGINE_OFF pressed 
    rjmp engine_stop

    cpi r_secCounter, runTime_secs  ; test if total run seconds matches the runTime_secs
    brlo engine_running

engine_stop:
    stop_engine
    rjmp main_loop 

engine_running:
;    sbic PINB, pin_buttonOFF        ; test if the button OFF pressed
;    rjmp engine_stop                ; and stop engine if yes
    rcall engineLED_blink
    rjmp main_loop

check_buttonON:
    sbis PINB, pin_buttonON         ; test if the button ENGINE_ON pressed
    rjmp main_loop
    ; if button pressed

    start_engine
    rjmp main_loop



;
; the interrupting enabled only when
; an engine is running
;
OVF0int_handler:
    in temp, SREG
    cli                                        ; disable interrupts handling
;    sbis PINB, pin_engineLine
;    rjmp count_runtime_end
    inc r_counter_L
    cpi r_counter_L, overflow_max
    breq counter_add_second
    rjmp counted_less_a_second
counter_add_second:
    inc r_secCounter                           ; it stores running seconds
    clr r_counter_L
counted_less_a_second:
    sei                                        ; enable the interrupts handling
    out SREG, temp
    reti



;
; led blinking
;
engineLED_blink:
    cp r_ledCounter, r_counter_L               ; compare two registers
    breq blink_led_end                         ; if matches found then go out
    inc r_ledCounter                           ; if it differs when increment LED counter
    cpi r_ledCounter, (overflow_max>>1)        ; test if  1/6 of a second (30/5) is reached
    brne blink_led_end                         ;
    clr r_ledCounter                           ; reset LED counter
    sbic PINB, pin_engineLED
    rjmp blink_led_off
blink_led_on:
    sbi PORTB, pin_engineLED
    rjmp blink_led_end
blink_led_off:
    cbi PORTB, pin_engineLED
blink_led_end:
    ret

