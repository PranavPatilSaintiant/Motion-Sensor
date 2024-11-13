/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "drv_i2c.h"
#include "Source/Drivers/MPU6050/MPU6050_reg.h"

#include "mxc_device.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "i2c_regs.h"
#include "i2c.h"
#include "dma.h"
#include "gpio.h"

/***** Definitions *****/
#define I2C_MASTER MXC_I2C2
#define I2C_FREQ 100000

#define MXC_GPIO_PORT_OUT MXC_GPIO0
#define RSTN  MXC_GPIO_PIN_24

#define MXC_GPIO_PORT_OUT MXC_GPIO0
#define MFIO MXC_GPIO_PIN_25

#define MAX_COMMAND_LEN 2
#define CMD_DELAY 10

// typedef enum { FAILED, PASSED } test_t;

/***** Globals *****/
static int cmd[MAX_COMMAND_LEN] = {0x75};
static int resp[MAX_COMMAND_LEN] = {0x00};
volatile uint8_t DMA_FLAG = 0;
volatile int I2C_FLAG;
/***** Functions *****/

//I2C callback function
void I2C_Callback2(mxc_i2c_req_t* req, int error)
{
    I2C_FLAG = error;
    return;
}

void ApplicationMode(void){
    MXC_GPIO_OutClr(MXC_GPIO_PORT_OUT,RSTN);
    MXC_Delay(MXC_DELAY_MSEC(1));
    MXC_GPIO_OutSet(MXC_GPIO_PORT_OUT,MFIO);
    MXC_Delay(MXC_DELAY_MSEC(9));
    MXC_GPIO_OutSet(MXC_GPIO_PORT_OUT,RSTN);
    MXC_Delay(MXC_DELAY_MSEC(2));
    MXC_Delay(MXC_DELAY_MSEC(5));
    
    MXC_Delay(MXC_DELAY_SEC(1));
}

void printData(void){
    int i;
    printf("\n-->TxData: ");
    for (i = 0; i < sizeof(cmd); ++i) {
        printf("%d ", cmd[i]);
    }
    printf("\n\n-->RxData: ");
    for (i = 0; i < sizeof(resp); ++i) {
        printf("%d ", resp[i]);
    }
    printf("\n");
    return;
}

void I2CScan(void){
uint8_t counter = 0;

    printf("\n******** I2C SLAVE ADDRESS SCANNER *********\n");
    printf("\nThis example finds the addresses of any I2C Slave devices connected to the");
#if defined(BOARD_FTHR_APPS_P1)
    printf("\nsame bus as I2C1 (SCL - P0.16, SDA - P0.17).\n");
#else
    printf("\nsame bus as I2C2 (SCL - P0.30, SDA - P0.31)");
    printf("\nYou must connect the pull-up jumpers (JP21 and JP22) to the proper I/O voltage.");
#endif

    //Setup the I2CM
    if (E_NO_ERROR != MXC_I2C_Init(I2C_MASTER, 1, 0)) {
        printf("-->FAILED master\n");
        return -1;
    } else {
        printf("\n-->I2C Master Initialization Complete\n");
    }

    printf("-->Scanning started\n");
    MXC_I2C_SetFrequency(I2C_MASTER, I2C_FREQ);
    mxc_i2c_req_t reqMaster;
    reqMaster.i2c      = I2C_MASTER;
    reqMaster.addr     = 0;
    reqMaster.tx_buf   = NULL;
    reqMaster.tx_len   = 0;
    reqMaster.rx_buf   = NULL;
    reqMaster.rx_len   = 0;
    reqMaster.restart  = 0;
    reqMaster.callback = NULL;

    printf(".");
    fflush(0);
    uint8_t address = 0x68;
    reqMaster.addr = address;
    if (E_NO_ERROR == MXC_I2C_MasterTransaction(&reqMaster)) {
        printf("\nFound slave ID %03d; 0x%02X\n", address, address);
        counter++;
    }
    else{
        printf("I2C Communication Error!");
    }
    MXC_Delay(MXC_DELAY_MSEC(200));
    printf("\n-->Scan finished. %d devices found\n", counter);
}

