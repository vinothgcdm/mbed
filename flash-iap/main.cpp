#include "mbed.h"

int main(void)
{
    uint8_t buf[512];
    uint32_t addr = 0x10000;
    FlashIAP flash;
    int ret;

    flash.init();

    printf("get_sector_size: %d\r\n", flash.get_sector_size(0x100));
    printf("get_sector_size: %d\r\n", flash.get_sector_size(0x10000));	
    printf("get_flash_start: %d\r\n", flash.get_flash_start());
    printf("get_flash_size : %d\r\n", flash.get_flash_size());
    printf("get_page_size  : %d\r\n", flash.get_page_size());

    ret = flash.read(buf, addr, sizeof(buf));
    printf("Read : %d\r\n", ret);

    ret = flash.erase(addr, 4096);
    printf("Erase: %d\r\n", ret);
    
    buf[0] += 1;
    ret = flash.program(buf, addr, sizeof(buf));
    printf("Prog : %d\r\n", ret);
    printf("RESET: %d\r\n", buf[0]);
    printf("\r\n\n");
    
    flash.deinit();
    return 0;
}
