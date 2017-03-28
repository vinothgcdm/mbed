#include "mbed.h"
#include "diag.h"

struct diag_menu {
	const char *key;
	const char *title;
	int (*item)(void);
};

struct diag_menu menu[] = {{"1", "Test of DGB LEDs", led_test},
			   {"2", "Test of Key", key_test},
			   // {"3", "Test of Bluetooth", bluetooth_test},
			   // {"4", "Test of SD-card", sdcard_test},
			   // {"5", "Test of USB-Host", usb_host_test},
			   // {"6", "Test of I/O Connector", io_conn_test},
			   // {"7", "Test of WiFi", wifi_test},
			   // {"8", "Test of OneWire", one_wire_test},
			   // {"9", "Test of EEPROM", eeprom_test},
			   // {"a", "Test of Connector-loopback", eth_conn_loopback_test},
			   // {"b", "Test of PHY-loopback", eth_phy_loopback_test}
};

int result[LEN(menu)];

/* Draw startup Image */
static void startup_image(void)
{
    /*
     * http://patorjk.com/software/taag/
     * font: slant
     * The string is 'ZCom Diag Test'
     */
    pc.printf("\r\n\n\n _____   ______                   ____  _                ______          __ \r\n");
    pc.printf("/__  /  / ____/___  ____ ___     / __ \\(_)___ _____ _   /_  __/__  _____/ /_\r\n");
    pc.printf("  / /  / /   / __ \\/ __ `__ \\   / / / / / __ `/ __ `/    / / / _ \\/ ___/ __/\r\n");
    pc.printf(" / /__/ /___/ /_/ / / / / / /  / /_/ / / /_/ / /_/ /    / / /  __(__  ) /_  \r\n");
    pc.printf("/____/\\____/\\____/_/ /_/ /_/  /_____/_/\\__,_/\\__, /    /_/  \\___/____/\\__/  \r\n");
    pc.printf("                                            /____/                          \r\n");
}

/*
 * Displays menu and test report
 */
static void display_menu(void)
{
    pc.printf("\r\n\n");
    pc.printf("  %s\r\n", "+==================================+================+");
    pc.printf("  %s\r\n", "|            ZCom Diag             |     Result     |");
    pc.printf("  %s\r\n", "+==================================+================+");
    for (unsigned int i = 0; i < LEN(menu); i++)
	pc.printf("  | %c. %-30s| %-15s|\r\n", *menu[i].key, menu[i].title,
		      (result[i] == PASS) ? "Pass" :
		      (result[i] == FAIL) ? "Fail" : "Not Tested");
    pc.printf("  %s\r\n", "+==================================+================+");
}

/* Read a char from the user and return it */
static char get_user_input(void)
{
    char ch;

    pc.printf("  Enter your option: ");
    ch = pc.getc();
    pc.putc(ch);
    pc.printf("\r\n\n");

    return ch;
}

/* Run user specified diag function */
static void run_diag(char ch)
{
    unsigned int i;
    bool flag = false;

    for (i = 0; i < LEN(menu); i++) {
	if (*menu[i].key == ch) {
	    result[i] = menu[i].item();
	    flag = true;
	    break;
	}
    }

    if (!flag)
	pc.printf("\r\n'%c' is Invalid option...\r\n", ch);
}

int main(void)
{
    char ch;

    pc.baud(115200);
    startup_image();
    while (1) {
	display_menu();
	ch = get_user_input();
	run_diag(ch);
    }
}
