BASE    = main
SRC		= $(BASE).c
OBJECT	= $(BASE).o
CC		= avr-gcc
SOC		= attiny13
SOC_FLH = t13
PROGDEV = /dev/ttyUSB0
TARGET_HEX = $(BASE).hex
INC_PATH = -I/usr/avr

default: compile

get_hex: extract_hex

clean:
		rm -f $(OBJECT) $(BASE).elf $(BASE).lst $(BASE).map $(TARGET_HEX) $(TARGET_HEX)
		rm -f $(BASE).as.*

compile:
		$(CC) -c -mmcu=$(SOC) -I. -g -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=$(BASE).lst  -std=gnu99 $(BASE).c -o $(OBJECT) $(INC_PATH)


linking: compile
		$(CC) -mmcu=$(SOC) -I. -g -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=$(BASE).o  -std=gnu99 $(OBJECT) --output $(BASE).elf -Wl,-Map=$(BASE).map,--cref -lm

extract_hex: linking
		avr-objcopy -O ihex -R .eeprom $(BASE).elf $(TARGET_HEX)

flash: extract_hex
		avrdude -p $(SOC_FLH) -P $(PROGDEV) -c avrisp -b 9600 -U flash:w:$(TARGET_HEX)



