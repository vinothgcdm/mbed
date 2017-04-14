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
    printf("Success flag : 0x%X\r\n", success_flag);
    printf("Firmware addr: 0x%X\r\n", firmware_addr);
    printf("Firmware len : %lu\r\n\n", firmware_len);

    // Download success validataion
    if ((success_flag & 0xFFFFFFFF) != DOWNLOAD_SUCCESS_FLAG)
        mbed_start_application(POST_APPLICATION_ADDR);

    uint32_t count;
    FlashIAP flash;    
    flash.init();
    count = (firmware_len / SECTOR_SIZE) + ((firmware_len % SECTOR_SIZE) ? 1 : 0);
    printf("%lu sectors going to erase\r\n");
    for (uint32_t i = 0; i < count; i++) {
        printf("Erasing sectors %d [0x%X]\r\n", i, APP_AREA + (i * SECTOR_SIZE));
        flash.erase(APP_AREA + (i * SECTOR_SIZE), SECTOR_SIZE);
    }

    uint8_t *src;
    uint32_t des;
    count = (firmware_len / PAGE_SIZE) + ((firmware_len % PAGE_SIZE) ? 1 : 0);
    for (uint32_t i = 0; i < count; i++) {
        src = (uint8_t *)firmware_addr + (PAGE_SIZE * i);
        des = APP_AREA + (PAGE_SIZE * i), PAGE_SIZE;
        printf("Page copy: %d, from %x to %x\r\n", i, src, des);
        flash.program(src, des, PAGE_SIZE);
    }

    flash.erase(DOWNLOAD_AREA, SECTOR_SIZE);  // Erase download_success_flag
    mbed_start_application(POST_APPLICATION_ADDR);
}
