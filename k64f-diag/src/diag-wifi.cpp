#include "mbed.h"
#include "diag.h"

#define ESP12E_TX PTC17
#define ESP12E_RX PTC16
#define ESP12E_CH_PD PTC2
#define ESP12E_RST PTB23
#define ESP12E_GPIO0 PTC18
#define ESP12E_GPIO2 PTD0
#define ESP12E_GPIO15 PTD1

static Serial wifi(ESP12E_TX, ESP12E_RX);
static DigitalOut ch_pd(ESP12E_CH_PD);
static DigitalOut rst(ESP12E_RST);
static DigitalOut gpio0(ESP12E_GPIO0);
static DigitalOut gpio2(ESP12E_GPIO2);
static DigitalOut gpio15(ESP12E_GPIO15);

static Serial serial(USBTX, USBRX);

static inline int read_wifi(void)
{
    static int count = 0;
    char ch;

    while (wifi.readable()) {
	ch = wifi.getc();
	if (ch == 'O' && count == 0)
	    count++;
	else if (ch == 'K' && count == 1)
	    count++;
	else if (ch == '\r' && count == 2)
	    count++;
	else if (ch == '\n' && count == 3)
	    return 1;
	else
	    count = 0;
    }

    return 0;
}

int wifi_test(void)
{
    Timer t;

    wifi.baud(115200);
    serial.baud(115200);

    /* AT mode */
    ch_pd = 1;
    gpio2 = 1;
    gpio15 = 0;
    gpio0 = 1;

    /* Reset the Wifi module */
    rst = 0;
    wait(1);
    rst = 1;

    wait_us(100);
    serial.printf("\r\n------- WIFI TEST -------\r\n");
    serial.printf("Sending AT Command...\r\n");

    wait(1);
    for (int i = 0; i < 3; i++) {
	wifi.puts("AT\r\n");
	    t.start();
	    while(t.read() < 3) {
		if (read_wifi()) {
		    serial.printf("AT command success\r\n");
		    return 1;
		}
	    }
	    t.stop();
    }
     serial.printf("Connection timeout. Wifi test failed!\r\n");

    return FAIL;
}
