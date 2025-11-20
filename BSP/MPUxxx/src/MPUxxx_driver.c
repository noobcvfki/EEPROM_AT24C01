/******************************************************************************
* Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file MPUxxx_driver.c
 * @file bsp_mpu6050_reg.h
 *
 * @par dependencies
 *
 * - stdint.h
 * - mpu6050.h
 *
 * @author liu
 *
 * @brief Provide the HAL APIs of MPU6050 and corresponding options.
 *
 * Processing flow:
 *
 * call directly.
 *
 * @version V1.1 2024-12-10
 *
 * @note 1 tab == 4 spaces!
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "MPUxxx_driver.h"
#include "bsp_mpu6050_reg.h"
#include "elog.h"
//******************************** Includes *********************************//
//---------------------------------------------------------------------------//
//******************************** Defines **********************************//
#define IIC_MEMADD_SIZE_8BIT 0x00000001U  //参数，说明iic目标的内存地址是八位地址
#define TIME_OUT_MS 1000
#define MPUXXX_NOT_INIT   0
#define MPUXXX_INIT       1
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "MPUXXX_driver"
#else
#define LOG_TAG "MPUXXX_driver"
#endif
//******************************** Defines **********************************//
//---------------------------------------------------------------------------//
//******************************** Macros ***********************************//
#define MPUXXX_WRITE_REG(p_mpu_driver,reg,p_data,len)\
   p_mpu_driver->p_iic_driver_interface->pf_iic_mem_write(\
   p_mpu_driver->p_iic_driver_interface->hi2c,\
   (MPU_ADDR<<1) | 0,\
   reg,\
   IIC_MEMADD_SIZE_8BIT,\
   p_data,\
   len,\
   TIME_OUT_MS\
)
#define MPUXXX_READ_REG(p_mpu_driver,reg,p_data,len)\
    p_mpu_driver->p_iic_driver_interface->pf_iic_mem_read(\
    p_mpu_driver->p_iic_driver_interface->hi2c,\
    (MPU_ADDR<<1)|1,\
    reg,\
    IIC_MEMADD_SIZE_8BIT,\
    p_data,\
    len,\
    TIME_OUT_MS\
)

#define DEBUG

#ifdef DEBUG
#define LOG_DEBUG(x)  log_d(x)
#define LOG_ERROR(x)  log_e(x)
#define NULL_CHECK(x,tag) do{\
    if(NULL == x){\
        LOG_ERROR("discover null prt");\
        goto tag;\
    }}while (0)
#else
#define DEBUG_LOG(x)        (0)
#define NULL_CHECK(x,tag)   (0)
#endif//end of DEBUG

//******************************** Macros ***********************************//
//---------------------------------------------------------------------------//
//******************************* Functions *********************************//
mpuxxx_status_t bsp_mpuxxx_driver_inst(
    bsp_mpuxxx_driver_t    *p_mpuxxx_driver,
    iic_driver_interface_t *p_iic_driver_interface,
#ifdef OS_SUPPORTING
    yield_interface_t      *p_yield_interface,
    os_interface_t         *p_os_interfece,
#endif //OS_SUPPORTING
    delay_interface_t      *p_delay_interface,
    timebase_interface_t   *p_timebase_interface,
    void                  (*callback_register)
                           (void (*callback)(void *, void *)),
    void                  (*callback_register_dma)
                           (void (*callback)(void *, void *)),
#ifdef OS_SUPPORTING
    void                   *queue_handle,
    void                   *semaphore_handle,
    void                   *notify_handle)
#endif//OS_SUPPORTING

{
    mpuxxx_status_t ret = MPUxxx_OK;
//******************************* 检查参数 *********************************//
    NULL_CHECK(p_mpuxxx_driver,                       mpu_driver_inst_null);
    NULL_CHECK(p_iic_driver_interface,                mpu_driver_inst_null);
#ifdef OS_SUPPORTING
    NULL_CHECK(p_yield_interface,                     mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece,                        mpu_driver_inst_null);
#endif //OS_SUPPORTING
    NULL_CHECK(p_delay_interface,                     mpu_driver_inst_null);
    NULL_CHECK(p_timebase_interface,                  mpu_driver_inst_null);
    NULL_CHECK(callback_register,                     mpu_driver_inst_null);
    NULL_CHECK(callback_register_dma,                 mpu_driver_inst_null);
#ifdef OS_SUPPORTING
    NULL_CHECK(queue_handle,                          mpu_driver_inst_null);
    NULL_CHECK(semaphore_handle,                      mpu_driver_inst_null);
    NULL_CHECK(notify_handle,                         mpu_driver_inst_null);
#endif //OS_SUPPORTING
    //******************************* 检查IIC参数 *******************************//
    NULL_CHECK(p_iic_driver_interface->pf_iic_init,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_iic_driver_interface->pf_iic_deinit,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_iic_driver_interface->pf_iic_mem_write,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_iic_driver_interface->pf_iic_mem_read,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_iic_driver_interface->pf_iic_mem_read_dma,
                                                         mpu_driver_inst_null);
    p_mpuxxx_driver->p_iic_driver_interface = p_iic_driver_interface;
#ifdef OS_SUPPORTING
    //**************************** 检查OS延时参数 *********************************//
    NULL_CHECK(p_yield_interface->pf_rtos_yield,
                                              mpu_driver_inst_null);
    p_mpuxxx_driver->p_yield_interface = p_yield_interface;
    //**************************** 检查OS参数 *********************************//
    NULL_CHECK(p_os_interfece->os_queue_create,          mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_queue_put,             mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_queue_put_isr,         mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_queue_get,             mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_queue_delete,          mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_create_mutex,mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_delete_mutex,mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_lock_mutex,  mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_unlock_mutex,mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_create_binary,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_delete_binary,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_wait_binary, mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_signal_binary,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_signal_binary_isr,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_signal_notify_isr,
                                                         mpu_driver_inst_null);
    NULL_CHECK(p_os_interfece->os_semaphore_wait_notify, mpu_driver_inst_null);
    p_mpuxxx_driver->p_os_interface = p_os_interfece;
#endif
    //**************************** 检查阻塞延时 *********************************//
    NULL_CHECK(p_delay_interface->pf_delay_init,         mpu_driver_inst_null);
    NULL_CHECK(p_delay_interface->pf_delay_us,           mpu_driver_inst_null);
    NULL_CHECK(p_delay_interface->pf_delay_ms,           mpu_driver_inst_null);
    p_mpuxxx_driver->p_delay_interface = p_delay_interface;
    //**************************** 时基获取接口 *********************************//
    NULL_CHECK(p_timebase_interface->pf_get_tick_count_ms,
                                                         mpu_driver_inst_null);
    p_mpuxxx_driver->p_timebase_interface = p_timebase_interface;
mpu_driver_inst_null:
    {
        log_e("bsp_mpuxxx_driver_inst");
        return MPUxxx_ERRORPARAMETER;
    }
}