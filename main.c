/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "Source/Drivers/i2c_rw.h"
#include "Source/Drivers/MPU6050/MPU6050.h"
#include "Source/Drivers/MPU6050/MPU6050_reg.h"

#include "nvic_table.h"
#include "dma.h"
#include "gpio.h"

/***** Definitions *****/
#define I2C_MASTER MXC_I2C2
#define I2C_FREQ 100000

#define MXC_GPIO_PORT_OUT MXC_GPIO0
#define RSTN  MXC_GPIO_PIN_24

#define MXC_GPIO_PORT_OUT MXC_GPIO0
#define MFIO MXC_GPIO_PIN_25

// typedef enum { FAILED, PASSED } test_t;

/***** Globals *****/
volatile uint8_t DMA_FLAG = 0;
// volatile int I2C_FLAG;
/***** Functions *****/

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
    if(read_req(MPU6050_WHO_AM_I)!=I2C_SLAVE_ADDR){
        printf("MPU6050 Not Connected\n");
    }
    else{
        printf("MPU6050 Connected\n");
    }

    MXC_DELAY_MSEC(100);
    mpu6050_rst();
    mpu6050_init();
    print_reg(read_req(MPU6050_WHO_AM_I));
    write_req(0x23,0x08);
    print_reg(read_req(0x23));

    while(1){
        if(checkAngularVibration()==VIBRATING || checkLinearVibration()==VIBRATING){
            printf("Vibration Detected\n");
        }
        else{
            printf("Stable\n");
        }
        // MXC_DELAY_MSEC(10);
        // printf("\nX: %d Y: %d Z: %d ",i8to16(a_x_h(),a_x_l()),i8to16(a_y_h(),a_y_l()),i8to16(a_z_h(),a_z_l()));
        // printf("\nX: %f Y: %f Z: %f ",i8to16(g_x_h(),g_x_l()),i8to16(g_y_h(),g_y_l()),i8to16(g_z_h(),g_z_l()));
        // printf("\nTemperaure: %f",convertToTemperature(temperature_h(),temperature_l()));
    }
}

void print_reg(uint8_t data){
    printf("\n%d\n",data);
}