//
// Created by capting on 2025/11/23.
//

#include "iic_ll.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_i2c.h"

/************************** 现有函数修正 **************************/
iic_ll_status_t ll_iic_start(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx) // 补充iic_bus空指针检查
        return IIC_NULL;

    uint32_t tickstart = HAL_GetTick();

    // 等待总线空闲
    while (LL_I2C_IsActiveFlag_BUSY(iic_bus->I2Cx))
    {
        if (HAL_GetTick() - tickstart > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }

    // 生成START条件
    LL_I2C_GenerateStartCondition(iic_bus->I2Cx);

    // 等待START发送成功（SB标志置1）
    while (!LL_I2C_IsActiveFlag_SB(iic_bus->I2Cx))
    {
        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }

    return IIC_OK;
}

iic_ll_status_t ll_iic_send_byte(iic_ll_bus_t* iic_bus, uint8_t data)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx)
        return IIC_NULL;

    uint32_t tickstart = HAL_GetTick();

    // 等待TXE标志（数据寄存器空）
    while (!LL_I2C_IsActiveFlag_TXE(iic_bus->I2Cx))
    {
        if (LL_I2C_IsActiveFlag_AF(iic_bus->I2Cx))
        {
            LL_I2C_ClearFlag_AF(iic_bus->I2Cx);
            return IIC_NOTACK;
        }
        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }

    // 发送数据
    LL_I2C_TransmitData8(iic_bus->I2Cx, data);

    // ✅ 移除BTF等待，由数据应答函数处理
    return IIC_OK;
}

iic_ll_status_t iic_init(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx || NULL == iic_bus->I2CInit_struct)
    {
        return IIC_NULL;
    }

    // 检查I2C是否已使能（已初始化）
    if (LL_I2C_IsEnabled(iic_bus->I2Cx))
    {
        return IIC_IS_INITED;
    }

    // 调用LL库初始化I2C
    if (SUCCESS != LL_I2C_Init(iic_bus->I2Cx, iic_bus->I2CInit_struct))
    {
        return IIC_ERROR;
    }

    // 初始化后使能I2C外设（CubeMX生成代码可能已包含，冗余保障）
    LL_I2C_Enable(iic_bus->I2Cx);

    return IIC_OK;
}

/************************** 补充核心函数 **************************/
/**
 * @brief  生成I2C STOP条件
 * @param  iic_bus: I2C总线句柄
 * @retval iic_ll_status_t: 操作状态
 */
iic_ll_status_t ll_iic_stop(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx)
        return IIC_NULL;

    uint32_t tickstart = HAL_GetTick();

    // 生成STOP条件
    LL_I2C_GenerateStopCondition(iic_bus->I2Cx);

    // 等待STOP信号发送完成（STOP标志清除）
    while (LL_I2C_IsActiveFlag_STOP(iic_bus->I2Cx))
    {
        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }

    return IIC_OK;
}

/**
 * @brief  等待地址应答（发送从机地址后调用）
 */
iic_ll_status_t ll_iic_wait_addr_ack(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx)
        return IIC_NULL;

    uint32_t tickstart = HAL_GetTick();

    while (1)
    {
        if (LL_I2C_IsActiveFlag_ADDR(iic_bus->I2Cx))
        {
            LL_I2C_ClearFlag_ADDR(iic_bus->I2Cx);
            return IIC_OK;
        }

        if (LL_I2C_IsActiveFlag_AF(iic_bus->I2Cx))
        {
            LL_I2C_ClearFlag_AF(iic_bus->I2Cx);
            return IIC_NOTACK;
        }

        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }
}

/**
 * @brief  等待数据应答（发送数据字节后调用）
 */
iic_ll_status_t ll_iic_wait_data_ack(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx)
        return IIC_NULL;

    uint32_t tickstart = HAL_GetTick();

    while (1)
    {
        // ✅ 关键修正：使用BTF而不是TXE
        if (LL_I2C_IsActiveFlag_BTF(iic_bus->I2Cx))
        {
            return IIC_OK;
        }

        if (LL_I2C_IsActiveFlag_AF(iic_bus->I2Cx))
        {
            LL_I2C_ClearFlag_AF(iic_bus->I2Cx);
            return IIC_NOTACK;
        }

        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }
}

/**
 * @brief  发送应答（ACK）
 * @param  iic_bus: I2C总线句柄
 * @retval iic_ll_status_t: 操作状态
 */
iic_ll_status_t ll_iic_send_ack(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx)
        return IIC_NULL;

    // 配置下一个字节接收后发送ACK
    LL_I2C_AcknowledgeNextData(iic_bus->I2Cx, LL_I2C_ACK);

    return IIC_OK;
}

/**
 * @brief  发送非应答（NACK）
 * @param  iic_bus: I2C总线句柄
 * @retval iic_ll_status_t: 操作状态
 */
iic_ll_status_t ll_iic_send_nack(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx)
        return IIC_NULL;

    // 配置下一个字节接收后发送NACK
    LL_I2C_AcknowledgeNextData(iic_bus->I2Cx, LL_I2C_NACK);

    return IIC_OK;
}

/**
 * @brief  读取1个字节数据
 * @param  iic_bus: I2C总线句柄
 * @param  p_data: 接收数据缓冲区（需非空）
 * @retval iic_ll_status_t: 操作状态
 */
