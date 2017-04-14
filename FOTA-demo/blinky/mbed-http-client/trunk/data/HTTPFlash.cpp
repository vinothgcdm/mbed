#include "HTTPFlash.h"
#include "../flash-iap-wrapper/FlashIAPWrapper.h"

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
    this->flash = NULL;
    this->addr = addr;
    DBG("Download area address: 0x%X\r\n", addr);
}

void HTTPFlash::writeReset() {}

int HTTPFlash::write(const char* buf, size_t len)
{
    if (flash == NULL)
        return -1;

    flash->write(buf, len);
    return len;
}

void HTTPFlash::setDataType(const char* type) {}
void HTTPFlash::setIsChunked(bool chunked) {}
void HTTPFlash::setDataLen(size_t len)
{
    static FlashIAPWrapper tmp(this->addr, len);
    this->flash = &tmp;
    printf("FlashIAPWrapper created\r\n");
}

