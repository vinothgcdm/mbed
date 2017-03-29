#include "mbed.h"
#include "diag.h"

static DigitalOut gpio[] = {PTC15, PTC13, PTB18, PTC0, PTC5, PTC7, PTC9,
                            PTD3, PTC11, PTD5, PTD7, PTB3, PTB11, PTC12,
                            PTB19, PTC1, PTC6, PTD2, PTC10, PTD4, PTD6,
                            PTB2, PTB10, PTC14};

static Serial serial(USBTX, USBRX);

/*
 * Blink all LEDs in sequentially
 */
static void gpio_conn_run(void)
{
    for (int i = 0; i < LEN(gpio); i++) {
	gpio[i] = 0;
	wait_ms(1000);
	gpio[i] = 1;
	serial.printf(".");
    }
}

/*
 * Initialize all the connector pins
 */
static void pins_init(void)
{
    int i;

    for (i = 0; i < LEN(gpio); i++)
        gpio[i].write(1);  // Set Default state as high (Test board LEDs are inverted)
}

int io_conn_test(void)
{
    char ch;

    serial.printf("\r\n----------- I/O Connector ----------\r\n");
    do {
	pins_init();
	serial.printf("Blinking LEDs ...\r\n");
	gpio_conn_run();
	serial.printf("Done\r\n");
	serial.printf("Did %d LEDs blink? [y]es [n]o [r]epeat: ", LEN(gpio));
	ch = serial.getc();
    } while (ch == 'r');

    if (ch == 'y')
	return PASS;
    else
	return FAIL;
}
