#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <i2c-bb.h>
#include <event.h>
#include "mma8452q.h"

#define MMA8452Q_ADDR 0x1F
#define HELTH_BIT 3
#define ACC_ACTIVE 1
#define ACC_DEAD 0
#define ACC_READ 1
#define ACC_WRITE 0

enum MMA8452Q_REGISTERS {
	STATUS       = 0x00,

	OUT_X_MSB    = 0x01,
	OUT_X_LSB    = 0x02,

	OUT_Y_MSB    = 0x03,
	OUT_Y_LSB    = 0x04,

	OUT_Z_MSB    = 0x05,
	OUT_Z_LSB    = 0x06,

	SYSMOD       = 0x0B,
	INT_SOURCE   = 0x0C,
	WHO_AM_I     = 0x0D,
	XYZ_DATA_CFG = 0x0E,

	PL_STATUS    = 0x10,
	PL_CFG       = 0x11,
	PL_COUNT     = 0x12,

	PULSE_CFG    = 0x21,
	PULSE_SRC    = 0x22,
	PULSE_THSX   = 0x23,
	PULSE_THSY   = 0x24,
	PULSE_THSZ   = 0x25,
	PULSE_TMLT   = 0x26,
	PULSE_LTCY   = 0x27,
	CTRL_REG1    = 0x2A,
	CTRL_REG2    = 0x2B,
	CTRL_REG3    = 0x2C,
	CTRL_REG4    = 0x2D,
	CTRL_REG5    = 0x2E,

	OFF_X        = 0x2F,
	OFF_Y        = 0x30,
	OFF_Z        = 0x31
};

enum MMA8452Q_STATUS {
	ZYX_OW = 0x80,
	Z_OW   = 0x40,
	Y_OW   = 0x20,
	X_OW   = 0x10,
	ZYX_DR = 0x08,
	Z_DR   = 0x04,
	Y_DR   = 0x02,
	X_DR   = 0x01,
};

enum MMA8452Q_INT_SOURCE {
	SRC_ASLP = 0x80,
	SRC_TRANS = 0x20,
	SRC_LNDPRT = 0x10,
	SRC_PULSE = 0x08,
	SRC_FF_MT = 0x04,
	SRC_DRDY = 0x01
};

enum MMA8452Q_FS {
	FS_2G = 0,
	FS_4G = 1,
	FS_8G = 2,
	FS_SHIFT = 0,
	FS_MASK = 0x03
};

enum MMA8452Q_CTRL_REG1 {
	ACTIVE = 0,
	F_READ = 1,
	LNOISE = 2,
	DR0    = 3,
	DR1    = 4,
	DR2    = 5
};

enum MMA8452Q_CTRL_REG2 {
	MODS0  = 0,
	MODS1  = 1,
	SLPE   = 2,
	SMODS0 = 3,
	SMODS1 = 4,
	RST    = 6,
	ST     = 7
};

enum MMA8452Q_CTRL_REG3 {
	PP_OD       = 0,
	IPOL        = 1,
	WAKE_FF_MT  = 3,
	WAKE_PULSE  = 4,
	WAKE_LNDPRT = 5,
	WAKE_TRANS  = 6
};

enum MMA8452Q_CTRL_REG4 {
	INT_EN_DRDY   = 0,
	INT_EN_FF_MT  = 2,
	INT_EN_PULSE  = 3,
	INT_EN_LNDPRT = 4,
	INT_EN_TRANS  = 5,
	INT_EN_ASLP   = 7
};

enum MMA8452Q_CTRL_REG5 {
	INT_CFG_DRDY   = 0,
	INT_CFG_FF_MT  = 2,
	INT_CFG_PULSE  = 3,
	INT_CFG_LNDPRT = 4,
	INT_CFG_TRANS  = 5,
	INT_CFG_ASLP   = 7
};

enum MMA8452Q_PL_STATUS {
	BAFRO = 0,
	LAPO0 = 1,
	LAPO1 = 2,
	LO    = 6,
	NEWLP = 7
};

enum MMA8452Q_PL_CFG {
	PL_EN  = 6,
	DBCNTM = 7
};

enum MMA8452Q_PULSE_SRC {
	PULSE_SRC_EA = 0x80,
	PULSE_SRC_AxZ = 0x40,
	PULSE_SRC_AxY = 0x20,
	PULSE_SRC_AxX = 0x10,
	PULSE_SRC_DPE = 0x08,
	PULSE_SRC_PolZ = 0x04,
	PULSE_SRC_PolY = 0x02,
	PULSE_SRC_PolX = 0x01
};

