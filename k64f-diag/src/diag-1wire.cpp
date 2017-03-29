#include "mbed.h"
#include "diag.h"

#define DOW PTA4
#define COMMAND 0x33
#define CRC8INIT 0x00
#define CRC8POLY 0x18  /* 0x18 = X^8+X^5+X^4+X^0 */

static Serial serial(USBTX, USBRX);

/* Pulse timings values */
enum {
	A = 6,
	B = 64,
	C = 60,
	D = 10,
	E = 9,
	F = 55,
	G = 0,
	H = 480,
	I = 70,
	J = 410,
};

static DigitalInOut dow(DOW);

/*
 * Pull down the wire for 8 time slot and
 * pull up the wire and wait for device to pull down
 * for a period of 1 time slot.
 */
static bool device_search(void)
{
	serial.printf("Searching for device ...\r\n");
	dow.output();
	dow = 0;
	wait_us(H);
	dow = 1;
	dow.input();
	// time_t start = time(NULL);

	while (true) {
		// time_t current = time(NULL);

		if (!dow)
			return true;

		// if ((current - start) > (C))
		// 	return false;
	}
}

/*
 * pull down the wire for a very short time period
 * and then pull it up
 */
static void strobe_pulse(void)
{
	dow.output();
	dow = 0;
	wait_us(A);
	dow = 1;
}

/*
 * To write single bit to one wire
 */
static void dow_write_bit(bool bit)
{

	if (bit) {
		strobe_pulse();
		wait_us(B);  /* Complete the time slot and 10us recovery */
	} else { /* Write '0' bit */
		dow = 0;
		wait_us(C);
		dow = 1;
		wait_us(D);
	}
}

/*
 * To write a single byte to one wire
 */
static void dow_write_byte(uint8_t byte)
{
	int i;

	for (i = 0; i < 8; i++) {
		dow_write_bit(byte & 0x01);
		byte >>= 1;
	}
}

/*
 * To read a single bit from one wire
 */
static bool dow_read_bit(void)
{
	bool result;

	strobe_pulse();
	dow.input();
	result = dow.read();  /* Sample the bit value from the slave */
	wait_us(F);  /* Complete the time slot and 10us recovery */
	return result;
}

/*
 * To read a single byte from one wire
 */
static uint8_t dow_read_byte(void)
{
	int loop;
	uint8_t result = 0;

	for (loop = 0; loop < 8; loop++) {
		/* shift the result to get it ready for the next bit */
		result >>= 1;
		/* if result is one, then set MS bit */
		if (dow_read_bit())
			result |= 0x80;
	}
	return result;
}

/*
 * To reset the one wire device
 */
static bool dow_reset(void)
{
	int result;

	wait_us(G);
	dow.output();

	dow = 0;
	wait_us(H);

	dow = 1;
	wait_us(I);

	dow.input();
	result = dow.read();  /* Sample for presence pulse from slave */
	wait_us(J);  /* Complete the reset sequence recovery */
	return result;  /* Return sample presence pulse result */
}

/*
 * To calculate crc for the unique id
 */
static uint8_t crc8_bit(uint8_t *data_in, int number_of_bytes_to_read)
{
	uint8_t  crc;
	uint16_t loop_count;
	uint8_t  bit_counter;
	uint8_t  data;
	uint8_t  feedback_bit;

	crc = CRC8INIT;
	for (loop_count = 0; loop_count != number_of_bytes_to_read;
	     loop_count++) {
		data = data_in[loop_count];
		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;

			if (feedback_bit == 0x01)
				crc = crc ^ CRC8POLY;

			crc = (crc >> 1) & 0x7F;

			if (feedback_bit == 0x01)
				crc = crc | 0x80;

			data = data >> 1;
			bit_counter--;
		} while (bit_counter > 0);
	}

	return crc;
}

int one_wire_test(void)
{
	bool found;
	uint8_t buff[8];
	uint8_t crc;

	serial.printf("\r\n----------- ONE WIRE ----------\r\n");
	found = device_search();
	if (found) {
		serial.printf("Device found\r\n");
		dow_reset();
		dow_write_byte(COMMAND);

		for (int i = 0; i < 8; i++)
			buff[i] = dow_read_byte();

		for (int i = 0; i < 8; i++)
			serial.printf("%0X ", buff[i]);
		serial.printf("\r\n");
	}

	crc = crc8_bit(buff, 8);
	if (crc) {
		serial.printf("CRC failed\r\n");
		return FAIL;
	} else {
		serial.printf("CRC OKAY\r\n");
		return PASS;
	}
}
