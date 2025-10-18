#include "AT24C02_driver.h"

#include "elog.h"

#define AT24C02_NOT_INITED 0  //没有初始化化flag
#define AT24C02_INITED     1  //初始化flag

#define IS_INITED    (AT24C02_INITED == init_flag)  //如果已经被定义

static uint8_t init_flag = AT24C02_NOT_INITED;

/**
 * 函数用于查看是芯片否存在，是否有回应
 * @param p_at24c02_instance BSP层的驱动结构体
 * @return 状态
 */
static at24c02_status_t at24c02_probe(const bsp_at24c02_driver_t * p_at24c02_instance)
{

#ifndef HARDWARE_IIC
  p_at24c02_instance->p_iic_driver_interface->pf_critical_enter();
#endif //end of HARDWARE_IIC
  //初始条件
  p_at24c02_instance->p_iic_driver_interface->pf_iic_start(NULL);
  //发送器件ID
  p_at24c02_instance->p_iic_driver_interface->pf_iic_send_byte(NULL,AT24C02_WRITE_ADD);
  //等待回应
  if (AT24C02_OK == \
        p_at24c02_instance->p_iic_driver_interface->pf_iic_wait_ack(NULL))
  {
    p_at24c02_instance->p_iic_driver_interface->pf_iic_stop(NULL);
    return AT24C02_OK;
  }
  else{
    return AT24C02_ERROR;
  }
}

static at24c02_status_t at24c02_read_id( bsp_at24c02_driver_t * const p_at24c02_instance)
{
  if (AT24C02_ERROR == at24c02_probe(p_at24c02_instance))
  {
    return AT24C02_ERROR;
  }
  return AT24C02_ADD;
}

/**
 * 这个函数用于初始化AT24C02，包括初始化IIC
 * @param p_at24c02_instance 最顶层的封装结构体
 * @return 是否运行成功
 */
static at24c02_status_t at24c02_init(const bsp_at24c02_driver_t * p_at24c02_instance)
{
  at24c02_status_t ret = AT24C02_OK;
  //判断传入的指针是否为空指针
  if (NULL == p_at24c02_instance->p_iic_driver_interface||
      NULL == p_at24c02_instance->p_iic_driver_interface->pf_iic_init)
  {
    log_e("AT24C02","p_iic_driver_interface is NULL");
    return AT24C02_ERROR;
  }
  //开始初始化
#ifndef HARDWARE_IIC
  p_at24c02_instance->p_iic_driver_interface->pf_critical_enter(); //进入临界区
#endif
  p_at24c02_instance->p_iic_driver_interface->pf_iic_init(NULL);
  log_d("IIC","iic is init ....");
#ifndef HARDWARE_IIC
  p_at24c02_instance->p_iic_driver_interface->pf_critical_exit();//退出临界区
#endif
  //呼唤AT24，查看是否有回应
  if (AT24C02_OK == at24c02_probe(p_at24c02_instance))
  {
    init_flag = AT24C02_INITED;
    return ret;
  }
  else
  {
    log_e("AT24C02","AT24C02 is ERROR,ID is ERROR");
    return AT24C02_ERROR;
  }
}