iic_ll_status_t ll_iic_read_byte(iic_ll_bus_t* iic_bus, uint8_t* p_data)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx || NULL == p_data)
        return IIC_NULL;

    uint32_t tickstart = HAL_GetTick();

    // 等待RXNE标志（接收数据寄存器非空）
    while (!LL_I2C_IsActiveFlag_RXNE(iic_bus->I2Cx))
    {
        if ((HAL_GetTick() - tickstart) > iic_bus->timeout_ms)
        {
            return IIC_TIMEOUT;
        }
    }

    // 读取数据
    *p_data = LL_I2C_ReceiveData8(iic_bus->I2Cx);

    return IIC_OK;
}

/**
 * @brief  多字节写入（主机→从机）
 * @param  iic_bus: I2C总线句柄
 * @param  slave_addr: 从机7位地址（不含读写位）
 * @param  p_data: 发送数据缓冲区（需非空）
 * @param  len: 发送数据长度（>0）
 * @retval iic_ll_status_t: 操作状态
 */
iic_ll_status_t ll_iic_write_multi_bytes(iic_ll_bus_t* iic_bus, uint8_t slave_addr, const uint8_t* p_data, uint16_t len)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx || NULL == p_data || len == 0)
        return IIC_NULL;

    iic_ll_status_t status;
    uint16_t i;

    // 1. 生成START条件
    status = ll_iic_start(iic_bus);
    if (status != IIC_OK)
        return status;

    // 2. 发送从机地址+写位（7位地址左移1位 + 0）
    status = ll_iic_send_byte(iic_bus, (slave_addr << 1) | LL_I2C_DIRECTION_WRITE);
    if (status != IIC_OK)
    {
        ll_iic_stop(iic_bus); // 发送失败，生成STOP
        return status;
    }

    // 3. 等待从机ACK
    status = ll_iic_wait_ack(iic_bus);
    if (status != IIC_OK)
    {
        ll_iic_stop(iic_bus);
        return status;
    }

    // 4. 连续发送len字节数据
    for (i = 0; i < len; i++)
    {
        status = ll_iic_send_byte(iic_bus, p_data[i]);
        if (status != IIC_OK)
        {
            ll_iic_stop(iic_bus);
            return status;
        }
    }

    // 5. 生成STOP条件，结束传输
    status = ll_iic_stop(iic_bus);
    return status;
}

/**
 * @brief  多字节读取（从机→主机）
 * @param  iic_bus: I2C总线句柄
 * @param  slave_addr: 从机7位地址（不含读写位）
 * @param  p_data: 接收数据缓冲区（需非空）
 * @param  len: 接收数据长度（>0）
 * @retval iic_ll_status_t: 操作状态
 */
iic_ll_status_t ll_iic_read_multi_bytes(iic_ll_bus_t* iic_bus, uint8_t slave_addr, uint8_t* p_data, uint16_t len)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx || NULL == p_data || len == 0)
        return IIC_NULL;

    iic_ll_status_t status;
    uint16_t i;

    // 1. 生成START条件
    status = ll_iic_start(iic_bus);
    if (status != IIC_OK)
        return status;

    // 2. 发送从机地址+读位（7位地址左移1位 + 1）
    status = ll_iic_send_byte(iic_bus, (slave_addr << 1) | LL_I2C_DIRECTION_READ);
    if (status != IIC_OK)
    {
        ll_iic_stop(iic_bus);
        return status;
    }

    // 3. 等待从机ACK
    status = ll_iic_wait_ack(iic_bus);
    if (status != IIC_OK)
    {
        ll_iic_stop(iic_bus);
        return status;
    }

    // 4. 连续接收len字节数据
    for (i = 0; i < len; i++)
    {
        // 前len-1字节：接收后发送ACK；最后1字节：接收前发送NACK
        if (i == len - 1)
        {
            status = ll_iic_send_nack(iic_bus); // 最后1字节，发送NACK
            if (status != IIC_OK)
            {
                ll_iic_stop(iic_bus);
                return status;
            }
        }
        else
        {
            status = ll_iic_send_ack(iic_bus); // 中间字节，发送ACK
            if (status != IIC_OK)
            {
                ll_iic_stop(iic_bus);
                return status;
            }
        }

        // 读取当前字节
        status = ll_iic_read_byte(iic_bus, &p_data[i]);
        if (status != IIC_OK)
        {
            ll_iic_stop(iic_bus);
            return status;
        }
    }

    // 5. 生成STOP条件，结束传输
    status = ll_iic_stop(iic_bus);
    return status;
}

/**
 * @brief  清除I2C错误标志（AF/BERR/ARLO/OVR）
 * @param  iic_bus: I2C总线句柄
 * @retval iic_ll_status_t: 操作状态
 */
iic_ll_status_t ll_iic_clear_error_flags(iic_ll_bus_t* iic_bus)
{
    if (NULL == iic_bus || NULL == iic_bus->I2Cx)
        return IIC_NULL;

    // 清除常见错误标志
    LL_I2C_ClearFlag_AF(iic_bus->I2Cx);
    LL_I2C_ClearFlag_BERR(iic_bus->I2Cx);
    LL_I2C_ClearFlag_ARLO(iic_bus->I2Cx);
    LL_I2C_ClearFlag_OVR(iic_bus->I2Cx);

    return IIC_OK;
}