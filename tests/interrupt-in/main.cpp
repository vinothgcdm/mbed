#include "mbed.h"
 
InterruptIn button(PTB22);
DigitalOut led(PTB21);
 
void flip() {
    printf("Button interrup callback function\r\n");
    for (int i = 0; i < 10; i++) {
        led.read() ? led = 0: led = 1;
        wait_ms(200);
    }
}
 
int main() {
    printf("InterruptIn:\r\n");
    button.rise(&flip);  // attach the address of the flip function to the rising edge
    while(1)
        ;
}
