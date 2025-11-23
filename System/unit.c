//
// Created by capting on 2025/11/22.
//

#include "FreeRTOS.h"
#include "unit.h"
#include "iic_hal.h"
#include "at24c02_driver.h"
#include "elog.h"
#include "stm32f4xx_hal.h"
#include "task.h"
//-----------------------------iic_AT24C02------------------------------------//
__STATIC_INLINE void enable_iic_clock(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

iic_bus_t eeprom_iic = {
    .IIC_SDA_PORT = GPIOB,
    .IIC_SCL_PORT = GPIOB,
    .IIC_SDA_PIN = GPIO_PIN_7,
    .IIC_SCL_PIN = GPIO_PIN_6,
    .CLK_ENABLE = enable_iic_clock
};

static eeprom_status_t iic_init_myown(void* p_iic)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    IICInit(iic_instance);
    return EEPROM_OK;
}

static eeprom_status_t iic_deinit_myown(void* p_iic)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    HAL_GPIO_DeInit(iic_instance->IIC_SCL_PORT,iic_instance->IIC_SCL_PIN);
    HAL_GPIO_DeInit(iic_instance->IIC_SDA_PORT,iic_instance->IIC_SDA_PIN);
    return EEPROM_OK;
}

static eeprom_status_t iic_start_myown(void* p_iic)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    IICStart(iic_instance);
    return EEPROM_OK;
}

static eeprom_status_t iic_send_byte_myown(void *p_iic,uint8_t data)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    IICSendByte(iic_instance ,data);
    return EEPROM_OK;
}

static eeprom_status_t iic_wait_ack_myown(void *p_iic)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    if (0 != IICWaitAck(iic_instance))
    {
        return EEPROM_ERRORTIMEOUT;
    }
    return EEPROM_OK;
}

static eeprom_status_t iic_read_byte_myown(void *p_iic,uint8_t* pData)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    *pData = IICReceiveByte(iic_instance);
    return EEPROM_OK;
}

static eeprom_status_t iic_send_ack_myown(void *p_iic)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    IICSendAck(iic_instance);
    return EEPROM_OK;
}

static eeprom_status_t iic_no_ack_myown(void *p_iic)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    IICSendNotAck(iic_instance);
    return EEPROM_OK;
}

static eeprom_status_t iic_stop_myown(void *p_iic)
{
    iic_bus_t* iic_instance = (iic_bus_t*)(p_iic);
    IICStop(iic_instance);
    return EEPROM_OK;
}

static void critical_enable(void)
{
    taskENTER_CRITICAL();
}

static void critical_disable(void)
{
    taskEXIT_CRITICAL();
}

eeprom_software_iic_driver_t bsp_eeprom_iic = {
    .iic_handle = &eeprom_iic,
    .pf_iic_init = iic_init_myown,
    .pf_iic_deinit = iic_deinit_myown,
    .pf_iic_start = iic_start_myown,
    .pf_iic_send_byte = iic_send_byte_myown,
    .pf_iic_wait_ack = iic_wait_ack_myown,
    .pf_iic_read_byte = iic_read_byte_myown,
    .pf_iic_send_ack = iic_send_ack_myown,
    .pf_iic_no_ack = iic_no_ack_myown,
    .pf_iic_stop = iic_stop_myown,
    .critical_enable = critical_enable,
    .critical_disable = critical_disable
};
