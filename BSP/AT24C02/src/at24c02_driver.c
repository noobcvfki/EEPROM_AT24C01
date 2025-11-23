//
// Created by capting on 2025/11/22.
//
//******************************** Includes *********************************//
#include "at24c02_driver.h"
#include "elog.h"
//******************************** Includes *********************************//
//---------------------------------------------------------------------------//
//********************************* define **********************************//
#ifdef LOG_TAG
    #undef LOG_TAG
    #define LOG_TAG "eeprom_driver"
#else
    #define LOG_TAG "eeprom_driver"
#endif

#define DEBUG

#ifdef DEBUG
    #define DEBUG_LOG(x,...)   log_d(x,##__VA_ARGS__)
    #define ERROR_LOG(x,...)   log_e(x,##__VA_ARGS__)
#else
#define DEBUG_LOG(x,...)    (void(0))
#define ERROR_LOG(x,...)    (void(0))
#endif

#define NULL_CHECK(x) do{                                     \
                            if(NULL == x){                    \
                                ERROR_LOG("check null ptr");  \
                                goto NULL_ERROR;              \
                            }                                 \
                      }while (0)

#define RETURN_CHECK(x) do{                                   \
                            if(EEPROM_OK != x){               \
                                ERROR_LOG("function is ng");  \
                                goto RETURN_ERROR;            \
                            }                                 \
                         }while (0)

#define IIC_HANDLE p_eeprom->p_eeprom_software_iic_driver->iic_handle
#define IIC_INSTANCE p_eeprom->p_eeprom_software_iic_driver
//********************************* define **********************************//
//---------------------------------------------------------------------------//
//******************************** Function *********************************//
static eeprom_status_t eeprom_readid(bsp_eeprom_driver_t* p_eeprom)
{
    eeprom_status_t ret = EEPROM_OK;
    IIC_INSTANCE->critical_enable();
//******************************** critical *********************************//
    IIC_INSTANCE->pf_iic_start(IIC_HANDLE);
    IIC_INSTANCE->pf_iic_send_byte(IIC_HANDLE,p_eeprom->iic_write_addr);
    ret=
    IIC_INSTANCE->pf_iic_wait_ack(IIC_HANDLE);
    IIC_INSTANCE->pf_iic_stop(IIC_HANDLE);
//******************************** critical *********************************//
    IIC_INSTANCE->critical_disable();
    if (ret != 0)
    {
        ERROR_LOG("eeprom not ack");
        return EEPROM_ERROR;
    }
    DEBUG_LOG("iic is ack");
    return ret;

}

static eeprom_status_t eeprom_init(bsp_eeprom_driver_t* p_eeprom)
{
    eeprom_status_t ret = EEPROM_OK;
    ret = IIC_INSTANCE->pf_iic_init(IIC_HANDLE);
    RETURN_CHECK(ret);
    ret = eeprom_readid(p_eeprom);
    RETURN_CHECK(ret);
    return ret;
RETURN_ERROR:
    {
        ERROR_LOG("eeprom_init is ng");
        return ret;
    }
}



