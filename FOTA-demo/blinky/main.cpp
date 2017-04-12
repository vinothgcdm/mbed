#include "mbed.h"
#include "EthernetInterface.h"
#include "HTTPClient.h"
#include "lwip_stack.h"
#include "HTTPFlash.h"

static uint32_t download_area = 0x40000;

int fun(void)
{
    uint32_t *addr = (uint32_t *)download_area;
    uint32_t i;
    uint32_t j;

    for (i = 0; i < 128; i++) {
        printf("%08X ", *(addr++));
        if (((i + 1) % 4) == 0)
            printf("\r\n");
    }
    printf("\r\n");

    for (j = 1; j <= 0; j++) {
        printf("page %lu[%p]\r\n", j, (uint8_t *)addr);
        for (i = 0; i < 512; i++) {
            printf("%c ", *(addr++));
            if (((i + 1) % 32) == 0)
                printf("\r\n");
        }
        printf("\r\n");
    }
    return 0;
}

bool check_success_flag(void)
{
    return false;
}

void download_firmware(void)
{
    printf("============== Download Firmware ===============\r\n");
    EthernetInterface net;
    printf("Connecting IP...\r\n");
    net.connect();
    printf("Getting IP address....\r\n");
    printf("IP address is: %s\r\n", net.get_ip_address());
    printf("Fetching data from server...\r\n");
    HTTPClient http(nsapi_create_stack(&lwip_stack));
    HTTPFlash flash(download_area);
    char url[] = "http://172.16.0.236/out.bin";    
    int ret = http.get(url, &flash);
    if (!ret) {
        printf("Data fetched successfully...\r\n\r\n");
    } else {
        printf("HTTP Error:: ret = %d\r\n", ret);
        printf("HTTP Error:: return code = %d\r\n",
               http.getHTTPResponseCode());
    }

    // fun();
    printf("============== System Restart ===============\r\n");
    NVIC_SystemReset();
}

int main(void)
{
    InterruptIn button(PTB22);
    DigitalOut led(PTB21);

    printf("=============== Blinky Application ===============\r\n");
    while (1) {
        led = !led.read();
        wait_ms(1000);
        if (button.read() == 0)
            download_firmware();
    }
}
