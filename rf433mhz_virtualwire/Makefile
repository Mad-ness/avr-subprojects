BINPATH=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin
OBJCOPY=${BINPATH}/avr-objcopy 
CC=${BINPATH}/avr-gcc -Iinclude -DF_CPU=16000000UL
DUDE=${BINPATH}/avrdude
SRC=main
TARGET=attiny85
TR=t85
FLAGS=-Wall -g -Os -mmcu=${TARGET}
##FLAGS=-mmcu=${TARGET}
CFG=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf
LIBS=manchester ${SRC}

all: hex


main.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o main.o main.c
manchester.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o manchester.o Manchester.cpp
wiring.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o wiring.o include/wiring.c
wiring_digital.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o wiring_digital.o include/wiring_digital.c
wiring_analog.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o wiring_analog.o include/wiring_analog.c
pins_arduino.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o pins_arduino.o include/pins_arduino.c





bin: main.o manchester.o #wiring.o wiring_digital wiring_analog pins_arduino.o
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin \
		main.o \
		manchester.o
#		wiring.o
#
#		wiring_digital.o wiring_analog.o pins_arduino.o

object:
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin ${SRC}.c 

hex: bin
	${OBJCOPY} -j .text -j .data -O ihex ${SRC}.bin ${SRC}.hex

flash: hex
	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.usbmodem14411 -C ${CFG} -b 19200 -U flash:w:${SRC}.hex

clean:
	rm -f *.elf *.hex *.o *.bin

