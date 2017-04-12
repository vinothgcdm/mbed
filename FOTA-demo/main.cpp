#include "mbed.h"
#include "EthernetInterface.h"
#include "HTTPClient.h"
#include "lwip_stack.h"
#include "HTTPFlash.h"

int fun(void)
{
    uint8_t *addr = (uint8_t *)0x80000;
    uint32_t i;
    uint32_t j;

    for (i = 0; i < 512; i++) {
        printf("%02X ", *(addr++));
        if (((i + 1) % 32) == 0)
            printf("\r\n");
    }
    printf("\r\n");

    for (j = 1; j <= 25; j++) {
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

int main(void)
{
    printf("============== HTTPS Example ===============\r\n");
    EthernetInterface net;
    printf("Connecting IP...\r\n");
    net.connect();
    printf("Getting IP address....\r\n");
    printf("IP address is: %s\r\n", net.get_ip_address());
    printf("Fetching data from server...\r\n");
    HTTPClient http(nsapi_create_stack(&lwip_stack));
    HTTPFlash flash(0x80000);
    char buff[] = "http://172.16.0.236/hello.txt";
    int ret = http.get(buff, &flash);
    if (!ret) {
        printf("Data fetched successfully...\r\n\r\n");
    } else {
        printf("HTTP Error:: ret = %d\r\n", ret);
        printf("HTTP Error:: return code = %d\r\n",
               http.getHTTPResponseCode());
    }

    fun();
    while (1);
    
    NVIC_SystemReset();
    return 0;
}

