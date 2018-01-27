BINPATH=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin
OBJCOPY=${BINPATH}/avr-objcopy
CC=${BINPATH}/avr-gcc -Iinclude -DF_CPU=16000000UL
DUDE=${BINPATH}/avrdude
SRC=main
TARGET=attiny13
TR=t85
FLAGS=-Wall -g -Os -mmcu=${TARGET}
##FLAGS=-mmcu=${TARGET}
CFG=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf
LIBS=manchester ${SRC}

all: hex


main.o:
	${CC} -g -Os -mmcu=${TARGET} -o main.o main.c

bin: 
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin main.c


object:
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin ${SRC}.c

hex: bin
	${OBJCOPY} -j .text -j .data -O ihex ${SRC}.bin ${SRC}.hex

flash: hex
#	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.wchusbserial14110 -C ${CFG} -U flash:w:${SRC}.hex
	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.usbmodem14111 -C ${CFG} -b 19200 -U flash:w:${SRC}.hex
clean:
	rm -f *.elf *.hex *.o *.bin

