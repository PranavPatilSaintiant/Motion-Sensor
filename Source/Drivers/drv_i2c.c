#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*	Add user define includes	*/
#include "mxc_device.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "i2c_regs.h"
#include "i2c.h"
#include "drv_i2c.h"
// #include "drv_uart.h"



/***** Globals *****/
uint8_t counter = 0;
#if 0


void I2C1_IRQHandler(void)
{
    MXC_I2C_AsyncHandler(I2C_MASTER_MPU6050);
    return;
}
#endif

// I2C callback function
void I2C_Callback(mxc_i2c_req_t *req, int error)
{
	return;
}

/// @brief initialize the i2c bus
/// @return E_NO_ERROR if successfull
int init_drv_i2c()
{
#if DEBUG
	// uart_print("Initializing I2C\n");
#endif
	int error;

	error = MXC_I2C_Init(I2C_MASTER_MPU6050, 1, 0);
	if (error != E_NO_ERROR)
	{
		// uart_print("FAILED to initialize=> I2C_MASTER_MPU6050.\n");
		return FAILED;
	}
	MXC_I2C_SetFrequency(I2C_MASTER_MPU6050, I2C_FREQ);

	error = MXC_I2C_Init(I2C_MASTER_EEPROM, 1, 0);
	if (error != E_NO_ERROR)
	{
		// uart_print("FAILED to initialize=> I2C_MASTER_EEPROM.\n");
		return FAILED;
	}
	MXC_I2C_SetFrequency(I2C_MASTER_EEPROM, I2C_FREQ);
//	MXC_I2C_SetFrequency(I2C_MASTER_EEPROM, 3400000);
#if 0
	NVIC_SetVector(I2C1_IRQn, I2C1_IRQHandler);
	NVIC_EnableIRQ(I2C1_IRQn);
	__enable_irq();
#endif
	return error;
}

/// @brief to read data from i2c channel
/// @param Interface i2s bus to use
/// @param SlaveID id of slave to read from
/// @param TxBuff array to transmit
/// @param TxLength length of array to transmit
/// @param RxBuff array to receive data
/// @param RxLength length of data to receive
/// @return None
int i2c_read(uint8_t Interface, unsigned int SlaveID, unsigned char *TxBuff, unsigned int TxLength, unsigned char *RxBuff, unsigned int RxLength)
{
	int error;
	mxc_i2c_req_t reqMaster;

	(void)memset(&reqMaster, 0x00, sizeof(mxc_i2c_req_t));

	if (Interface == I2C_2_CODEC)
	{
		reqMaster.i2c = I2C_MASTER_MPU6050;
	}
	else
	{
		/*	Trying to access wrong i2c instance	*/
		return -1;
	}

	SlaveID = ((SlaveID >> 1) | 0x01);

	reqMaster.addr = SlaveID;
	reqMaster.tx_buf = TxBuff;
	reqMaster.tx_len = TxLength;
	reqMaster.rx_buf = RxBuff;
	reqMaster.rx_len = RxLength;
	reqMaster.restart = 0;
	reqMaster.callback = I2C_Callback;

	if ((error = MXC_I2C_MasterTransaction(&reqMaster)) != 0)
	{
		// uart_print("Error reading: %d\n", error);
		return FAILED;
	}

	return error;
}

/// @brief to write data to i2c channel
/// @param Interface i2c bus to use
/// @param SlaveID id of slave to write to
/// @param TxBuff array to transmit
/// @param TxLength length of array to transmit
/// @return None
int i2c_write(uint8_t Interface, unsigned int SlaveID, uint8_t *TxBuff, unsigned int TxLength)
{
	int error;
	mxc_i2c_req_t reqMaster;

	// if (Interface == I2C_2_CODEC)
	// {
	// 	reqMaster.i2c = I2C_MASTER_MPU6050;
	// }
	// else
	// {
	// 	/*	Trying to access wrong i2c instance	*/
	// 	return -1;
	// }

	SlaveID = (SlaveID >> 1);
	reqMaster.i2c = Interface;
	reqMaster.addr = SlaveID;
	reqMaster.tx_buf = TxBuff;
	reqMaster.tx_len = TxLength;
	reqMaster.rx_buf = NULL;
	reqMaster.rx_len = 0;
	reqMaster.restart = 0;
	reqMaster.callback = I2C_Callback;

	if ((error = MXC_I2C_MasterTransaction(&reqMaster)) != 0)
	{
		// uart_print("Error writing: %d\n", error);
		return FAILED;
	}

	return error;
}