int main()
{   
    //Enter Application Mode
    ApplicationMode();
    printf("Application Mode Started\n");
    // I2CScan();
    // Setup the I2CM
    int error;
    error = MXC_I2C_Init(I2C_MASTER, 1, 0);
    if (error != E_NO_ERROR) {
        printf("-->FAILED master\n");
        return FAILED;
    } else {
        printf("\n-->I2C Master Initialization Complete\n");
    }

    MXC_I2C_SetFrequency(I2C_MASTER, I2C_FREQ);
    
    //Recieve Device Address from Module
    read_req(MPU6050_WHO_AM_I);
    if(resp[0]!=I2C_SLAVE_ADDR){
        printf("MPU6050 Not Connected\n");
    }
    else{
        printf("MPU6050 Connected\n");
    }
    //Reset Sequence
    MXC_DELAY_MSEC(10);
    write_bitslice(MPU6050_PWR_MGMT_1,0x01,1,7);
    

    // read_req(MPU6050_SIGNAL_PATH_RESET);
    // print_reg(resp[0]);
}

void read_req(uint8_t regi)
{
    uint8_t TxBuff[MAX_COMMAND_LEN] = {0};
    TxBuff[0] = regi;
    // uint8_t RxBuff[MAX_COMMAND_LEN] = {0};
    mxc_i2c_req_t SendCommand;
    SendCommand.i2c      = I2C_MASTER;
    SendCommand.addr     = I2C_SLAVE_ADDR;
    SendCommand.tx_buf   = TxBuff;
    SendCommand.tx_len   = 1;
    SendCommand.rx_buf   = resp;
    SendCommand.rx_len   = 0;
    SendCommand.restart  = 0;
    SendCommand.callback = I2C_Callback2;
    int error;
    if ((error = MXC_I2C_MasterTransaction(&SendCommand)) != 0) {
        printf("Error writing: %d\n", error);
        return FAILED;
    }
    MXC_DELAY_MSEC(CMD_DELAY);

    mxc_i2c_req_t ReadResponse;
    ReadResponse.i2c      = I2C_MASTER;
    ReadResponse.addr     = I2C_SLAVE_ADDR;
    ReadResponse.tx_buf   = NULL;
    ReadResponse.tx_len   = 0;
    ReadResponse.rx_buf   = resp;
    ReadResponse.rx_len   = 1;
    ReadResponse.restart  = 0;
    ReadResponse.callback = I2C_Callback2;

    if ((error = MXC_I2C_MasterTransaction(&ReadResponse)) != 0) {
    printf("Error writing: %d\n", error);
    return FAILED;
    }
}

void write_req(uint8_t regi, uint8_t data)
{
    uint8_t TxBuff[MAX_COMMAND_LEN];
    TxBuff[0] = regi;
    TxBuff[1] = data;
    mxc_i2c_req_t SendCommand;
    SendCommand.i2c      = I2C_MASTER;
    SendCommand.addr     = I2C_SLAVE_ADDR;
    SendCommand.tx_buf   = TxBuff;
    SendCommand.tx_len   = 2;
    SendCommand.rx_buf   = NULL;
    SendCommand.rx_len   = 0;
    SendCommand.restart  = 0;
    SendCommand.callback = I2C_Callback2;
    int error;
    if ((error = MXC_I2C_MasterTransaction(&SendCommand)) != 0) {
        printf("Error writing: %d\n", error);
        return FAILED;
    }
    MXC_DELAY_MSEC(CMD_DELAY);
}

uint8_t read_bitslice(uint8_t regi,uint8_t bits,uint8_t shift){
    read_req(regi);
    uint8_t val = resp[0];
    val >>= shift;
    return val & ((1 << (bits)) - 1);
}

void write_bitslice(uint8_t regi, uint8_t data, uint8_t bits, uint8_t shift)
{
    read_req(regi);
    uint8_t val = resp[0];

    // mask off the data before writing
    uint8_t mask = (1 << (bits)) - 1;
    data &= mask;

    mask <<= shift;
    val &= ~mask;          // remove the current data at that spot
    val |= data << shift; // and add in the new data

    write_req(regi,val);
}

void print_reg(uint8_t data){
    printf("\n%d\n",data);
}