static at24c02_status_t at24c02_write(const bsp_at24c02_driver_t * p_at24c02_driver,
                                      uint8_t const write_address,
                                      uint8_t const write_data)
{
  if (!IS_INITED) return AT24C02_ERROR;
  /****************起始条件******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_start(NULL);
  /****************发送设备地址******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_send_byte(NULL,AT24C02_WRITE_ADD);
  /****************等待回应******************/
  if (1 == \
    p_at24c02_driver->p_iic_driver_interface->pf_iic_wait_ack(NULL))
  {
    return AT24C02_ERRORTIMEOUT;
  }
  /****************发送写地址******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_send_byte(NULL,write_address);
  /****************等待回应******************/
  if (1 ==
    p_at24c02_driver->p_iic_driver_interface->pf_iic_wait_ack(NULL))
  {
    return AT24C02_ERRORTIMEOUT;
  }
  /****************发送写数据******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_send_byte(NULL,write_data);
  /****************等待回应******************/
  if (1 ==
    p_at24c02_driver->p_iic_driver_interface->pf_iic_wait_ack(NULL))
  {
    return AT24C02_ERRORTIMEOUT;
  }
  /****************停止位******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_stop(NULL);
  return AT24C02_OK;

}

static at24c02_status_t at24c02_read(const bsp_at24c02_driver_t * p_at24c02_driver,
                                     const uint8_t read_address,
                                     uint8_t * const read_data)
{
  if (!IS_INITED) return AT24C02_ERROR;
  /****************起始条件******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_start(NULL);
  /****************发送设备地址******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_send_byte(NULL,AT24C02_WRITE_ADD);
  /****************等待回应******************/
  if (1 == \
    p_at24c02_driver->p_iic_driver_interface->pf_iic_wait_ack(NULL))
  {
    return AT24C02_ERRORTIMEOUT;
  }
  /****************发送写地址******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_send_byte(NULL,read_address);
  /****************等待回应******************/
  if (1 ==
    p_at24c02_driver->p_iic_driver_interface->pf_iic_wait_ack(NULL))
  {
    return AT24C02_ERRORTIMEOUT;
  }
  /****************起始条件******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_start(NULL);
  /****************发送设备地址******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_send_byte(NULL,AT24C02_READ_ADD);
  /****************等待回应******************/
  if (1 ==
    p_at24c02_driver->p_iic_driver_interface->pf_iic_wait_ack(NULL))
  {
    return AT24C02_ERRORTIMEOUT;
  }
  /****************接收数据******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_receive_byte(NULL,read_data);
  /****************发送回应******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_send_no_ack(NULL);
  /****************停止位******************/
  p_at24c02_driver->p_iic_driver_interface->pf_iic_stop(NULL);
  return AT24C02_OK;

}
//TODO:函数还未完成
static at24c02_status_t at24c02_deinit(const bsp_at24c02_driver_t * p_at24c02_driver)
{
  if (!IS_INITED) return AT24C02_ERRORRESOURCE;
  return AT24C02_OK;
}

at24c02_status_t at24c02_inst(
  bsp_at24c02_driver_t   * p_at24c02_instance,
  iic_driver_interface_t * p_iic_driver_interface,
#ifdef OS_SUPPORTING
  yield_interface_t      * p_yield_interface,
#endif
  timebase_interface_t   * p_timebase_interface)
{
  if (IS_INITED) return AT24C02_ERRORRESOURCE;

  log_d("inst","at24c02 is inst");
  uint8_t ret = 0;
  if (NULL == p_at24c02_instance||\
      NULL == p_iic_driver_interface)
  {
    return 1;
  }
  p_at24c02_instance->p_iic_driver_interface = p_iic_driver_interface;
  p_at24c02_instance->p_timebase_interface = p_timebase_interface;
#ifdef OS_SUPPORTING
  p_at24c02_instance->p_yield_interface = p_yield_interface;
#endif
  p_at24c02_instance->pf_init =
    (at24c02_status_t (*)( bsp_at24c02_driver_t* const))at24c02_probe;
  p_at24c02_instance->pf_read_ID =
    (at24c02_status_t (*)( bsp_at24c02_driver_t * const))at24c02_read_id;
  p_at24c02_instance->pf_read =
    (at24c02_status_t (*)( bsp_at24c02_driver_t * const,const uint8_t, uint8_t * const))at24c02_read;
  p_at24c02_instance->pf_write =
    (at24c02_status_t (*) ( bsp_at24c02_driver_t * const,const uint8_t, const uint8_t))at24c02_write;
  p_at24c02_instance->pf_deinit =
    (at24c02_status_t (*)( bsp_at24c02_driver_t * const))at24c02_deinit;

  ret = at24c02_init(p_at24c02_instance);
  if (ret)
  {
    log_e("AT24","AT24 eeror ret is: %d",ret);
    return AT24C02_ERRORRESOURCE;
  }
  log_d("AT24","AT24 inst end");
  return  AT24C02_OK;
}
