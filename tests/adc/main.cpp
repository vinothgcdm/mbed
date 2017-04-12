#include "mbed.h"

#define LEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))

Serial pc(USBTX, USBRX);
AnalogIn s_ch[] = {PTB2, PTB3, PTB10, PTB11};

int main(void)
{
    pc.baud(115200);
    pc.printf("ADC test:\r\n");
    while (1) {
        for (int i = 0; i < LEN(s_ch); i++)
            pc.printf("%d: %4X  ", i, s_ch[i].read_u16());
        pc.printf("\r\n");
        wait(1);
    }
    return 0;
}
