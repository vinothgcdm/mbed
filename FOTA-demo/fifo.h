#ifndef _FIFO_H_
#define _FIFO_H_

#include "mbed.h"

typedef struct {
	char *buf;
	int head;
	int tail;
	int size;
	int free_bytes;
} fifo_t;

void fifo_init(fifo_t *fifo, char *buf, int size);
int fifo_read(fifo_t *fifo, char *buf, int len);
bool fifo_write(fifo_t *fifo, const char *buf, int len);
size_t fifo_get_free_bytes(fifo_t *fifo);
size_t fifo_get_len(fifo_t *fifo);

#endif
