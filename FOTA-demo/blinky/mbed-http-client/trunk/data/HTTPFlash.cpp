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

HTTPFlash::HTTPFlash(uint32_t addr, uint32_t page_size, uint32_t sector_size)
{
	if (addr < (APPLICATION_ADDR + APPLICATION_SIZE))
		ERR("Download address is wrong [%d, %d]",
            APPLICATION_ADDR, APPLICATION_SIZE);

	this->fifo = new FIFO(buf, sizeof(buf));
    this->addr = addr;
    this->page_size = page_size;
    this->sector_size = sector_size;
    this->data = new char(this->page_size);
}

void HTTPFlash::writeReset() {}

int HTTPFlash::write(const char* buf, size_t len)
{
    static uint32_t byte_count = 0;
    static uint32_t page_count = 0;
    uint32_t fifo_len;

    fifo->fifo_write(buf, len);
    byte_count += len;

    fifo_len = fifo->fifo_get_len();
    if (fifo_len >= this->page_size) {
        fifo->fifo_read(data, this->page_size);
        flash.program(data,
                      this->addr + (page_count * this->page_size),
                      this->page_size);
        page_count++;
    } else if (byte_count == tot_len) {
        memset(data, 0xFF, this->page_size);
        fifo->fifo_read(data, fifo_len);
        flash.program(data,
                      this->addr + (page_count * this->page_size),
                      this->page_size);
    }

    if (byte_count == tot_len) {
        DBG("%lu bytes received", byte_count);
        DBG("Download Success");
    }

    return len;
}

void HTTPFlash::setDataType(const char* type) {}
void HTTPFlash::setIsChunked(bool chunked) {}
void HTTPFlash::setDataLen(size_t len)
{
    uint32_t page_count;

    INFO("Downloading firmware ...");
    DBG("Total bytes: %d", len);
    this->tot_len = len;
    this->flash.init();

    /* Erase required sector  */
    page_count = (tot_len / this->sector_size);
    page_count += (tot_len % this->sector_size) ? 1 : 0;
    if (0 != flash.erase(this->addr, page_count * this->sector_size))
        ERR("Flash.erase failed");
}

