ARDUINO := /usr/share/arduino
AVR := /usr/avr

norm: gps.c
	avr-gcc -c -g -Os -Wall \
	-mmcu=attiny4313 -DF_CPU=19200000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
	main.c -o main.c.o
	avr-gcc -c -g -Os -Wall \
	-mmcu=attiny4313 -DF_CPU=19200000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
	gps.c -o gps.c.o
	avr-gcc -c -g -Os -Wall \
	-mmcu=attiny4313 -DF_CPU=19200000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
	uart.c -o uart.c.o

paranoid: gps.c
	avr-gcc -c -g -Os -Wall -pedantic -ansi -fno-exceptions -ffunction-sections -fdata-sections \
	-std=iso9899:1999 \
	-Wall -pedantic \
	-Wmissing-prototypes \
	-pedantic-errors -Wextra -Wall -Waggregate-return -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Wcomment -Wconversion \
	-Wdisabled-optimization \
	-Werror -Wfloat-equal -Wformat -Wformat=2 \
	-Wformat-nonliteral -Wformat-security \
	-Wformat-y2k \
	-Wimport -Winit-self -Winline \
	-Winvalid-pch \
	-Wunsafe-loop-optimizations -Wlong-long -Wmissing-braces \
	-Wmissing-field-initializers -Wmissing-format-attribute \
	-Wmissing-include-dirs -Wmissing-noreturn \
	-Wpacked -Wpadded -Wparentheses -Wpointer-arith \
	-Wredundant-decls -Wreturn-type \
	-Wsequence-point -Wshadow -Wsign-compare -Wstack-protector \
	-Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default \
	-Wswitch-enum -Wtrigraphs -Wuninitialized \
	-Wunknown-pragmas -Wunreachable-code -Wunused \
	-Wunused-function -Wunused-label -Wunused-parameter \
	-Wunused-value -Wunused-variable -Wvariadic-macros \
	-Wvolatile-register-var -Wwrite-strings \
	-mmcu=attiny4313 -DF_CPU=19200000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
	gps.c -o gps.c.o

link: norm
	avr-gcc -Os -Wl,--gc-sections -mmcu=attiny4313 -o main.c.elf main.c.o gps.c.o uart.c.o
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 main.c.elf main.c.eep 
	avr-objcopy -O ihex -R .eeprom main.c.elf main.c.hex
	avr-nm --size-sort main.c.elf
	avr-size -C --mcu=attiny4313 main.c.elf

clean:
	rm -f *.elf *.o *.d *.eep *.hex

flash:
	avrdude -vvv -p t4313 -c avrisp2 -P usb -U flash:w:main.c.hex

fuse:
	avrdude -p t4313 -c avrisp2 -P usb -U lfuse:w:0xDF:m -v

readfuse:
	avrdude -p t4313 -c avrisp2 -P usb -U lfuse:r:-:i -v -b 19200

new:
	$(MAKE) fuse
	$(MAKE) norm
	$(MAKE) link
	$(MAKE) flash
