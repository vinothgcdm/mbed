BOARD?=K64F

all:
	mbed compile -t GCC_ARM -m $(BOARD) --source . --source ../../mbed-os

clean:
	rm -fr ../../BUILD

flash:
	cp ../BUILD/$(BOARD)/GCC_ARM/bootloader.bin 
