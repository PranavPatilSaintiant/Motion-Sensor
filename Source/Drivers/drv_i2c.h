#ifndef DRV_I2C_H_
#define DRV_I2C_H_
#include <stdint.h>
#include <string.h>

typedef enum
{
	I2C_2_CODEC,
	I2C_0_EEPROM,
	I2C_MAX
} I2C_;

#define i2c_write_address_24LC01_EEPROM 0xA0
#define i2c_read_address_24LC01_EEPROM 0xA1

#define I2C_MASTER_MPU6050 MXC_I2C2
#define I2C_MASTER_EEPROM MXC_I2C0
#define I2C_FREQ 100000
#define I2C_SLAVE_ADDR 0x68
#define I2C_BYTES 1

#define FAILED 0x01
#define PASSED 0x00

/********************** Audio Codec ***********************/
int init_drv_i2c(void);
int i2c_read(uint8_t Interface, unsigned int SlaveID, unsigned char *TxBuff, unsigned int TxLength, unsigned char *RxBuff, unsigned int RxLength);
int i2c_write(uint8_t Interface, unsigned int SlaveID, uint8_t *TxBuff, unsigned int TxLength);
int i2c_scanner(uint8_t Interface);

/********************** EEPROM *************************/
int write_i2c_byte_24LC01_EEPROM(uint8_t Interface, uint8_t SlaveID, uint8_t WordAddress, uint8_t *Write_Byte, uint8_t Write_Byte_length);
int read_i2c_byte_24LC01_EEPROM(uint8_t Interface, uint8_t SlaveID, uint8_t WordAddress, uint8_t *Read_Byte, uint8_t Read_Byte_length);
int string_write_i2c_24LC01_EEPROM(uint8_t Interface, uint8_t SlaveID, uint8_t WordAddress, uint8_t *Write_Byte, uint8_t Write_Byte_length, uint8_t max_string_length);

#endif /* DRV_I2C_H_ */