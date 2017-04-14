#ifndef _FIFO_H_
#define _FIFO_H_

#include "mbed.h"

class FIFO {
public:
        FIFO(char *buf, uint32_t size);
	int fifo_read(char *buf, int len);
	bool fifo_write(const char *buf, int len);
	size_t fifo_get_free_bytes(void);
	size_t fifo_get_len(void);
private:
	char *buf;
	int head;
	int tail;
	int size;
	int free_bytes;
};

#endif
