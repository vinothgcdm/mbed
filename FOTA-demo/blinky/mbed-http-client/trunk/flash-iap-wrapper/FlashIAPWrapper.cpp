#include "FlashIAPWrapper.h"

#define PAGE_SIZE 512
#define SECTOR_SIZE 4096
#define FLAS_CONFIG_RANGE 0x20000

FlashIAPWrapper::FlashIAPWrapper(uint32_t addr, uint32_t tot_len, uint32_t magic_word) {
    if (addr < FLAS_CONFIG_RANGE) {
        printf("Wrong download_area address\r\n");
        return;
    }

    printf("Total bytes: %lu\r\n", tot_len);
    this->magic_word = magic_word;
    this->tot_len = tot_len;
    this->addr = addr;
    if (this->flash.init()) {
        printf("Flas init failed\r\n");
        return;
    }
}

void FlashIAPWrapper::erase_sectors(uint32_t byte_count)
{
    static uint32_t sector_count = 0;
    uint32_t bytes = byte_count + PAGE_SIZE;  // for meta information page

    if (bytes > (sector_count * SECTOR_SIZE)) {
        printf("Erasing sectors %lu [0x%X]\r\n", sector_count,
               this->addr + (sector_count * SECTOR_SIZE));
        if (flash.erase(this->addr + (sector_count * SECTOR_SIZE),
                         SECTOR_SIZE)) {
            printf("Erase faild\r\n");
            return;
        }
        sector_count++;
    }
}

void FlashIAPWrapper::write_success_flag(void)
{
    uint32_t flag[128] = {this->magic_word, (this->addr + 512), this->tot_len};
    if (flash.program(flag, this->addr, PAGE_SIZE))
        printf("Write download success flag failed\r\n");
    uint32_t *tmp = (uint32_t *)0x40000;
    printf("Success Flag: %x %x %x\r\n", tmp[0], tmp[1], tmp[2]);
}

int FlashIAPWrapper::write(const char buf[], uint32_t len) {
    static uint32_t byte_count = 0;
    static uint32_t page_count = 1;  // for meta information page
    uint32_t fifo_len;
    int ret;
    char data[PAGE_SIZE];

    fifo.write(buf, len);
    byte_count += len;
    printf("byte count: %lu\r\n", byte_count);

    erase_sectors(byte_count);

    fifo_len = fifo.get_len();
    if (fifo_len >= PAGE_SIZE) {
        ret = fifo.read(data, sizeof(data));
        // printf("Page write: %lu[0x%X] - %d bytes\r\n", page_count,
            // this->addr + (page_count * PAGE_SIZE), ret);
        if (flash.program(data, this->addr + (page_count * PAGE_SIZE), PAGE_SIZE)) {
            printf("Data write failed\r\n");
            return -1;
        }
        page_count++;
    }

    /* Flush remaining all bytes */
    if (byte_count == tot_len) {
        memset(data, 0xFF, sizeof(data));
        ret = fifo.read(data, sizeof(data));
        printf("Page write: %lu[0x%X] - %d bytes\r\n", page_count,
            this->addr + (page_count * PAGE_SIZE), ret);
        if (flash.program(data, this->addr + (page_count * PAGE_SIZE), PAGE_SIZE)) {
            printf("Last data write failed\r\n");
            return -2;
        }
        page_count++;

        printf("All byte received\r\n");
        write_success_flag();
    }

    return len;
}
