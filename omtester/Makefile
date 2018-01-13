BINPATH=/Applications//Arduino.app/Contents/Java/hardware/tools/avr/bin
OBJCOPY=${BINPATH}/avr-objcopy 
CC=${BINPATH}/avr-gcc
DUDE=${BINPATH}/avrdude
SRC=main
TARGET=attiny13
FLAGS=-Wall -g -Os -mmcu=${TARGET}
CFG=/Applications//Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf

build: hex

object:
	${CC} -Wall -g -Os -mmcu=${TARGET} -c -o ${SRC}.o ${SRC}.c 
	${CC} -Wall -g -Os -mmcu=${TARGET} -c -o tm1637.o tm1637.c 
	${CC} -Wall -g -Os -mmcu=${TARGET} -o ${SRC}.bin ${SRC}.o tm1637.o


hex: object
	${OBJCOPY} -j .text -j .data -O ihex ${SRC}.bin ${SRC}.hex

flash:
	${DUDE} -p ${TARGET} -vvv -c usbasp -p t13 -P /dev/cu.wchusbserial14410 -C ${CFG} -b 9600 -q -U flash:w:main.hex

clean:
	rm -f *.elf *.hex *.o *.bin

