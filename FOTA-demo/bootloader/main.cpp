#include "mbed.h"
#include "FlashIAP.h"

#define PAGE_SIZE 512
#define SECTOR_SIZE 4096
#define DOWNLOAD_AREA 0x40000
#define APP_AREA 0x20000
#define DOWNLOAD_SUCCESS_FLAG 0x55CC55CC

int main(void) {
    printf("============ Bootloader ============\r\n");
    uint32_t success_flag;
    uint32_t firmware_addr;
    uint32_t firmware_len;
    uint32_t *download_area_32ptr = (uint32_t *)DOWNLOAD_AREA;

    success_flag = download_area_32ptr[0];
    firmware_addr = download_area_32ptr[1];
    firmware_len = download_area_32ptr[2];
    printf("Success flag : %p\r\n", (uint32_t *)success_flag);
    printf("Firmware addr: %p\r\n", (uint32_t *)firmware_addr);
    printf("Firmware len : %lu\r\n\n", firmware_len);

    uint32_t *ptr = (uint32_t *)0x40000;
    for (int i = 0; i < 70; i++) {
      printf("%08x ", ptr[i]);
      if (((i + 1) % 4) == 0)
	printf("\r\n");
    }
    
    // Download success validataion
    if ((success_flag & 0xFFFFFFFF) != DOWNLOAD_SUCCESS_FLAG)
        mbed_start_application(POST_APPLICATION_ADDR);

    uint32_t count;
    FlashIAP flash;

    flash.init();
    count = (firmware_len / SECTOR_SIZE);
    count += (firmware_len % SECTOR_SIZE) ? 1 : 0;
    printf("%lu sectors going to erase\r\n", count);
    if (0 != flash.erase(APP_AREA, count * SECTOR_SIZE))
        printf("Flash.erase failed\r\n");

    uint8_t *src;
    uint32_t des;
    count = (firmware_len / PAGE_SIZE) + ((firmware_len % PAGE_SIZE) ? 1 : 0);
    for (uint32_t i = 0; i < count; i++) {
        src = (uint8_t *)firmware_addr + (PAGE_SIZE * i);
        des = APP_AREA + (i * PAGE_SIZE);
        if (0 != flash.program(src, des, PAGE_SIZE))
            printf("flash.programe failed\r\n");
    }

    if (0 != flash.erase(DOWNLOAD_AREA, SECTOR_SIZE))  // Erase header data
        printf("Header data erase failed\r\n");

    mbed_start_application(POST_APPLICATION_ADDR);
}
