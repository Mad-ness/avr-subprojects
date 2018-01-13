#include <stdint.h>
#include <avr/io.h>
#define F_CPU 400000UL
#include <util/delay.h>
#include "tm1637.h"

// #define TM1637_DIO_PIN                  PB0
// #define TM1637_CLK_PIN                  PB1

void
main(void)
{
    uint8_t i = 0;

    /* setup */
    TM1637_init();

    /* loop */
    while (1) {
        TM1637_display_digit(TM1637_SET_ADR_00H, i % 0x10);
        TM1637_display_digit(TM1637_SET_ADR_01H, (i + 1) % 0x10);
        TM1637_display_digit(TM1637_SET_ADR_02H, (i + 2) % 0x10);
        TM1637_display_digit(TM1637_SET_ADR_03H, (i + 3) % 0x10);
        TM1637_display_colon(true);
        _delay_ms(200);
        TM1637_display_colon(false);
        _delay_ms(200);
        i++;
    }
}

