TARGET = atmega168
PROGRAMMER = m168p
F_CPU=8000000

COMPILEC = avr-gcc -Wall -O2 -Iusbdrv -I.-DF_CPU=$(F_CPU) -mmcu=$(TARGET) # -DDEBUG_LEVEL=2

OBJECTS =  main.o

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILEC) -c $< -o $@

.S.o:
	$(COMPILEC) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILEC) -S $< -o $@

clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o main.s 

# file targets:
main.bin:	$(OBJECTS)
	$(COMPILEC) -o main.bin $(OBJECTS) -Wl,-Map,main.map

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex

disasm:	main.bin
	avr-objdump -d main.bin

flash:
	avrdude -c duemilanove -p $(PROGRAMMER) -P ft0 -u -U flash:w:main.hex -B 4800

fuses:
	avrdude -c duemilanove -p $(PROGRAMMER) -P ft0 -u -U hfuse:w:0xdf:m -U lfuse:w:0xe2:m -B 4800

read:
	avrdude -c duemilanove -p $(PROGRAMMER) -P ft0 -B 4800 -v r