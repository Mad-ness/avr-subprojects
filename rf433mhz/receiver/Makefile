BINPATH=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin
OBJCOPY=${BINPATH}/avr-objcopy 
CC=${BINPATH}/avr-gcc -Iinclude -DF_CPU=8000000UL -DRCTSWITCH_RECEIVER
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
virtualwire.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o virtualwire.o VirtualWire.cpp
rcswitch.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o RCSwitch.o RCSwitch.cpp 
common.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o common.o common.c
rctswitch.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o rctswitch.o rctswitch.c
rf433.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o rf433.o rf433.c
tm1637.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o tm1637.o tm1637.c


bin: main.o tm1637.o rctswitch.o common.o
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin main.o tm1637.o rctswitch.o common.o


object:
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin ${SRC}.c 

hex: bin
	${OBJCOPY} -j .text -j .data -O ihex ${SRC}.bin ${SRC}.hex

flash: hex
#	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.wchusbserial14110 -C ${CFG} -U flash:w:${SRC}.hex
	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.usbmodem14411 -C ${CFG} -b 19200 -U flash:w:${SRC}.hex
clean:
	rm -f *.elf *.hex *.o *.bin

