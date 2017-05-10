#ifndef _FIFO_H_
#define _FIFO_H_

#include "mbed.h"

class FIFO {
public:
        FIFO(char *buf, uint32_t size);
	int read(char *buf, int len);
	bool write(const char *buf, int len);
	size_t get_free_bytes(void);
	size_t get_len(void);
private:
	char *buf;
	int head;
	int tail;
	int size;
	int free_bytes;
};

#endif
