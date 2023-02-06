.PHONY: default
default: blink.hex

a.out: blink.c
	avr-gcc -mmcu=atmega328p blink.c 

blink.hex: a.out
	avr-objcopy -O ihex -j.text -j.data a.out blink.hex

flash: blink.hex
	avrdude -v -c /etc/avrdude.conf -p atmega328p -c usbasp -P usb -U flash:w:blink.hex:i
