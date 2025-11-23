//
// Created by capting on 2025/11/23.
//

#include "iic_ll.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_i2c.h"

iic_ll_status_t ll_iic_start(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus->I2Cx)return IIC_NULL;
    uint32_t tickstart = HAL_GetTick();

    while (LL_I2C_IsActiveFlag_BUSY(iic_bus->I2Cx))
    {
        if (HAL_GetTick() - tickstart > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }

    LL_I2C_GenerateStartCondition(iic_bus->I2Cx);

    while (!LL_I2C_IsActiveFlag_SB(iic_bus->I2Cx))
    {
        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms) {
            return IIC_TIMEOUT;
        }
    }

    return IIC_OK;
}

iic_ll_status_t ll_iic_send_byte(iic_ll_bus_t* iic_bus,uint8_t data)
{
    if (NULL == iic_bus)return IIC_NULL;
    uint32_t tickstart = HAL_GetTick();
    while (!LL_I2C_IsActiveFlag_TXE(iic_bus->I2Cx)) {
        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms) {
            return IIC_TIMEOUT;
        }
        // 检查是否收到NACK（从机无响应）
        if (LL_I2C_IsActiveFlag_TXE(iic_bus->I2Cx)) {
            LL_I2C_ClearFlag_AF(iic_bus->I2Cx);
            return IIC_NOTACK;
        }
    }
    LL_I2C_TransmitData8(iic_bus->I2Cx, data);

    while (!LL_I2C_IsActiveFlag_BTF(iic_bus->I2Cx)) {
        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms) {
            return IIC_TIMEOUT;
        }
    }
    return IIC_OK;
}

iic_ll_status_t iic_init(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus||NULL == iic_bus->I2CInit_struct)
    {
        return IIC_NULL;
    }
    if (1 == LL_I2C_IsEnabled(iic_bus->I2Cx))
    {
        return IIC_IS_INITED;
    }
    if (SUCCESS != LL_I2C_Init(iic_bus->I2Cx,iic_bus->I2CInit_struct))
    {
        return IIC_ERROR;
    }
    return IIC_OK;
}

