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

#endif
