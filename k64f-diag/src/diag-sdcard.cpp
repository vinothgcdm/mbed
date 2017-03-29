#include "mbed.h"
#include "diag.h"

#define TIMEOUT 5000

SPI spi(PTE3, PTE1, PTE2);
DigitalOut cs(PTE4);

/*
 * Sends a SD-Card command and receives response on the SPI interface.
 * Returns the response on sucess and -1 on failure.
 */
static int cmd(int cmd, int arg)
{
	int i;

	cs = 0;

	// send command
	spi.write(0x40 | cmd);
	spi.write(arg >> 24);
	spi.write(arg >> 16);
	spi.write(arg >> 8);
	spi.write(arg >> 0);
	spi.write(0x95);

	for (i = 0; i < TIMEOUT; i++) {
		int response = spi.write(0xFF);

		if (!(response & 0x80)) {
			cs = 1;
			spi.write(0xFF);
			return response;
		}
	}

	cs = 1;
	spi.write(0xFF);
	return -1;
}

/*
 * Tests SD-Card and return PASS on success, and FAIL on failure.
 */
int sdcard_test(void)
{
	int i;

	spi.frequency(100000);
	cs = 1;

	for (i = 0 ; i < 16; i++) {
		spi.write(0xFF);
	}

	if (cmd(0, 0) == 1) {
		printf("SDcard Detected\r\n");
		return PASS;
	}
	printf("SDcard not detected\r\n");
	return FAIL;
}
