#include "HTTPFlash.h"
#include "FIFO.h"

#define DEBUG "HTTPFlash"
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

#define PAGE_SIZE 512
#define SECTOR_SIZE 4096

HTTPFlash::HTTPFlash(uint32_t addr)
{
	if (addr < (APPLICATION_ADDR + APPLICATION_SIZE))
		ERR("Try to download on bootloader area");

	this->fifo = new FIFO(buf, sizeof(buf));
    this->addr = addr;
    DBG("Download area address: 0x%X\r\n", addr);
}

void HTTPFlash::writeReset() {}

void HTTPFlash::write_success_flag(void)
{
    uint32_t flag[128] = {0x55CC55CC, (this->addr + 512), this->tot_len};
    flash.program(flag, this->addr, PAGE_SIZE);
}

int HTTPFlash::write(const char* buf, size_t len)
{
    static uint32_t byte_count = 0;
    static uint32_t page_count = 1;  // for meta information page
    uint32_t fifo_len;
    int ret;
    char data[PAGE_SIZE];

    fifo->fifo_write(buf, len);
    byte_count += len;

    fifo_len = fifo->fifo_get_len();
    if (fifo_len >= PAGE_SIZE) {
        ret = fifo->fifo_read(data, sizeof(data));
        DBG("Page write: %lu[0x%X] - %d bytes\r\n", page_count,
            this->addr + (page_count * PAGE_SIZE), ret);
        flash.program(data, this->addr + (page_count * PAGE_SIZE), PAGE_SIZE);
        page_count++;
    }

    /* Flush remaining all bytes */
    if (byte_count == tot_len) {
        memset(data, 0xFF, sizeof(data));
        ret = fifo->fifo_read(data, sizeof(data));
        DBG("Page write: %lu[0x%X] - %d bytes\r\n", page_count,
            this->addr + (page_count * PAGE_SIZE), ret);
        flash.program(data, this->addr + (page_count * PAGE_SIZE), PAGE_SIZE);
        page_count++;

        DBG("All byte received\r\n");
        write_success_flag();
    }

    return len;
}

void HTTPFlash::setDataType(const char* type) {}
void HTTPFlash::setIsChunked(bool chunked) {}
void HTTPFlash::setDataLen(size_t len)
{
    uint32_t count;
    uint32_t tmp;

    DBG("Total bytes: %lu\r\n", tot_len);
    this->tot_len = tot_len;
    this->flash.init();

    /* Erase required sector  */
    tmp = tot_len + PAGE_SIZE;  // for meta information page
    count = (tmp / SECTOR_SIZE) + ((tmp % SECTOR_SIZE) ? 1 : 0);
    DBG("%lu sectors going to erase\r\n", count);
    for (uint32_t i = 0; i < count; i++) {
        DBG("Erasing sectors %lu [0x%X]\r\n", i,
               this->addr + (i * SECTOR_SIZE));
        flash.erase(this->addr + (i * SECTOR_SIZE), SECTOR_SIZE);
    }
}

