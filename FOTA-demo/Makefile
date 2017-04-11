BOARD?=K64F

all:
	mbed compile -t GCC_ARM -m $(BOARD) --source . --source ../mbed-os/

cbuild:
	mbed compile -t GCC_ARM -m $(BOARD) --source . --source ../mbed-os/ -c

flash:
	cp ../BUILD/K64F/GCC_ARM/eth-down.bin /media/vinoth/MBED

clean:
	rm -fr ../BUILD
