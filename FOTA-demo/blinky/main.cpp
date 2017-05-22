#include "mbed.h"
#include "EthernetInterface.h"
#include "HTTPClient.h"
#include "lwip_stack.h"
#include "HTTPFlash.h"

#define DOWNLOAD_AREA 0x40000
#define PAGE_SIZE 512
#define SECTOR_SIZE 4096

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
    HTTPFlash flash(DOWNLOAD_AREA, PAGE_SIZE, SECTOR_SIZE);
    char url[] = "http://172.16.2.207/out.bin";
    int ret = http.get(url, &flash);
    if (!ret) {
        printf("Data fetched successfully...\r\n\r\n");
    } else {
        printf("HTTP Error:: ret = %d\r\n", ret);
        printf("HTTP Error:: return code = %d\r\n", http.getHTTPResponseCode());
    }

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
        wait_ms(200);
        // for (int i = 0; i < 10; i++) {
        //     led = !led.read();
        //     wait_ms(50);
        // }
        // wait_ms(1000);
        
        if (button.read() == 0)
            download_firmware();
    }
}
