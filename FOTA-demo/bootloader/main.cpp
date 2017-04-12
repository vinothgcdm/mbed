#include "mbed.h"
#include "FlashIAP.h"

DigitalOut led1(PTB21);

int main(void) {
    printf("============ Enter Bootloader ============\r\n");
    // printf("POST_APPLICATION_ADDR: %x\r\n", POST_APPLICATION_ADDR);
    // printf("APPLICATION_SIZE     : %x\r\n\n", APPLICATION_SIZE);

    // FlashIAP flash;
    // flash.init();
    // uint8_t buf[512];
    // memcpy(buf, 0x00000, 512);
    // flash.program(buf, 0x40000, 512);

    uint32_t success_flag;
    uint32_t success_flag_val;
    uint32_t firmware_addr;
    uint32_t firmware_len;
    uint32_t *download_area_32ptr = (uint32_t *)0x40000;

    success_flag_val = 0x55CC55CC;
    success_flag = download_area_32ptr[0];
    firmware_addr = download_area_32ptr[1];
    firmware_len = download_area_32ptr[2];
    printf("Success flag : 0x%X\r\n", success_flag);
    printf("Firmware addr: 0x%X\r\n", firmware_addr);
    printf("Firmware len : %d\r\n\n", firmware_len);

    if (0 != memcmp(&success_flag, &success_flag_val, 4))
        mbed_start_application(POST_APPLICATION_ADDR);
        
    FlashIAP flash;
    uint32_t count;
    uint8_t *download_area_8ptr = (uint8_t *)0x40000;
    uint8_t *app_area_8ptr = (uint8_t *)0x20000;
    
    flash.init();
    count = (firmware_len / 4096) + ((firmware_len % 4096) ? 1 : 0);
    printf("%d sectors going to erase\r\n", count);    
    for (uint32_t i = 0; i < count; i++) {
        printf("Erasing sectors %d [%p]\r\n", i, app_area_8ptr + (i * 4096));
        flash.erase(0x20000 + (i * 4096), 4096);
    }

    uint8_t buf[512];
    count = (firmware_len / 512) + ((firmware_len % 512) ? 1 : 0);
    printf("%d pages going to copy download_area to application_area\r\n", count);
    for (uint32_t i = 0; i < count; i++) {
        printf("Page copy: %d, from %x to %x\r\n", i, firmware_addr + (512 * i),
               0x20000 + (512 * i));
        memcpy(buf, (uint8_t *)firmware_addr + (512 * i), 512);
        flash.program(buf, 0x20000 + (512 * i), 512);
    }
    
    // for (uint32_t i = 0; i < count; i++) {
    //     printf("Page %d[%x]:\r\n", i, 0x20000 + (512 * i));
    //     for (uint32_t j = 0; j < 512; j++) {
    //         printf("%c", *((uint8_t *)0x20000 + (512 * i) + j));
    //     }
    //     printf("\r\n");
    // }

    flash.erase(0x40000, 4096);  // Erase download success flag
    printf("============ Exit Bootloader ============\r\n");
    mbed_start_application(POST_APPLICATION_ADDR);
}
