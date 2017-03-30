#include "mbed.h"
#include "TCPSocket.h"
#include "esp8266-driver/ESP8266Interface.h"

#define KEY1 P2_6
#define KEY2 P2_7

void wifi_init(void)
{
    DigitalOut boot_wifi(P0_22);
    DigitalOut rest_wifi(P1_19);

    wait_us(500); // Approximate time value
    boot_wifi = 0;
    rest_wifi = 0;
    wait_us(20);
    rest_wifi= 1;   
}

AnalogIn ch0(P0_23);

int main()
{
    Serial pc(USBTX, USBRX);
    ESP8266Interface net(P2_8, P2_9, true);

    pc.baud(115200);
    pc.printf("\r\nExample network-socket HTTP client\r\n");
    wifi_init();
    net.connect("Zilogic Quiz", "zilogic123");
    const char *ip = net.get_ip_address();
    const char *mac = net.get_mac_address();
    pc.printf("IP address is: %s\r\n", ip ? ip : "No IP");
    pc.printf("MAC address is: %s\r\n", mac ? mac : "No MAC");

    while (1) {
        pc.printf("Open socket connection\r\n");
        TCPSocket socket;

        int sopen = socket.open(&net);
        if (sopen < 0)
            pc.printf("Soc Open ERR: %d\r\n", sopen);

        int socon = socket.connect("80.243.190.58", 80);
        if (socon < 0)
            pc.printf("Soc Conn ERR: %d\r\n", socon);

        // "GET /input/post.json?json={Key:1}&apikey=1b4e02511d80e5debcc4eea7265f143c\r\nHost: 80.243.190.58\r\n"
        char sbuffer[512] = {0};
        sprintf(sbuffer,
                "GET /feed/insert.json?id=149138&time=UNIXTIME&value=%d&apikey=1b4e02511d80e5debcc4eea7265f143c\r\nHost: 80.243.190.58\r\n",
                ch0.read_u16());
       
        int scount = socket.send(sbuffer, strlen(sbuffer) + 1);
        pc.printf("sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);
   
        char rbuffer[64];
        int rcount = socket.recv(rbuffer, sizeof rbuffer);
        pc.printf("recv %d [%.*s]\r\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
   
        int sclose = socket.close();
        if (sclose < 0)
            pc.printf("Soc Close ERR: %d\r\n", sclose);
    }
    net.disconnect();
    pc.printf("Done\r\n");
}
