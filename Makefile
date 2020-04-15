MCU   = atmega328p
F_CPU = 1000000
BAUD  = 19200

PROGRAMMER_TYPE = avrisp

PORT = /dev/cu.usbmodem14101

all: main.hex

clean:
	rm main.o main.elf main.hex

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

main.elf: main.o
	avr-g++ -std=c++0x -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -Os main.o -o main.elf

main.o: main.cpp
	avr-g++ -std=c++0x -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -Wall -Os -c main.cpp

upload: main.hex
	avrdude -p $(MCU) -c $(PROGRAMMER_TYPE) -P $(PORT) -b $(BAUD) -U flash:w:main.hex
