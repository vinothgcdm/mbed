#include "mbed.h"
#include "FIFO.h"

FIFO::FIFO(char *buf, uint32_t size)
{
    this->head = 0;
    this->tail = 0;
    this->size = size;
    this->buf = buf;
    this->free_bytes = size;
}

int FIFO::fifo_read(char *buf, int len)
{
    int i;
    int count = (this->size - this->free_bytes);

    for (i = 0; (i < count) && (i < len); i++) {
        buf[i] = this->buf[this->head];
        this->buf[this->head] = 0;
        this->head++;
        this->head %= this->size;
        this->free_bytes++;
    }
    return i;
}

bool FIFO::fifo_write(const char *buf, int len)
{
    int i;

    if (0 == this->free_bytes)
        return false;

    for (i = 0; (this->free_bytes >= len) && (len--); i++) {
        this->buf[this->tail] = buf[i];
        this->tail++;
        this->tail %= this->size;
        this->free_bytes--;
    }

    return true;
}

size_t FIFO::fifo_get_free_bytes(void)
{
    return this->free_bytes;
}

size_t FIFO::fifo_get_len(void)
{
    return this->size - this->free_bytes;
}
