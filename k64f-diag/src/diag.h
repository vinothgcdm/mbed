#ifndef DIAG_H
#define DIAG_H

#define LEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))

enum {
	NOT_TESTED,
	PASS,
	FAIL
};

static Serial pc(USBTX, USBRX, 115200);

int led_test(void);
int key_test(void);
/* int bluetooth_test(void); */
int sdcard_test(void);
/* int usb_host_test(void); */
int io_conn_test(void);
int wifi_test(void);
int one_wire_test(void);
int eeprom_test(void);
/* int eth_conn_loopback_test(void); */
/* int eth_phy_loopback_test(void); */

#endif
