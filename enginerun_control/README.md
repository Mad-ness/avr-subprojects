# Engine run control

Souce code language: assembler
Target device: attiny2313 (later it migrates in attiny13)


## Connection layout

    PORTB |0| - (input line) which starts a connected device at PIN1
          |1| - (output line) connected device
          |2| - (output line) it blinks when PIN1 has a HIGH state
          |3| - (input line) it drops PIN1 into a LOW state


## Compiling a source code

    bash$ avra firmware.as


## Flashing an avr chip

There is used an Arduino Mega as a USBISP programmator:

    bash$ avrdude -p t2313 -P /dev/ttyUSB0 -c avrisp -b 9600 -U flash:w:firmware.as.hex

