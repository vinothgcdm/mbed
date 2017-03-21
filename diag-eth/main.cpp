#define DEVICE_ETHERNET 1
#include "mbed.h"
#include "fsl_phy.h"
// #include "cmsis.h"

enum {
	NOT_TESTED,
	PASS,
	FAIL
};

#define MESSAGE "CONNECTOR LOOP BACK MESSAGE \r\n"
#define BUFFSIZE 0X600
#define DP83848C_DEF_ADR 0x0100
#define MII_WR_TOUT 0x00050000
#define MII_RD_TOUT 0x00050000	/* MII Read timeout count */
#define MIND_BUSY 0x00000001
#define MCMD_READ 0x00000001  /* MII Read */
#define TIMEOUT 5 /* Seconds */

static Serial serial(USBTX, USBRX);
static Ethernet eth;
static char buffer[BUFFSIZE] = { 0 };
static char broadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/*
 * To frame a packet for sending
 */
static char *frame_packet(void)
{
	static char packet[BUFFSIZE] = { 0 };

	memcpy(packet, broadcast, 6);
	eth.address(packet + 6);
	packet[12] = 0x08;
	packet[13] = 0x00;
	memcpy(packet + 14, MESSAGE, strlen(MESSAGE));
	return packet;
}

/*
 * To send the frammed packet by ethernet
 */
static void send(void)
{
	int size;

	if (!eth.link())
		return;

	size = 6 + 6 + 2 + strlen(MESSAGE);
	eth.write(frame_packet(), size);
	eth.send();
}

/*
 * To read ethernet packet and store it in a buffer
 */
static char *read(void)
{
	int size;

	if (!eth.link()) {
		serial.printf("Ethernet link down \r\n");
		return NULL;
	}
	size = eth.receive();
	if (size <= 0) {
		serial.printf("Empty packet received \r\n");
		return NULL;
	}

	eth.read(buffer, size);
	return buffer;
}

// /*
//  * To write data to phy chip register
//  */

// static int phy_write(unsigned int phy_reg, unsigned short data)
// {
// 	unsigned int timeout;

// 	LPC_EMAC->MADR = DP83848C_DEF_ADR | phy_reg;
// 	LPC_EMAC->MWTD = data;

// 	/* Wait until operation completed */
// 	for (timeout = 0; timeout < MII_WR_TOUT; timeout++) {
// 		if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
// 			return 0;
// 	}
// 	return -1;
// }

// /*
//  * To read the phy chip register
//  */
// static int phy_read(unsigned int phy_reg)
// {
// 	unsigned int timeout;

// 	LPC_EMAC->MADR = DP83848C_DEF_ADR | phy_reg;
// 	LPC_EMAC->MCMD = MCMD_READ;

// 	/* Wait until operation completed */
// 	for (timeout = 0; timeout < MII_RD_TOUT; timeout++) {
// 		if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
// 			LPC_EMAC->MCMD = 0;
// 			return LPC_EMAC->MRDD;	/* Return a 16-bit value. */
// 		}
// 	}

// 	return -1;
// }

/*
 * To Enable and disable phy loopback
 */
static int set_phy_loopback(int status)
{
    ENET_Type eth_lp;
    int ret = PHY_EnableLoopback(&eth_lp, 0x00, kPHY_LocalLoop, 1);
    serial.printf("Enble: %d\r\n", ret);
    
    // int creg_val = phy_read(0x0);

    //     if (creg_val == -1)
    //     	return creg_val;

    //     if (status)
    //     	return phy_write(0x0, creg_val | 0x4000);
    //     else
    //     	return phy_write(0x0, creg_val & (~0x4000));
}

/*
 * Compare transmit and receive packets in accordance to @test_type loopback
 */
static int eth_send_recv(bool phy_loopback_enable)
{
	char *received = NULL;
	Timer t;
	int ret;

	eth.set_link(eth.FullDuplex100);
	t.reset();
	t.start();

	ret = set_phy_loopback(phy_loopback_enable);

	if (ret == -1) {
		if (phy_loopback_enable == 1)
			serial.printf("\r\n Failed to set phy loopback \r\n");
		else
			serial.printf("\r\n Failed to unset phy loopback \r\n");
		return FAIL;
	}

	while (t.read() < TIMEOUT) {
		send();
		received = read();
		if (received == NULL)
			continue;
		if (strcmp(&received[14], MESSAGE) == 0) {
			serial.printf("Packet received \r\n");
			return PASS;
		}
	}
	return FAIL;
}

/*
 * To test ethernet connector and PHY
 */
static int eth_loopback_test(int type)
{
	if (eth_send_recv(type) == PASS)
		return PASS;
	serial.printf("Loopback test failed!\r\n");
	return FAIL;
}

/*
 * Connector loopback test call back function
 */
int eth_conn_loopback_test(void)
{
	serial.printf("\r\n------- ETHERNET CONNECTOR LOOPBACK  -------\r\n");
	return eth_loopback_test(0);
}

/*
 * PHY loopback test call back function
 */
int eth_phy_loopback_test(void)
{
	serial.printf("\r\n------- ETHERNET PHY LOOPBACK  -------\r\n");
	return eth_loopback_test(1);
}

int main(void)
{
    
    
    eth_conn_loopback_test();
	return 0;
}
