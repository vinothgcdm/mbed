#include "HTTPFlash.h"
#include "FIFO.h"

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
    DBG("Addr: %p\r\n", (uint8_t *)addr);
    DBG("Sector Size: %lu\r\n", sector_size);
    uint32_t page_size = flash.get_page_size();
    DBG("Page Size: %lu\r\n", page_size);
}

void HTTPFlash::writeReset() {}

void HTTPFlash::write_success_flag(void)
{
    char flag[512] = {0x00, 0x11, 0x22, 0x33, 0x44};
    flash.program(flag, this->addr, sizeof(flag));
}

int HTTPFlash::write(const char* buf, size_t len)
{
    static uint32_t byte_count = 0;
    static uint32_t page_count = 1;
    
    fifo.fifo_write(buf, len);
    byte_count += len;
    uint32_t fifo_len = fifo.fifo_get_len();
    if ((fifo_len >= 512) || (byte_count == data_len)) {
        char data[512];
        uint32_t ret = fifo.fifo_read(data, sizeof(data));
        DBG("Page write: %lu[%p] - %lu bytes\r\n", page_count,
            (uint8_t *)this->addr + (page_count * 512), ret);
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
    DBG("Total Bytes & Success flag: %lu & %u\r\n", this->data_len, 512);
    uint32_t tmp = data_len + 512;  // Success flag page
    int count = (tmp / sector_size) + ((tmp % sector_size) ? 1 : 0);
    DBG("%d sectors going to erase\r\n", count);
    for (int i = 0; i < count; i++) {
        DBG("Erasing sectors %d [%p]\r\n", i, (uint8_t *)addr + (i * sector_size));
        flash.erase(addr + (i * sector_size), sector_size);
    }
}

