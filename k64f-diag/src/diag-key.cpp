#include "mbed.h"
#include "diag.h"

#define HIGH 1
#define LOW 0

DigitalIn key(PTB22);
static int flag = 0;

/*
 * Reads the specified key and prints its status.
 */
static void key_press(DigitalIn key, int status)
{
	if (pc.getc() == '\r') {
		int read = key.read();
		if (read == LOW)
			pc.printf("KEY Pressed\r\n");
		else
			pc.printf("KEY not pressed\r\n");

		if (read == status)
		  flag++;
	}
}


/*
 * Tests the on-board keys, returns PASS on success, FAIL on failure.
 */
int key_test(void)
{
	pc.printf("\r\n----------- KEY TEST ----------\r\n");

	while (1) {
		pc.printf("Press and hold KEY, then press Enter:\r\n");
		key_press(key, LOW);
		pc.printf("Relese the KEY, then press Enter:\r\n");
		key_press(key, HIGH);

		if (flag == 2) {
			pc.printf("Key Test Passed\r\n");
			return PASS;
		} else {
			pc.printf("Key Test Failed!\r\n");
			return FAIL;
		}
	}
}
