#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <tm1637.h>


void loop(void) {

}

void setup(void) {
    TM1637_init();
}

int main(void) { 
    setup();
    for (;;) {
        loop();
    }
    return 0;
}

