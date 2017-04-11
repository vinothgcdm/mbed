#include "HTTPFlash.h"
#include "fifo.h"

#define DEBUG "HTfi"
#include <cstdio>
#if (defined(DEBUG) && !defined(TARGET_LPC11U24))
#define DBG(x, ...)  std::printf("[DBG %s %3d] "x"\r\n", DEBUG, __LINE__, ##__VA_ARGS__);
#define WARN(x, ...) std::printf("[WRN %s %3d] "x"\r\n", DEBUG, __LINE__, ##__VA_ARGS__);
#define ERR(x, ...)  std::printf("[ERR %s %3d] "x"\r\n", DEBUG, __LINE__, ##__VA_ARGS__);
#define INFO(x, ...) std::printf("[INF %s %3d] "x"\r\n", DEBUG, __LINE__, ##__VA_ARGS__);
#else
#define DBG(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#define INFO(x, ...)
#endif

HTTPFlash::HTTPFlash(uint32_t addr)
{
    
    flash.init();
    this->addr = addr;
    this->sector_size = flash.get_sector_size(addr);
    DBG("Addr: 0x%X\r\n", addr);
    DBG("Sector Size: %d\r\n", sector_size);

    fifo_init(&fifo, buf, sizeof(buf));
    // this->page_size = flash.get_page_size();
    // DBG("Page Size: %d\r\n", page_size);
    // this->buf = NULL;
}

void HTTPFlash::writeReset() {}

void HTTPFlash::write_success_flag(void)
{
    char flag[512] = {0x00, 0x11, 0x22, 0x33, 0x44};
    flash.program(flag, this->addr, sizeof(flag));
}

int HTTPFlash::write(const char* buf, size_t len)
{
    static size_t byte_count = 0;
    static size_t page_count = 1;
    
    fifo_write(&fifo, buf, len);
    byte_count += len;
    size_t fifo_len = fifo_get_len(&fifo);
    if ((fifo_len >= 512) || (byte_count == data_len)) {
        char data[512];
        size_t ret = fifo_read(&fifo, data, sizeof(data));
        DBG("Page Count: %d[%p] - %d bytes\r\n", page_count,
            this->addr + (page_count * 512), ret);
        flash.program(data, this->addr + (page_count * 512), 512);
        page_count++;
    }

    if (byte_count == data_len) {
        DBG("All byte received\r\n");
        write_success_flag();
    }
        
    return len;
}

void HTTPFlash::setDataType(const char* type) {}
void HTTPFlash::setIsChunked(bool chunked) {}
void HTTPFlash::setDataLen(size_t len)
{
    this->data_len = len;
    DBG("Total Bytes & Success flag: %d & %d\r\n", this->data_len, 512);
    uint32_t tmp = data_len + 512;
    int count = (tmp / sector_size) + ((tmp % sector_size) ? 1 : 0);
    DBG("%d sectors going to erase\r\n", count);
    for (int i = 0; i < count; i++) {
        DBG("Erasing sectors %d [%p]\r\n", i, addr + (i * sector_size));
        flash.erase(addr + (i * sector_size), sector_size);
    }
}

