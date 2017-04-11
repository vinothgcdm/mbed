#include "mbed.h"
#include "fifo.h"

void fifo_init(fifo_t *fifo, char *buf, int size)
{
    fifo->head = 0;
    fifo->tail = 0;
    fifo->size = size;
    fifo->buf = buf;
    fifo->free_bytes = size;
}

int fifo_read(fifo_t *fifo, char *buf, int len)
{
    int i;
    int count = (fifo->size - fifo->free_bytes);

    for (i = 0; (i < count) && (i < len); i++) {
        buf[i] = fifo->buf[fifo->head];
        fifo->buf[fifo->head] = 0;
        fifo->head++;
        fifo->head %= fifo->size;
        fifo->free_bytes++;
    }
    return i;
}

bool fifo_write(fifo_t *fifo, const char *buf, int len)
{
    int i;

    if (0 == fifo->free_bytes)
        return false;

    for (i = 0; (fifo->free_bytes >= len) && (len--); i++) {
        fifo->buf[fifo->tail] = buf[i];
        fifo->tail++;
        fifo->tail %= fifo->size;
        fifo->free_bytes--;
    }

    return true;
}

size_t fifo_get_free_bytes(fifo_t *fifo)
{
    return fifo->free_bytes;
}

size_t fifo_get_len(fifo_t *fifo)
{
    return fifo->size - fifo->free_bytes;
}