static eeprom_status_t eeprom_write(bsp_eeprom_driver_t* p_eeprom,
                                    uint8_t write_data_addr,
                                    uint8_t* p_data,
                                    uint8_t  len)
{
    eeprom_status_t ret = EEPROM_OK;
    uint8_t remaining_bytes = len; // 剩余要写的字节数
    uint8_t current_addr = write_data_addr; // 当前要写的 EEPROM 内部地址
    uint8_t* p_current_data = p_data; // 当前要写的数据指针

    // 优化：分开检查 NULL，日志准确
    if (p_data == NULL)
    {
        ERROR_LOG("input p_data is null");
        return EEPROM_ERRORPARAMETER;
    }
    if (p_eeprom == NULL)
    {
        ERROR_LOG("input p_eeprom is null");
        return EEPROM_ERRORPARAMETER;
    }
    if (0 == len)
    {
        ERROR_LOG("input len is 0");
        return EEPROM_ERRORPARAMETER;
    }

    // 循环处理跨页写（直到所有字节写完）
    while (remaining_bytes > 0)
    {
        // 计算当前页能写的最大字节数（不超过页大小，不跨页）
        uint8_t page_remain = EEPROM_PAGE_SIZE - (current_addr % EEPROM_PAGE_SIZE);
        uint8_t write_bytes = (remaining_bytes > page_remain) ? page_remain : remaining_bytes;

        IIC_INSTANCE->critical_enable();
        //---------------------critical----------------------------//
        IIC_INSTANCE->pf_iic_start(IIC_HANDLE);
        // 发送写器件地址
        IIC_INSTANCE->pf_iic_send_byte(IIC_HANDLE, p_eeprom->iic_write_addr);
        ret = IIC_INSTANCE->pf_iic_wait_ack(IIC_HANDLE);
        RETURN_CHECK(ret);

        // 发送当前页的起始地址
        IIC_INSTANCE->pf_iic_send_byte(IIC_HANDLE, current_addr);
        ret = IIC_INSTANCE->pf_iic_wait_ack(IIC_HANDLE);
        RETURN_CHECK(ret);

        // 发送当前页的字节（不跨页）
        for (uint8_t i = 0; i < write_bytes; i++)
        {
            IIC_INSTANCE->pf_iic_send_byte(IIC_HANDLE, *(p_current_data + i));
            ret = IIC_INSTANCE->pf_iic_wait_ack(IIC_HANDLE);
            RETURN_CHECK(ret);
        }

        // 发送 STOP，结束当前页写操作
        IIC_INSTANCE->pf_iic_stop(IIC_HANDLE);
        //---------------------critical----------------------------//
        IIC_INSTANCE->critical_disable();

        // 关键：EEPROM 写周期延时（50ms 兼容绝大多数型号）
        p_eeprom->p_os_yield->delay_ms(50);// 若用 FreeRTOS，也可用 vTaskDelay(pdMS_TO_TICKS(50))

        // 更新剩余字节数、当前地址、当前数据指针
        remaining_bytes -= write_bytes;
        current_addr += write_bytes;
        p_current_data += write_bytes;
    }

    return ret;

RETURN_ERROR:
    ERROR_LOG("eeprom write failed, ret=%d, current_addr=0x%02X", ret, current_addr);
    // 出错时必须发送 STOP，释放 I2C 总线
    IIC_INSTANCE->pf_iic_stop(IIC_HANDLE);
    // 关闭临界区，避免系统异常
    IIC_INSTANCE->critical_disable();
    return ret;
}
static eeprom_status_t eeprom_read(bsp_eeprom_driver_t* p_eeprom,
                                               uint8_t  read_data_addr,
                                               uint8_t* p_data,
                                               uint8_t  len)
{
    eeprom_status_t ret = EEPROM_OK;
    NULL_CHECK(p_data);
    NULL_CHECK(p_eeprom);
    if (0 == len)
    {
        ERROR_LOG("input len is 0");
        return EEPROM_ERRORPARAMETER;
    }
    IIC_INSTANCE->critical_enable();
    //---------------------critical----------------------------//
    IIC_INSTANCE->pf_iic_start(IIC_HANDLE);
    IIC_INSTANCE->pf_iic_send_byte(IIC_HANDLE,p_eeprom->iic_write_addr);
    ret = IIC_INSTANCE->pf_iic_wait_ack(IIC_HANDLE);
    RETURN_CHECK(ret);
    IIC_INSTANCE->pf_iic_send_byte(IIC_HANDLE,read_data_addr);
    ret = IIC_INSTANCE->pf_iic_wait_ack(IIC_HANDLE);
    RETURN_CHECK(ret);

    IIC_INSTANCE->pf_iic_start(IIC_HANDLE);
    IIC_INSTANCE->pf_iic_send_byte(IIC_HANDLE,p_eeprom->iic_read_addr);
    ret = IIC_INSTANCE->pf_iic_wait_ack(IIC_HANDLE);
    RETURN_CHECK(ret);
    for (uint8_t i = 0; i<len;i++)
    {
        IIC_INSTANCE->pf_iic_read_byte(IIC_HANDLE,p_data+i);
        if (len-1 == i) break;
        ret = IIC_INSTANCE->pf_iic_send_ack(IIC_HANDLE);
        RETURN_CHECK(ret);
    }
    IIC_INSTANCE->pf_iic_no_ack(IIC_HANDLE);
    IIC_INSTANCE->pf_iic_stop(IIC_HANDLE);
    //---------------------critical----------------------------//
    IIC_INSTANCE->critical_disable();

    return ret;

    RETURN_ERROR:
    {
        ERROR_LOG("eeprom not ack");
        return ret;
    }
    NULL_ERROR:
        {
            ERROR_LOG("input p_data is null");
            return EEPROM_ERRORPARAMETER;
        }
}





#ifdef SOFTWARE_IIC
eeprom_status_t eeprom_inst(bsp_eeprom_driver_t* p_eeprom,
                                 uint8_t eeprom_7bit_addr,
                                 os_yield_t* p_os_yield,
                      eeprom_software_iic_driver_t* p_iic)
{
    DEBUG_LOG("=============eeprom inst start==========");
    eeprom_status_t ret = EEPROM_OK;
    NULL_CHECK(p_eeprom                                 );
    NULL_CHECK(p_iic                                    );
    NULL_CHECK(p_iic->iic_handle                        );
    NULL_CHECK(p_iic->pf_iic_init                       );
    NULL_CHECK(p_iic->pf_iic_deinit                     );
    NULL_CHECK(p_iic->pf_iic_start                      );
    NULL_CHECK(p_iic->pf_iic_send_byte                  );
    NULL_CHECK(p_iic->pf_iic_wait_ack                   );
    NULL_CHECK(p_iic->pf_iic_read_byte                  );
    NULL_CHECK(p_iic->pf_iic_send_ack                   );
    NULL_CHECK(p_iic->pf_iic_no_ack                     );
    NULL_CHECK(p_iic->pf_iic_stop                       );
    NULL_CHECK(p_iic->critical_enable                   );
    NULL_CHECK(p_iic->critical_disable                  );
    NULL_CHECK(p_os_yield);
    NULL_CHECK(p_os_yield->delay_ms);

    p_eeprom->p_eeprom_software_iic_driver = p_iic;
    p_eeprom->iic_handle       = p_iic->iic_handle;
    p_eeprom->iic_read_addr    = (eeprom_7bit_addr<<1)|1;
    p_eeprom->iic_write_addr   = (eeprom_7bit_addr<<1)|0;

    p_eeprom->pf_eeprom_init   = eeprom_init;
    p_eeprom->pf_eeprom_readid = eeprom_readid;
    p_eeprom->pf_eeprom_write  = eeprom_write;
    p_eeprom->pf_eeprom_read   = eeprom_read;

    p_eeprom->p_os_yield = p_os_yield;


    ret = eeprom_init(p_eeprom);
    if (EEPROM_OK!=ret)
    {
        ERROR_LOG("eeprom init is ng");
        return ret;
    }
    DEBUG_LOG("======eeprom inst is end=========");
    return ret;
NULL_ERROR:
    {
        ERROR_LOG("eeprom inst check null ptr");
        return EEPROM_ERROR;
    }
}
#else
eeprom_status_t eeprom_inst(bsp_eeprom_driver_t* p_eeprom,
                                 uint8_t eeprom_7bit_addr,
                      eeprom_hardware_iic_driver_t* p_iic);
#endif