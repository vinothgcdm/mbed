BOARD?=K64F

all:
	mbed compile -t GCC_ARM -m $(BOARD) --source . --source ../../mbed-os --source ../flash-iap-wrapper

clean:
	rm -fr ../../BUILD
