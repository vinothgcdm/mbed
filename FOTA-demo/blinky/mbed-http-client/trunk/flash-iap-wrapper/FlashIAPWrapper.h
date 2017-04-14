#ifndef FLASHIAP_WRAPPER_H
#define FLASHIAP_WRAPPER_H

#include "FlashIAP.h"
#include "FIFO.h"
#include "mbed.h"

class FlashIAPWrapper {
public:
	FlashIAPWrapper(uint32_t flash_addr, uint32_t tot_len);
        int write(const char buf[], uint32_t len);

private:
	void write_success_flag(void);
	
	uint32_t addr;
	uint32_t tot_len;
	char buf[1024];
	FIFO fifo = FIFO(buf, sizeof(buf));
	FlashIAP flash;
};

#endif
