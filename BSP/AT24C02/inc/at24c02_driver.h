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
#define SOFTWARE_IIC
//******************************** macro ************************************//
//---------------------------------------------------------------------------//
//******************************* struct ************************************//
//软件IIC
#ifdef SOFTWARE_IIC
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
#else
//硬件IIC
typedef struct
{
    void *hi2c;             /* hi2c pointer to a I2C_HandleTypeDef structure */
    eeprom_status_t (*pf_iic_init)      (void *);   /* IIC init    interface */
    eeprom_status_t (*pf_iic_deinit)    (void *);   /* IIC deinit  interface */

    eeprom_status_t (*pf_iic_mem_write)(void *hi2c,
                            /*七位地址：*/uint16_t dst_address,
                            /*内存地址：*/uint16_t mem_addr,
                            /*内存大小：*/uint16_t mem_size,
                            /*写入指针：*/uint8_t  *p_data,
                            /*写入大小：*/uint16_t size,
                            /*等待时间：*/uint32_t timeout);

    eeprom_status_t (*pf_iic_mem_read) (void *hi2c,
                            /*七位地址：*/uint16_t dst_address,
                            /*内存地址：*/uint16_t mem_addr,
                            /*内存大小：*/uint16_t mem_size,
                            /*写入指针：*/uint8_t  *p_data,
                            /*写入大小：*/uint16_t size,
                            /*等待时间：*/uint32_t timeout);
    // Use DMA to asynchronously read data from the I2C device's registers.
    eeprom_status_t (*pf_iic_mem_read_dma)
                                       (void *hi2c,
                                        uint16_t dst_address,
                                        uint16_t mem_addr,
                                        uint16_t mem_size,
                                        uint8_t  *p_data,
                                        uint16_t size );
}eeprom_hardware_iic_driver_t;
#endif
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
#ifdef SOFTWARE_IIC
    eeprom_software_iic_driver_t* p_eeprom_software_iic_driver;
#else
    eeprom_hardware_iic_driver_t* p_eeprom_hardware_iic_driver;
#endif
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

#ifdef SOFTWARE_IIC
eeprom_status_t eeprom_inst(bsp_eeprom_driver_t* p_eeprom,
                                 uint8_t eeprom_7bit_addr,
                                 os_yield_t* p_os_yield,
                      eeprom_software_iic_driver_t* p_iic);
#else
eeprom_status_t eeprom_inst(bsp_eeprom_driver_t* p_eeprom,
                                 uint8_t eeprom_7bit_addr,
                      eeprom_hardware_iic_driver_t* p_iic);
#endif
#endif //USER_MPUXXX_AT24C02_DRIVER_H