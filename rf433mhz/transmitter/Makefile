BINPATH=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin
OBJCOPY=${BINPATH}/avr-objcopy 
CC=${BINPATH}/avr-gcc -I. -Iinclude -DF_CPU=1000000UL -I../receiver -DRCTSWITCH_TRANSMITTER
AVRSIZE=${BINPATH}/avr-size
DUDE=${BINPATH}/avrdude
C_DIR=../receiver
SRC=main
TARGET=attiny13
TR=t85
FLAGS=-Wall -g -Os -mmcu=${TARGET}
##FLAGS=-mmcu=${TARGET}
CFG=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf
LIBS=manchester ${SRC}

all: hex


main.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o main.o main.c
common.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o common.o ${C_DIR}/common.c
rctswitch.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o rctswitch.o ${C_DIR}/rctswitch.c
tm1637.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o tm1637.o ${C_DIR}/tm1637.c
romsearch.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o romsearch.o romsearch.c
ds18b20.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o ds18b20.o ds18b20.c
onewire.o:
	${CC} -g -Os -mmcu=${TARGET} -c -o onewire.o onewire.c


bin: main.o common.o rctswitch.o tm1637.o onewire.o ds18b20.o romsearch.o
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin main.o common.o tm1637.o


object:
	${CC} -g -Os -mmcu=${TARGET} -o ${SRC}.bin ${SRC}.c 

hex: firmware_size bin
	${OBJCOPY} -j .text -j .data -O ihex ${SRC}.bin ${SRC}.hex

firmware_size: bin
	${AVRSIZE} -C --mcu=${TARGET} ${SRC}.bin

flash: hex
#	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.wchusbserial14110 -C ${CFG} -U flash:w:${SRC}.hex
	${DUDE} -p ${TARGET} -v -c avrisp -P /dev/cu.usbmodem14111 -C ${CFG} -b 19200 -U flash:w:${SRC}.hex
clean:
	rm -f *.elf *.hex *.o *.bin

