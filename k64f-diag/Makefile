all:
	mbed compile -t GCC_ARM -m K64F --source . --source ../mbed-os/

clean:
	rm -fr ../BUILD
flash:
	cp ../BUILD/K64F/GCC_ARM/k64f-diag.bin /media/vinoth/MBED/
