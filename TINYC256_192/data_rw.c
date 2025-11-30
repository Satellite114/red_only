#include "data_rw.h"
#include "i2c.h"

ir_error_t i2c_data_read(uint16_t byI2CSlaveID, uint16_t wI2CRegAddr, uint16_t wLen, uint8_t *pbyData)
{
    HAL_StatusTypeDef rst;
    rst = HAL_I2C_Mem_Read(&hi2c3, byI2CSlaveID, wI2CRegAddr, I2C_MEMADD_SIZE_16BIT, pbyData, wLen, I2C_TRANSFER_WAIT_TIME_MS);
    if (rst != HAL_OK)
    {
        // printf("I2C read command filed (error_code:%d)\n", rst);
        return IR_I2C_GET_REGISTER_FAIL;
    }
    else
    {
        return IR_SUCCESS;
    }
}

static ir_error_t i2c_check_access_done(void)
{
    uint8_t status = 0xFF;
    uint8_t error_type = 0;
    uint16_t wWaitTime = I2C_TRANSFER_WAIT_TIME_MS;
    do
    {
        i2c_data_read(I2C_SLAVE_ID, I2C_VD_BUFFER_STATUS, 1, &status);
        error_type = (status & VCMD_ERR_STS_BIT);
        switch (error_type)
        {
        case VCMD_ERR_STS_LEN_ERR:
            // printf("LEN_ERR\n");
            return IR_CHECK_DONE_FAIL;
        case VCMD_ERR_STS_UNKNOWN_CMD_ERR:
            // printf("UNKNOWN_CMD_ERR\n");
            return IR_CHECK_DONE_FAIL;
        case VCMD_ERR_STS_HW_ERR:
            // printf("HW_ERR\n");
            return IR_CHECK_DONE_FAIL;
        case VCMD_ERR_STS_UNKNOWN_SUBCMD_ERR:
            // printf("UNKNOWN_SUBCMD_ERR\n");
            return IR_CHECK_DONE_FAIL;
        case VCMD_ERR_STS_PARAM_ERR:
            // printf("PARAM_ERR\n");
            return IR_CHECK_DONE_FAIL;
        default:
            break;
        }
        if ((status & (VCMD_RST_STS_BIT | VCMD_BUSY_STS_BIT)) ==
            (VCMD_BUSY_STS_IDLE | VCMD_RST_STS_PASS))
        {
            return IR_SUCCESS;
        }
        HAL_Delay(100);
        wWaitTime -= 100;
    } while (wWaitTime);

    // printf("check done fail!\n");
    return IR_CHECK_DONE_FAIL;
}

ir_error_t i2c_data_write(uint16_t byI2CSlaveID, uint16_t wI2CRegAddr, uint16_t wLen, uint8_t *pbyData)
{
    HAL_StatusTypeDef rst;
    rst = HAL_I2C_Mem_Write(&hi2c3, byI2CSlaveID, wI2CRegAddr, I2C_MEMADD_SIZE_16BIT, pbyData, wLen, I2C_TRANSFER_WAIT_TIME_MS);
    
   // rst = HAL_I2C_IsDeviceReady(&hi2c3, 0x3C<<1, 2, 10);
    volatile int temp_err =     HAL_I2C_GetError(&hi2c3);
    if (rst != HAL_OK)
    {
        // printf("I2C write command filed (error_code:%d)\n", rst);
        return IR_I2C_SET_REGISTER_FAIL;
    }
    // when wI2CRegAddr bit[15] = 1,only transfer data to vdcmd bufff,no need to call check_access_done
    if (wI2CRegAddr & I2C_VD_CHECK_ACCESS)
    {
        return IR_SUCCESS;
    }
    else
    {
        return i2c_check_access_done();
    }
}
