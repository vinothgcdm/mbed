all: blinky
	cp ../BUILD/K64F/GCC_ARM/blinky.bin blinky.bin

blinky: bootloader
	rm -fr ../BUILD
	make -C blinky

bootloader: clean
	make -C bootloader
	cp ../BUILD/K64F/GCC_ARM/bootloader.bin .

clean:
	rm -fr ../BUILD
	find . -name "*.bin" -exec rm {} ";"

flash:
	cp blinky.bin /media/vinoth/MBED/

.PHONY: all blinky bootloader clean
