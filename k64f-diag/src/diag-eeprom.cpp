#include "mbed.h"
#include "diag.h"

#define EEPROM_ADDR 0xA0
#define EEPROM_SDA PTE25
#define EEPROM_SCL PTE24

static I2C eeprom(EEPROM_SDA, EEPROM_SCL);
static Serial serial(USBTX, USBRX);

/*
 * To read a single byte from i2c eeprom
 */
static int eeprom_read(int addr)
{
	uint8_t data;

	eeprom.start();
	if (!eeprom.write(EEPROM_ADDR))
		goto dev_fail;
	if (!eeprom.write(addr))
		goto data_fail;
	eeprom.start();
	if (!eeprom.write(EEPROM_ADDR | 1))
		goto dev_fail;
	data = eeprom.read(0);
	eeprom.stop();
	return PASS;

 dev_fail:
	eeprom.stop();
	serial.printf("Dev Not Found!\r\n");
	return FAIL;

 data_fail:
	eeprom.stop();
	serial.printf("Data read failed!\r\n");
	return FAIL;
}

int eeprom_test(void)
{
  return eeprom_read(0x10);
}
