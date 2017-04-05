#include "mbed.h"
DigitalOut led1(PTB21);

int main(void) {
  while (true) {
    printf("#");
    led1 = !led1;
    wait_ms(100);
  }
}
