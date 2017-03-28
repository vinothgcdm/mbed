#include "mbed.h"
#include "diag.h"

DigitalOut led(PTB21);
static int flag = 0;

/*
 * Blinks the specifed LED.
 */
static void blinky(DigitalOut led)
{
	led = 1;
	wait(0.2);
	led = 0;
	wait(0.2);
}


/*
 * Blinks the specified LED, and increments global 'flag'
 * variable to indicate success/failure.
 */
static void print_led(DigitalOut led, int i)
{
	char ch;

	while(1) {
		blinky(led);
		if (pc.readable()) {
			ch = pc.getc();
			if (ch == 'y') {
				pc.printf("LED%d Blinked\r\n", i);
				flag++;
				break;
			} else {
				pc.printf("LED%d not Blinked\r\n", i);
				break;
			}
		}
	}
}

/*
 * Tests the LEDs, returns PASS on success and FAIL on failure.
 */
int led_test(void)
{
	pc.printf("\r\n----------- LED TEST ----------\r\n");

	pc.printf("If LED blinks press (y/n)\r\n");
	print_led(led, 1);

	if (flag == 2) {
		pc.printf("Led Test Passed\r\n");
		return PASS;
	} else {
		pc.printf("Led Test Failed\r\n");
		return FAIL;
	}
}
