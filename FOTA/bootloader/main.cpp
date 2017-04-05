#include "mbed.h"
DigitalOut led1(PTB21);

int main(void) {
  printf("POST_APPLICATION_ADDR: %x\r\n", POST_APPLICATION_ADDR);
  printf("APPLICATION_SIZE     : %x\r\n\n", APPLICATION_SIZE);
  
  for (int i = 0; i < 6; i++) {
    led1 = !led1;
    wait(1);
  }

  mbed_start_application(POST_APPLICATION_ADDR);
}