static uint8_t int_status;
static uint16_t xyz_thold[3] = {1024, 1024, 1024};

static inline void set_bit(uint8_t *data, uint8_t bit, bool value)
{
	if (value)
		*data |= (1 << bit);
	else
		*data &= ~(1 << bit);
}

static inline void set_helth_bit(bool value)
{
	set_bit(&int_status, HELTH_BIT, value);
}

static inline uint8_t acc_read(uint8_t addr)
{
	uint8_t data;

	i2c_bb_start();
	if (i2c_bb_write((MMA8452Q_ADDR << 1) | ACC_WRITE))
		set_helth_bit(ACC_DEAD);
	if (i2c_bb_write(addr))
		set_helth_bit(ACC_DEAD);
	i2c_bb_start();
	if (i2c_bb_write((MMA8452Q_ADDR << 1) | ACC_READ))
		set_helth_bit(ACC_DEAD);
	data = i2c_bb_read(1);
	i2c_bb_stop();

	return data;
}

static inline void acc_rx(uint8_t addr, uint8_t data[], uint8_t count)
{
	uint8_t i;

	i2c_bb_start();
	if (i2c_bb_write((MMA8452Q_ADDR << 1) | ACC_WRITE))
		set_helth_bit(ACC_DEAD);
	if (i2c_bb_write(addr))
		set_helth_bit(ACC_DEAD);
	i2c_bb_start();
	if (i2c_bb_write((MMA8452Q_ADDR << 1) | ACC_READ))
		set_helth_bit(ACC_DEAD);
	for (i = 0; i < (count - 1); i++)
		data[i] = i2c_bb_read(0);
	data[i] = i2c_bb_read(1);
	i2c_bb_stop();
}

static inline void register_set_bit(uint8_t addr, uint8_t bit, bool value)
{
	uint8_t buf[2];

	buf[0] = addr;
	buf[1] = acc_read(addr);
	set_bit(&buf[1], bit, value);
	i2c_bb_tx(MMA8452Q_ADDR, buf, 2);
}

static inline void set_active(bool enable)
{
	register_set_bit(CTRL_REG1, ACTIVE, enable);
}

static inline void set_interrupt(enum MMA8452Q_CTRL_REG4 intt,
				 bool enable)
{
	register_set_bit(CTRL_REG4, intt, enable);
}

static inline bool is_xyz_ready(void)
{
	return (acc_read(STATUS) & ZYX_DR);
}

/* @axes len always should be 3 bytes */
static inline void read_xyz(void)
{
	uint16_t xyz_acc[3];
	uint8_t data[6];
	uint8_t i;

	acc_rx(OUT_X_MSB, data, 6);
	xyz_acc[0] = data[0] << 4 | data[1] >> 4;
	xyz_acc[1] = data[2] << 4 | data[3] >> 4;
	xyz_acc[2] = data[4] << 4 | data[5] >> 4;

	for (i = 0; i < 3; i++) {
		if (!(xyz_acc[i] & 0x800)) {
			if (xyz_acc[i] > xyz_thold[i])
				set_bit(&int_status, i, 1);
		}
	}
}

static inline bool who_am_i(void)
{
	uint8_t data;

	data = acc_read(WHO_AM_I);
	if (data != 0xC7)
		return 0;
	else
		return 1;
}

uint8_t acc_get_status(void)
{
	uint8_t tmp;

	tmp = int_status;
	int_status &= ~0x07;

	return tmp;
}

void acc_set_threshold(uint16_t x_thold, uint16_t y_thold, uint16_t z_thold)
{
	xyz_thold[0] = x_thold;
	xyz_thold[1] = y_thold;
	xyz_thold[2] = z_thold;
}

void acc_init(void)
{
	static event_poll_t cb1;

	event_init();
	i2c_bb_init(100, GPIO1_11, GPIO1_10);
	if (!who_am_i())
		return;
	set_active(1);
	set_helth_bit(ACC_ACTIVE);
	event_poll_add(&cb1, is_xyz_ready, read_xyz);
}

int main(void)
{

    if (who_am_i())
        serial.printf("Acc test passed.\r\n");
    else
        serial.printf("Acc test failed.\r\n");
    return 0;
}