/*//////////////////////////////////////////////////////////////////////////////////////////*/
/***************************************** EEPROM *******************************************/
int i2c_read_eeprom(uint8_t Interface, uint8_t SlaveID, unsigned char *RxBuff, uint8_t RxLength)
{
	int error;
	mxc_i2c_req_t reqMaster;

	(void)memset(&reqMaster, 0x00, sizeof(mxc_i2c_req_t));

	reqMaster.i2c = I2C_MASTER_EEPROM;

	SlaveID = ((SlaveID >> 1) | 0x01);

	reqMaster.addr = SlaveID;
	reqMaster.tx_buf = NULL;
	reqMaster.tx_len = 0;
	reqMaster.rx_buf = RxBuff;
	reqMaster.rx_len = RxLength;
	reqMaster.restart = 0;
	reqMaster.callback = I2C_Callback;

	if ((MXC_I2C_MasterTransaction(&reqMaster)) == 0)
	{
		// uart_print("\nslave responded on read to ID %X\n", SlaveID);
	}
	else
	{
		// uart_print("\nslave not responded on read to ID %X\n", SlaveID);
	}

	return error;
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/
/// @brief to write data to i2c channel
/// @param Interface i2c bus to use
/// @param SlaveID id of slave to write to
/// @param TxBuff array to transmit
/// @param TxLength length of array to transmit
/// @return None
int i2c_write_eeprom(uint8_t Interface, uint8_t SlaveID, unsigned char *TxBuff, uint8_t TxLength)
{
	int error;
	mxc_i2c_req_t reqMaster;

	reqMaster.i2c = I2C_MASTER_EEPROM;
	SlaveID = (SlaveID >> 1);

	reqMaster.addr = SlaveID;
	reqMaster.tx_buf = TxBuff;
	reqMaster.tx_len = TxLength;
	reqMaster.rx_buf = NULL;
	reqMaster.rx_len = 0;
	reqMaster.restart = 0;
	reqMaster.callback = NULL;

	if ((MXC_I2C_MasterTransaction(&reqMaster)) == 0)
	{
		// uart_print("\nslave responded on write to ID %X\n", SlaveID);
	}
	else
	{
		// uart_print("\nslave not responded on write to ID %X\n", SlaveID);
	}

	return error;
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/
/// @brief to scan the connected slave devices of a channel
/// @param Interface i2c bus to use
/// @return None
int i2c_scanner(uint8_t Interface)
{
	uint8_t counter = 0;
	// uart_print("Entered in I2C Scanner\n");
	mxc_i2c_req_t reqMaster;

	(void)memset(&reqMaster, 0x00, sizeof(mxc_i2c_req_t));

	if (Interface == I2C_2_CODEC)
	{
		reqMaster.i2c = I2C_MASTER_MPU6050;
	}
	else if (Interface == I2C_0_EEPROM)
	{
		reqMaster.i2c = I2C_MASTER_EEPROM;
	}
	else
	{
		/*	Trying to access wrong i2c instance	*/
		return -1;
	}

	reqMaster.addr = 0;
	reqMaster.tx_buf = NULL;
	reqMaster.tx_len = 0;
	reqMaster.rx_buf = NULL;
	reqMaster.rx_len = 0;
	reqMaster.restart = 0;
	reqMaster.callback = NULL;

	for (uint8_t address = 8; address < 120; address++)
	{
		reqMaster.addr = address;
		// uart_print(".");

		if ((MXC_I2C_MasterTransaction(&reqMaster)) == 0)
		{
			// uart_print("\nFound slave ID %03d; 0x%02X\n", address, address);
			counter++;
		}
		MXC_Delay(MXC_DELAY_MSEC(200));
	}

	// uart_print("\n-->Scan finished. %d devices found\n", counter);
	return 0;
}
/*////////////////////////////////////////////////////////////////////////////////////////////////*/

/*============================= 24LC01_EEPROM ==============================*/

int write_i2c_byte_24LC01_EEPROM(uint8_t Interface, uint8_t SlaveID, uint8_t WordAddress, uint8_t *Write_Byte, uint8_t Write_Byte_length)
{
	uint8_t Sending_data[1 + Write_Byte_length];
	// init_drv_i2c();
	int error;
	mxc_i2c_req_t reqMaster;
	memset(Sending_data, 0x00, (1 + Write_Byte_length));
	Sending_data[0] = WordAddress;

	for (uint8_t i = 0; i < Write_Byte_length; i++)
	{
		Sending_data[i + 1] = Write_Byte[i];
	}

	i2c_write_eeprom(I2C_0_EEPROM, i2c_write_address_24LC01_EEPROM, Sending_data, sizeof(Sending_data));
}

int read_i2c_byte_24LC01_EEPROM(uint8_t Interface, uint8_t SlaveID, uint8_t WordAddress, uint8_t *Read_Byte, uint8_t Read_Byte_length)
{
	// init_drv_i2c();
	int error;
	mxc_i2c_req_t reqMaster;
	(void)memset(&reqMaster, 0x00, sizeof(mxc_i2c_req_t));
	// memset(Read_Byte, 0x00, Read_Byte_length);

	uint8_t Sending_data[1]; // 1st is the address and
	memset(Sending_data, 0x00, 1);
	Sending_data[0] = WordAddress;

	i2c_write_eeprom(I2C_0_EEPROM, i2c_write_address_24LC01_EEPROM, Sending_data, sizeof(Sending_data));
	// MXC_Delay(10);
	i2c_read_eeprom(I2C_0_EEPROM, i2c_read_address_24LC01_EEPROM, Read_Byte, Read_Byte_length);
}

int string_write_i2c_24LC01_EEPROM(uint8_t Interface, uint8_t SlaveID, uint8_t WordAddress, uint8_t *Write_Byte, uint8_t Write_Byte_length, uint8_t max_string_length)
{
	if (Write_Byte_length > max_string_length)
	{
		return -1;
	}
	for (uint8_t i = 0; i < Write_Byte_length; i++)
	{
		write_i2c_byte_24LC01_EEPROM(Interface, SlaveID, WordAddress + i, &Write_Byte[i], 1);
		MXC_Delay(MXC_DELAY_MSEC(4));
	}

	// uart_print("page_number %d first char %c", page_number, Write_Byte[page_number * 8]);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
