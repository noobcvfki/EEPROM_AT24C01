//
// Created by capting on 2025/11/23.
//

#ifndef USER_MPUXXX_IIC_LL_H
#define USER_MPUXXX_IIC_LL_H

#include "stm32f4xx_ll_i2c.h"

typedef enum
{
    IIC_OK,
    IIC_IS_INITED,
    IIC_ERROR,
    IIC_NULL,
    IIC_TIMEOUT
}iic_ll_status_t;

typedef struct
{
    I2C_TypeDef* I2Cx;
    LL_I2C_InitTypeDef* I2CInit_struct;
    uint32_t timeout_ms;
}iic_ll_bus_t;

#endif //USER_MPUXXX_IIC_LL_H