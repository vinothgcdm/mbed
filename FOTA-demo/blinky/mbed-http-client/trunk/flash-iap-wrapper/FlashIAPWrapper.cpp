#include "FlashIAPWrapper.h"

#define PAGE_SIZE 512
#define SECTOR_SIZE 4096

FlashIAPWrapper::FlashIAPWrapper(uint32_t addr, uint32_t tot_len) {
    uint32_t count;
    uint32_t tmp;
    
    if (addr < 0x20000) {
        printf("Wrong download_area address\r\n");
        return;
    }
    printf("Total bytes: %lu\r\n", tot_len);
    this->tot_len = tot_len;
    this->addr = addr;
    this->flash.init();

    /* Erase required sector  */
    tmp = tot_len + PAGE_SIZE;  // for meta information page
    count = (tmp / SECTOR_SIZE) + ((tmp % SECTOR_SIZE) ? 1 : 0);
    printf("%lu sectors going to erase\r\n", count);
    for (uint32_t i = 0; i < count; i++) {
        printf("Erasing sectors %lu [0x%X]\r\n", i,
               this->addr + (i * SECTOR_SIZE));
        flash.erase(this->addr + (i * SECTOR_SIZE), SECTOR_SIZE);
    }
}

void FlashIAPWrapper::write_success_flag(void)
{
    uint32_t flag[128] = {0x55CC55CC, (this->addr + 512), this->tot_len};
    flash.program(flag, this->addr, PAGE_SIZE);
}

int FlashIAPWrapper::write(const char buf[], uint32_t len) {
    static uint32_t byte_count = 0;
    static uint32_t page_count = 1;  // for meta information page
    uint32_t fifo_len;
    int ret;
    char data[PAGE_SIZE];

    fifo.fifo_write(buf, len);
    byte_count += len;

    fifo_len = fifo.fifo_get_len();
    if (fifo_len >= PAGE_SIZE) {
        ret = fifo.fifo_read(data, sizeof(data));
        printf("Page write: %lu[0x%X] - %d bytes\r\n", page_count,
            this->addr + (page_count * PAGE_SIZE), ret);
        flash.program(data, this->addr + (page_count * PAGE_SIZE), PAGE_SIZE);
        page_count++;
    }

    /* Flush remaining all bytes */
    if (byte_count == tot_len) {
        memset(data, 0xFF, sizeof(data));
        ret = fifo.fifo_read(data, sizeof(data));
        printf("Page write: %lu[0x%X] - %d bytes\r\n", page_count,
            this->addr + (page_count * PAGE_SIZE), ret);
        flash.program(data, this->addr + (page_count * PAGE_SIZE), PAGE_SIZE);
        page_count++;

        printf("All byte received\r\n");
        write_success_flag();
    }

    return len;
}
