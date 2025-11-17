//
// Created by capting on 2025/11/16.
//

#ifndef USER_MPUXXX_MPUXXX_DRIVER_H
#define USER_MPUXXX_MPUXXX_DRIVER_H
//******************************** Includes *********************************//
#include <stdint.h>
/*   函数返回值    */
typedef enum
{
    MPUxxx_OK             = 0,          /*Operation completed successfully   */
    MPUxxx_ERROR          = 1,          /*Run-time error without case matched*/
    MPUxxx_ERRORTIMEOUT   = 2,          /*Operation failed with timeout      */
    MPUxxx_ERRORRESOURCE  = 3,          /*Resource not available             */
    MPUxxx_ERRORPARAMETER = 4,          /*Parameter error                    */
    MPUxxx_ERRORNOMEMORY  = 5,          /*Out of memory                      */
    MPUxxx_ERRORISR       = 6,          /*Not allowed in ISR context         */
    MPUxxx_RESERVED       = 0x7FFFFFFF, /*Reserved                           */
} mpuxxx_status_t;

/* HAL_硬件IIC    */
typedef struct
{
    void *hi2c;             /* hi2c pointer to a I2C_HandleTypeDef structure */
    mpuxxx_status_t (*pf_iic_init)      (void *);   /* IIC init    interface */
    mpuxxx_status_t (*pf_iic_deinit)    (void *);   /* IIC deinit  interface */

    mpuxxx_status_t (*pf_iic_mem_write)(void *hi2c,
                            /*七位地址：*/uint16_t dst_address,
                            /*内存地址：*/uint16_t mem_addr,
                            /*内存大小：*/uint16_t mem_size,
                            /*写入指针：*/uint8_t  *p_data,
                            /*写入大小：*/uint16_t size,
                            /*等待时间：*/uint32_t timeout);

    mpuxxx_status_t (*pf_iic_mem_read) (void *hi2c,
                            /*七位地址：*/uint16_t dst_address,
                            /*内存地址：*/uint16_t mem_addr,
                            /*内存大小：*/uint16_t mem_size,
                            /*写入指针：*/uint8_t  *p_data,
                            /*写入大小：*/uint16_t size,
                            /*等待时间：*/uint32_t timeout);
    // Use DMA to asynchronously read data from the I2C device's registers.
    mpuxxx_status_t (*pf_iic_mem_read_dma)
                                       (void *hi2c,
                                        uint16_t dst_address,
                                        uint16_t mem_addr,
                                        uint16_t mem_size,
                                        uint8_t  *p_data,
                                        uint16_t size );
} iic_driver_interface_t;


typedef struct
{
    mpuxxx_status_t (*pf_init)              (void);
    mpuxxx_status_t (*pf_deinit)            (void);
    mpuxxx_status_t (*pf_enable_interrupt)  (void);
    mpuxxx_status_t (*pf_disable_interrupt) (void);
    mpuxxx_status_t (*pf_enable_clock)      (void);
    mpuxxx_status_t (*pf_disable_clock)     (void);
}hardware_interrupt_interface_t;

typedef struct
{
    uint32_t (*pf_get_tick_count_ms) (void);
}timebase_interface_t;

typedef struct
{
    uint8_t* (*pf_buffer_init)  (uint8_t ,uint8_t);
    uint8_t* (*pf_get_rbuffer_addr)(void);
    uint8_t* (*pf_get_wbuffer_addr)(void);
}buffer_interface_t;

typedef struct
{
    void (*pf_delay_init)(void);
    void (*pf_delay_us)(const uint32_t);
    void (*pf_delay_ms)(const uint32_t);
}delay_interface_t;

typedef struct
{
    void (*pf_rtos_yield)  (const uint32_t);
}yield_interface_t;

typedef struct
{
    mpuxxx_status_t (*os_queue_create)(const uint32_t num ,
                                       const uint32_t size,
                                void** const queue_handle );
    mpuxxx_status_t (*os_queue_put)(void * const queue_handle)
}os_interface_t;

#endif //USER_MPUXXX_MPUXXX_DRIVER_H