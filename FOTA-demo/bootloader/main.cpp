#include "mbed.h"
#include "FlashIAP.h"

#define PAGE_SIZE 512
#define SECTOR_SIZE 4096
#define DOWNLOAD_AREA 0x40000
#define APP_AREA 0x20000
#define DOWNLOAD_SUCCESS_FLAG 0x55CC55CC

#define DEBUG "Bootloader"
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

static uint32_t success_flag;
static uint32_t firmware_csum;
static uint32_t firmware_addr;
static uint32_t firmware_len;

bool check_sum_validation(uint32_t *lol_sum)
{
    uint8_t *src = (uint8_t *)DOWNLOAD_AREA + PAGE_SIZE;
    uint32_t sum = 0;

    for (uint32_t i = 0; i < (firmware_len - PAGE_SIZE); i++)
        sum += (src[i] & 0xFF);

    *lol_sum = sum;
    return (sum == firmware_csum);
}

int main(void) {
    INFO("============ Bootloader ============");
    uint32_t *download_area_32ptr = (uint32_t *)DOWNLOAD_AREA;

    success_flag = download_area_32ptr[0];
    firmware_csum = download_area_32ptr[1];
    firmware_addr = download_area_32ptr[2];
    firmware_len = download_area_32ptr[3];
    INFO("Success flag : %p", (uint32_t *)success_flag);
    INFO("Checksum val : %p", (uint32_t *)firmware_csum);
    INFO("Firmware addr: %p", (uint32_t *)firmware_addr);
    INFO("Firmware len : %lu", firmware_len);

    // Download success validataion
    if ((success_flag & 0xFFFFFFFF) != DOWNLOAD_SUCCESS_FLAG)
        mbed_start_application(POST_APPLICATION_ADDR);

    // Checksum validation
    uint32_t lol_sum;
    if (!check_sum_validation(&lol_sum)) {
        ERR("Checksum error (%p != %p)",
            (uint32_t *)firmware_csum, (uint32_t *)lol_sum);
        mbed_start_application(POST_APPLICATION_ADDR);
    } else {
        INFO("Checksum validation done: (%p == %p)",
             (uint32_t *)firmware_csum, (uint32_t *)lol_sum);
    }

    uint32_t count;
    FlashIAP flash;
    DBG("New firmware detected");
    flash.init();
    count = (firmware_len / SECTOR_SIZE);
    count += (firmware_len % SECTOR_SIZE) ? 1 : 0;
    if (0 != flash.erase(APP_AREA, count * SECTOR_SIZE))
        ERR("Flash.erase failed");

    uint8_t *src;
    uint32_t des;
    count = (firmware_len / PAGE_SIZE) + ((firmware_len % PAGE_SIZE) ? 1 : 0);
    for (uint32_t i = 0; i < count; i++) {
        src = (uint8_t *)firmware_addr + ((i + 1) *PAGE_SIZE);
        des = APP_AREA + (i * PAGE_SIZE);
        if (0 != flash.program(src, des, PAGE_SIZE))
            ERR("flash.programe failed");
    }
    DBG("New firmware updated");

    if (0 != flash.erase(DOWNLOAD_AREA, SECTOR_SIZE))  // Erase header data
        ERR("Header data erase failed");

    mbed_start_application(POST_APPLICATION_ADDR);
}
