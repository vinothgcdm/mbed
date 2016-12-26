#include "mbed.h"

#define LED1 PTB21

int main(void)
{
    DigitalOut led(LED1);

    while (1) {
        led = !led.read();
        wait(1);
    }
    
    return 0;
}
