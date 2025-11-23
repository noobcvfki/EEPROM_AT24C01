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

#define EEPROM_PAGE_SIZE 8
//******************************** macro ************************************//
//---------------------------------------------------------------------------//
//******************************* struct ************************************//

typedef struct
{
    void* iic_handle;
    eeprom_status_t (*pf_iic_init     )(void* p_iic_handle);
    eeprom_status_t (*pf_iic_deinit   )(void* p_iic_handle);
    eeprom_status_t (*pf_iic_start    )(void* p_iic_handle);
    eeprom_status_t (*pf_iic_send_byte)(void* p_iic_handle,uint8_t data);
    eeprom_status_t (*pf_iic_wait_ack )(void* p_iic_handle);
    eeprom_status_t (*pf_iic_read_byte)(void* p_iic_handle,uint8_t* p_data);
    eeprom_status_t (*pf_iic_send_ack )(void* p_iic_handle);
    eeprom_status_t (*pf_iic_no_ack   )(void* p_iic_handle);
    eeprom_status_t (*pf_iic_stop     )(void* p_iic_handle);
    void            (*critical_enable )(void);
    void            (*critical_disable)(void);
}eeprom_software_iic_driver_t;

//******************************* struct ************************************//
//---------------------------------------------------------------------------//
//******************************* struct ************************************//
typedef struct bsp_eeprom_driver bsp_eeprom_driver_t ;

typedef struct
{
    void (*delay_ms)(uint32_t ms);
}os_yield_t;

typedef struct bsp_eeprom_driver
{
    uint8_t iic_read_addr;
    uint8_t iic_write_addr;
    void* iic_handle;
    eeprom_software_iic_driver_t* p_eeprom_software_iic_driver;
    os_yield_t* p_os_yield;

    eeprom_status_t (*pf_eeprom_init)(bsp_eeprom_driver_t* p_eeprom);
    eeprom_status_t (*pf_eeprom_readid)(bsp_eeprom_driver_t* p_eeprom);
    eeprom_status_t (*pf_eeprom_write)(bsp_eeprom_driver_t* p_eeprom,
                                      uint8_t write_addr,
                                      uint8_t* p_data,uint8_t len);
    eeprom_status_t (*pf_eeprom_read)(bsp_eeprom_driver_t* p_eeprom,
                                      uint8_t read_addr,
                                      uint8_t* p_data,uint8_t len);

}bsp_eeprom_driver_t;


eeprom_status_t eeprom_inst(bsp_eeprom_driver_t* p_eeprom,
                                 uint8_t eeprom_7bit_addr,
                                 os_yield_t* p_os_yield,
                      eeprom_software_iic_driver_t* p_iic);
#endif //USER_MPUXXX_AT24C02_DRIVER_H