# Soil moisture sensor firmware

It contains two version of firmware written on C language and Assembler language. 
Btw, assembler version is most newer but C-version can also be used. 

This details found in disp.as:


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
    ; - almost same functionality I coded on C language but firmware size weight 700-800 bytes - it is only the service code without useful functionality.
    ; - if you will do same thing as me (measure soil moisture) then avoid it your device be under current
    ;   all of the time. It 1) saves some power, 2) lets your device working more time because of corrosion, 
    ;   3) I don't know but may be something in soil doesn't like current.
    ; - all calls use reg_1, reg_2 and so on as input parameters and reg_1 as output parameters.
    
