#include "Source/Drivers/i2c_rw.h"

static int resp[MAX_COMMAND_LEN];

uint8_t read_req(uint8_t regi)
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
    return resp[0];
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
    uint8_t val = read_req(regi);

    // mask off the data before writing
    uint8_t mask = (1 << (bits)) - 1;
    data &= mask;

    mask <<= shift;
    val &= ~mask;          // remove the current data at that spot
    val |= data << shift; // and add in the new data

    write_req(regi,val);
}

//I2C callback function
void I2C_Callback2(mxc_i2c_req_t* req, int error)
{
    // I2C_FLAG = error;
    return;
}