BOARD?=K64F

all:
	mbed compile -t GCC_ARM -m $(BOARD) --source . --source ../../mbed-os/

flash:
	cp ../../BUILD/K64F/GCC_ARM/interrupt-in.bin /media/vinoth/MBED
clean:
	rm -fr ../BUILD
