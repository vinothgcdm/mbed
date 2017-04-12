#include "mbed.h"

DigitalOut led(PTB21);

int main(void)
{
  while (1) {
    led = 1;
    wait(0.2);
    led = 0;
    wait(0.2);
  }
}
