// #include "mbed.h"
// #include "esp8266-driver/ESP8266/ATParser/ATParser.h"

// Serial pc(USBTX, USBRX);
// BufferedSerial wifi(P2_8, P2_9, 1024);
// ATParser parser(wifi);

// void wifi_init(void)
// {
//    DigitalOut boot_wifi(P0_22);
//    DigitalOut rest_wifi(P1_19);

//    wait_us(500); // Approximate time value
//    boot_wifi = 0;
//    rest_wifi = 0;
//    wait_us(20);
//    rest_wifi= 1;   
// }

// void pc_callback(void) {
//     static char ch;
//     if (pc.readable()) {
//         static int count = 0;
//         char ch = pc.getc();
//         if (ch == '\r') {
//             wifi.putc('\r');
//             wifi.putc('\n');
//         } else {
//             wifi.putc(ch);   
//         }
//     }
// }

// void wifi_callback(void) {
//     if (wifi.readable())
//         pc.putc(wifi.getc());
// }

// void goto_at_mode(void)
// {
//     if (pc.readable())
//         if (pc.getc() == '#') {
//             pc.printf("\r\nEntered into AT mode\r\n");
//             pc.attach(&pc_callback);
//             wifi.attach(&wifi_callback);
//             pc.puts("AT\r");
//             while (1)
//                 ;
//         }
// }

// int main()
// {
//     pc.attach(goto_at_mode);
//     pc.baud(115200);
//     wifi.baud(115200);
//     parser.debugOn(1);
//     wifi_init();

//     pc.printf("\r\nATParser:\r\n");
//     pc.printf("---------\r\n");

//     bool ret = parser.send("AT+RST")
//             && parser.recv("OK\r\nready");
//     pc.printf(">> RST: %d\r\n", ret);

//     ret = parser.send("AT+CWMODE=%d", 3)
//             && parser.recv("OK")
//             && parser.send("AT+CIPMUX=1")
//             && parser.recv("OK");
//     pc.printf(">> StartUp: %d\r\n", ret);

//     ret = parser.send("AT+CWDHCP=%d,%d", true?1:0, 1)
//             && parser.recv("OK");
//     pc.printf(">> CHCP: %d\r\n", ret);

//     char ap[] = "Zilogic Quiz";
//     char passPhrase[] = "zilogic123";
//     ret = parser.send("AT+CWJAP=\"%s\",\"%s\"", ap, passPhrase)
//             && parser.recv("OK");
//     pc.printf(">> Conn AP: %d\r\n", ret);

//     char _ip_buffer[30];
//     parser.send("AT+CIFSR")
//             && parser.recv("+CIFSR:STAIP,\"%15[^\"]\"", _ip_buffer)
//             && parser.recv("OK");
//     pc.printf(">> IP ADD: %s\r\n", _ip_buffer);

//     char _mac_buffer[30];
//     parser.send("AT+CIFSR")
//             && parser.recv("+CIFSR:STAMAC,\"%17[^\"]\"", _mac_buffer)
//             && parser.recv("OK");
//     pc.printf(">> MAC ADD: %s\r\n", _mac_buffer);

//     for (int i = 1; 1; i++) {
//         printf("\r\n>> PACK %d\r\n", i);
//         int id = 0;
//         char type[] = "TCP";
//         char addr[] = "80.243.190.58";
//         int port = 80;
//         ret = parser.send("AT+CIPSTART=%d,\"%s\",\"%s\",%d", id, type, addr, port)
//                 && parser.recv("OK");
//         printf(">> TCP Conn: %d\r\n", ret);

//         char data[255];
//         sprintf(data, "GET /input/post.json?json={Key:%d}&apikey=1b4e02511d80e5debcc4eea7265f143c\r\nHost: 80.243.190.58\r\n", i);
//         int amount = strlen(data);
//         ret = parser.send("AT+CIPSEND=%d,%d", id, amount)
//                 && parser.recv(">")
//                 && parser.write((char*)data, (int)amount)
//                 && parser.recv("CLOSED");
//         printf(">> Send: %d\r\n", ret);
//     }
//     printf("END\r\n\n");
                
//     return 0;   
// }

