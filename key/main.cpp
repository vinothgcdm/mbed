#include "mbed.h"

int main()
{
	Serial serial(USBTX, USBRX);
	serial.baud(115200);
	DigitalIn key(PTB22);

	while (1) {
		serial.printf("Key Status: %d\r\n", key.read());
		wait(1);
	}
	return 0;
}
