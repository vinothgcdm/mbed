all:
	mbed compile -t GCC_ARM -m K64F --source ../mbed-os/ --source .

clean:
	rm -fr ../BUILD

flash:
	cp ../BUILD/K64F/GCC_ARM/mbed-os.bin /media/vinoth/MBED/

.PHONY: all clean

