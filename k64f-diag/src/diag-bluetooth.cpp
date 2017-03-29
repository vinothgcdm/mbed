#include "mbed.h"
#include "diag.h"

#define SIZE 128
#define DELAY 5

Serial bluetooth(PTC4, PTC3);
DigitalOut reset_blue(PTB20);
Timer timer;

/*
 * Tests the bluetooth interface, using AT command.
 * Returns PASS on success, FAIL on failure.
 */
int bluetooth_test(void)
{
	char sbuff[SIZE];

	bluetooth.baud(38400);
	pc.printf("\r\n------- BLUETOOTH TEST -------\r\n");

	printf("Sending AT Command...\r\n");
	bluetooth.puts("AT\r\n");
	wait_ms(DELAY);

	timer.reset();
	timer.start();

	while (timer.read() < DELAY) {
		if (bluetooth.readable()) {
			bluetooth.gets(sbuff, sizeof(sbuff));

			if ((sbuff[0] == 'O') && (sbuff[1] == 'K')) {
				pc.puts("Bluetooth test passed!\r\n");
				return PASS;
			} else {
				pc.puts("Bluetooth test failed!\r\n");
				return FAIL;
			}
		}
	}
	pc.puts("Connection timeout. Bluetooth test failed!\r\n");
	return FAIL;
}

