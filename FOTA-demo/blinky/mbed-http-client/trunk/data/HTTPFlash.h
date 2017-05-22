#ifndef HTTPFLASH_H
#define HTTPFLASH_H
#include <mbed.h>
#include "../IHTTPData.h"
#include "FIFO.h"

class HTTPFlash : public IHTTPDataIn {
public:
	HTTPFlash(uint32_t addr, uint32_t page_size, uint32_t sector_size);
	
protected:
	friend class HTTPClient;
	virtual void writeReset();
	virtual int write(const char* buf, size_t len);
	virtual void setDataType(const char* type);
	virtual void setIsChunked(bool chunked);
	virtual void setDataLen(size_t len);
private:
	void write_success_flag(void);

	uint32_t addr;
	uint32_t tot_len;
	char buf[1024];
	FIFO *fifo;
	FlashIAP flash;
	uint32_t page_size;
	uint32_t sector_size;
	char *data;
};

#endif
