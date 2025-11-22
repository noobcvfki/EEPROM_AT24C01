//
// Created by capting on 2025/11/22.
//

#ifndef USER_MPUXXX_AT24C02_DRIVER_H
#define USER_MPUXXX_AT24C02_DRIVER_H
//******************************** Includes *********************************//
#include <stdint.h>
//******************************** Includes *********************************//
//---------------------------------------------------------------------------//
//******************************** Includes *********************************//
//---------------------------------------------------------------------------//
//****************************** define enum ********************************//
typedef enum
{
    EEPROM_OK             = 0,          //*操作执行成功
    EEPROM_ERROR          = 1,          //*无匹配场景的运行时错误
    EEPROM_ERRORTIMEOUT   = 2,          //*操作超时失败
    EEPROM_ERRORRESOURCE  = 3,          //*资源不可用
    EEPROM_ERRORPARAMETER = 4,          //*参数错误
    EEPROM_ERRORNOMEMORY  = 5,          //*内存不足
    EEPROM_ERRORISR       = 6,          //*不允许在ISR（中断服务程序）上下文使用
    EEPROM_RESERVED       = 0x7FFFFFFF, //*保留（预留）
} eeprom_status_t;
//****************************** define enum ********************************//
//---------------------------------------------------------------------------//
//******************************** macro ************************************//
#define EEPROM_READ_ADDR   0xA1
#define EEPROM_WRITE_ADDR  0xA0
//******************************** macro ************************************//
//---------------------------------------------------------------------------//
//******************************* struct ************************************//
typedef struct
{
    void* iic_handle;
    eeprom_status_t (*pf_iic_init)(void* p_iic_handle);
    eeprom_status_t (*pf_iic_deinit)(void* p_iic_handle);
    eeprom_status_t (*pf_iic_mem_read)(void* p_iic_handle,uint8_t* data);
    eeprom_status_t (*pf_iic_mem_write)(void* p_iic_handle,uint8_t);
    void (*critical_enable)(void);
    void (*critical_disable)(void);
}eeprom_iic_driver_t;
#endif //USER_MPUXXX_AT24C02_DRIVER_H