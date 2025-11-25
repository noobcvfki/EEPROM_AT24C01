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
    IIC_TIMEOUT,
    IIC_NOTACK
}iic_ll_status_t;

typedef struct
{
    I2C_TypeDef* I2Cx;
    LL_I2C_InitTypeDef* I2CInit_struct;
    uint32_t timeout_ms;
}iic_ll_bus_t;

iic_ll_status_t ll_iic_start(iic_ll_bus_t* iic_bus);
iic_ll_status_t ll_iic_send_byte(iic_ll_bus_t* iic_bus, uint8_t data);
iic_ll_status_t ll_iic_init(iic_ll_bus_t* iic_bus);
iic_ll_status_t ll_iic_stop(iic_ll_bus_t* iic_bus);
iic_ll_status_t ll_iic_wait_addr_ack(iic_ll_bus_t* iic_bus);
iic_ll_status_t ll_iic_wait_data_ack(iic_ll_bus_t* iic_bus);
iic_ll_status_t ll_iic_send_ack(iic_ll_bus_t* iic_bus);
iic_ll_status_t ll_iic_send_nack(iic_ll_bus_t* iic_bus);
iic_ll_status_t ll_iic_read_byte(iic_ll_bus_t* iic_bus, uint8_t* p_data);
iic_ll_status_t ll_iic_clear_error_flags(iic_ll_bus_t* iic_bus);
__WEAK iic_ll_status_t ll_iic_write_multi_bytes(iic_ll_bus_t* iic_bus, uint8_t slave_addr, const uint8_t* p_data, uint16_t len);
__WEAK iic_ll_status_t ll_iic_read_multi_bytes(iic_ll_bus_t* iic_bus, uint8_t slave_addr, uint8_t* p_data, uint16_t len);


#endif //USER_MPUXXX_IIC_LL_H