BINPATH=/Applications//Arduino.app/Contents/Java/hardware/tools/avr/bin
OBJCOPY=${BINPATH}/avr-objcopy 
CC=${BINPATH}/avr-gcc -DF_CPU=1200000UL -I.
DUDE=${BINPATH}/avrdude
SRC=main
TARGET=attiny85
FLAGS=-Wall -g -Os -mmcu=${TARGET}
CFG=/Applications//Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf

build: hex

object:
	${CC} -g -Os -mmcu=${TARGET} -c -o ${SRC}.bin ${SRC}.c 


hex: object
	${OBJCOPY} -j .text -j .data -O ihex ${SRC}.bin ${SRC}.hex

flash:
	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.usbmodem14411 -C ${CFG} -b 19200 -U flash:w:main.hex

clean:
	rm -f *.elf *.hex *.o *.bin

