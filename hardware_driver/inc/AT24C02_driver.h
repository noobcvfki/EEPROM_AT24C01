
#ifndef AT24C02_H
#define AT24C02_H
//******************************** Includes *********************************//
#include "stm32f1xx_hal.h"
#include "iic_hal_software.h"

//***************************** Includes END *********************************//

//******************************** Defines **********************************//
#define OS_SUPPORTING
// #define HARDWARE_IIC

#define AT24C02_ADD       ( 0x50U      )
#define AT24C02_READ_ADD  ((0x50U<<1)|1)
#define AT24C02_WRITE_ADD ((0x50U<<1)|0)

typedef enum
{
  AT24C02_OK                = 0,         /* 操作已成功完成 */
  AT24C02_ERROR             = 1,         /* 运行错误      */
  AT24C02_ERRORTIMEOUT      = 2,         /* 操作超时      */
  AT24C02_ERRORRESOURCE     = 3,         /* 资源不可用    */
  AT24C02_ERRORPARAMETER    = 4,         /* 参数错误      */
  AT24C02_ERRORNOMEMORY     = 5,         /* 内存不足      */
  AT24C02_ERRORISR          = 6,         /* 禁止中断操作   */
  AT24C02_RESERVED          = 0x7FFFFFFF /* 保留         */
}at24c02_status_t;
//******************************** Defines END ************************//

//******************************** Declaring **********************//
#ifndef HARDWARE_IIC
/************************iic接口类型传递**********************/
typedef iic_bus_t iic_driver_t;
/************************AT24C02的iic驱动接口结构体**********************/
typedef struct
{
  //iic初始化函数
  at24c02_status_t (*pf_iic_init)         (void*);
  //iic反初始化函数
  at24c02_status_t (*pf_iic_deinit)       (void*);
  //iic初始信号
  at24c02_status_t (*pf_iic_start)        (void*);
  //iic停止条件
  at24c02_status_t (*pf_iic_stop)         (void*);
  //iic等待回应
  at24c02_status_t (*pf_iic_wait_ack)     (void*);
  //iic发送回应
  at24c02_status_t (*pf_iic_send_ack)     (void*);
  //iic不发送回应
  at24c02_status_t (*pf_iic_send_no_ack)  (void*);
  //iic发送一个字节
  at24c02_status_t (*pf_iic_send_byte)    (void*,const uint8_t);
  //iic接收一个字节
  at24c02_status_t (*pf_iic_receive_byte) (void*,uint8_t * const);
  //iic进入临界态
  at24c02_status_t (*pf_critical_enter)   (void);
  //iic退出临界态
  at24c02_status_t (*pf_critical_exit)    (void);

}iic_driver_interface_t;

#endif //end of HARDWARE_IIC

/*************************获取时基***************************/
typedef struct
{
  uint32_t (*pf_get_tick_count)(void);
}timebase_interface_t;
/************************* 让出CPU ***************************/
#ifdef OS_SUPPORTING
//来自os的os_delay,让出CPU使用权
typedef struct
{
  void (*pf_rtos_yield) (const uint32_t);
}yield_interface_t;
#endif //end of OS_SUPPORTING

/************************* BSP整体接口 ***************************/
//前向声明：
typedef struct temp_bsp_at24c02_driver_t bsp_at24c02_driver_t;
// at24c02硬件抽象层
struct temp_bsp_at24c02_driver_t
{
  /*********结构体接口*******/
  //iic需要定义的接口
  iic_driver_interface_t * p_iic_driver_interface;
  //时基获取接口
  timebase_interface_t   * p_timebase_interface;
#ifdef OS_SUPPORTING
  //os_delay接口
  yield_interface_t      * p_yield_interface;
#endif //end of os_supporting

  /*******函数指针接口***/
  //AT24C02实例化函数
  at24c02_status_t (*pf_inst) (
                    const void                   *   at24c02_interface,
                    const iic_driver_interface_t *   p_iic_driver_interface,
                    const timebase_interface_t   *   p_timebase_interface,
#ifdef OS_SUPPORTING
                    const yield_interface_t      *   p_yield_interface);
#endif

  //初始化函数指针
  at24c02_status_t (*pf_init)    (bsp_at24c02_driver_t * const);
  //反初始化指针
  at24c02_status_t (*pf_deinit)  (bsp_at24c02_driver_t * const);
  //读取ID
  at24c02_status_t (*pf_read_ID) (bsp_at24c02_driver_t * const);
  //at24c02写操作
  at24c02_status_t (*pf_write)   (bsp_at24c02_driver_t * const,
                                  const uint8_t   write_address,
                                  const uint8_t   write_data);
  //at24c02读操作
  at24c02_status_t (*pf_read)    (bsp_at24c02_driver_t * const,
                                  const uint8_t  read_address,
                                  uint8_t* const read_data);
};

at24c02_status_t at24c02_inst(
  bsp_at24c02_driver_t   * p_at24c02_instance,
  iic_driver_interface_t * p_iic_driver_interface,
#ifdef OS_SUPPORTING
  yield_interface_t      * p_yield_interface,
#endif
  timebase_interface_t   * p_timebase_interface);
//******************************** Declaring ********************************//

#endif //AT24C02_H
