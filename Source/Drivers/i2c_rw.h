#ifndef _I2C_RW_H_
#define _I2C_RW_H_

#define MAX_COMMAND_LEN 2
#define CMD_DELAY 10

#define FAILED 0x01
#define PASSED 0x00

#define I2C_MASTER MXC_I2C2
#define I2C_FREQ 100000
#define I2C_SLAVE_ADDR 0x68

#include <stdint.h>
#include "drv_i2c.h"

#include "mxc_device.h"
#include "mxc_delay.h"
#include "i2c_regs.h"
#include "i2c.h"

uint8_t read_req(uint8_t regi);
uint8_t read_bitslice(uint8_t regi,uint8_t bits,uint8_t shift);
void write_req(uint8_t regi, uint8_t data);
void write_bitslice(uint8_t regi, uint8_t data, uint8_t bits, uint8_t shift);
void I2C_Callback2(mxc_i2c_req_t* req, int error);

#endif