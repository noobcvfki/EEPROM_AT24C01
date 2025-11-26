//
// Created by capting on 2025/11/26.
//


//********************************* Includes *********************************//
#include <stdint.h>
#include "MPUxxx_handler.h"
#include "elog.h"
//********************************* Includes *********************************//
//----------------------------------------------------------------------------//
//********************************* defFunc **********************************//
#define NULL_CHECK(x) do{                                  \
                          if(NULL == x){                   \
                            LOG_ERROR("[%s] is null prt",#x);\
                            goto NULL_ERROR;}              \
                         }while (0)
#define RETURN_CHECK(x) do{                                             \
                             if(MPUxxx_OK != x){                   \
                                 LOG_ERROR("[%s] is return error",#x); \
                                 goto RETURN_ERROR;}                    \
                           }while (0)
//********************************* defFunc **********************************//
//----------------------------------------------------------------------------//
//********************************** define **********************************//
#undef LOG_TAG
#define LOG_TAG "mpu_handler"

#define MPUxxx_HANDLER_DEBUG
#ifdef MPUxxx_HANDLER_DEBUG
#define LOG_DEBUG(x,...)     log_d(x,##__VA_ARGS__)
#define LOG_ERROR(x,...)     log_e(x,##__VA_ARGS__)
#else
#define LOG_DEBUG(x,...)     ((void)(0))
#define LOG_ERROR(x,...)     ((void)(0))
#endif

#define HANDLER_UNINITIALIZED 0
#define HANDLER_INITIALIZED   1
//********************************** define **********************************//
//----------------------------------------------------------------------------//
//********************************* Variables ********************************//
static uint8_t mpuxxx_handler_init_flag = HANDLER_UNINITIALIZED;
bsp_mpuxxx_handler_t handler_instance = {0};
//********************************* Variables ********************************//
//----------------------------------------------------------------------------//
//********************************* Functions ********************************//
extern void (*pf_pin_interrupt_callback)(void*, void*);
extern void (*pf_DMA_interrupt_callback)(void*, void*);

//把参数传入的函数注册进pf_pin_interrupt_callback
void register_callback_pin(void (*callback)(void*,void*))
{
    pf_pin_interrupt_callback = callback;
}
//把参数传入的函数注册进pf_DMA_interrupt_callback
void register_callback_dma(void (*callback)(void*,void*))
{
    pf_DMA_interrupt_callback = callback;
}

mpuxxx_status_t mpuxxx_handler_init(bsp_mpuxxx_handler_t* pHandle)
{
    mpuxxx_status_t ret = MPUxxx_OK;
    NULL_CHECK(pHandle);
    if (mpuxxx_handler_init_flag == HANDLER_INITIALIZED)
    {
        LOG_ERROR("mpuxxx not need init");
        return MPUxxx_ERRORRESOURCE;
    }
    //创建解包队列
    ret = pHandle->pInput_API->pOS->os_queue_create(
                              pHandle->Queue_item_size,
                               pHandle->Queue_length,
                               &pHandle->pUnpack_queue_handle);
    RETURN_CHECK(ret);
    NULL_CHECK(pHandle->pDriver);
    ret = bsp_mpuxxx_driver_inst(
                                     pHandle->pDriver,
                                     pHandle->pInput_API->pIIC_driver,
                                     pHandle->pInput_API->pYield,
                                     pHandle->pInput_API->pOS,
                                     pHandle->pInput_API->pDelay,
                                     pHandle->pInput_API->pTimebase,
                                     register_callback_pin,
                                     register_callback_dma,
                                     pHandle->Queue_handle,
                                     pHandle->semaphore_binary_handle,
                                     pHandle->notify_handle);
    RETURN_CHECK(ret);
    return ret;
NULL_ERROR:
    {
        LOG_ERROR("the driver is null");
        return MPUxxx_ERROR;
    }
RETURN_ERROR:
    {
        LOG_DEBUG("function is return a error");
        return ret;
    }
}

mpuxxx_status_t mpu_handler_inst(      bsp_mpuxxx_handler_t* pHandle,
                                 mpuxxx_handler_input_api_t* pInput_API)
{
    LOG_DEBUG("===mpu inst is start===");
    NULL_CHECK(pHandle);
    NULL_CHECK(pInput_API);
    NULL_CHECK(pInput_API->pOS);
    NULL_CHECK(pInput_API->pDelay);
    NULL_CHECK(pInput_API->pIIC_driver);
    NULL_CHECK(pInput_API->pTimebase);
    NULL_CHECK(pInput_API->pTimebase);

    pHandle->pInput_API = pInput_API;

    RETURN_CHECK(mpuxxx_handler_init(pHandle));
    mpuxxx_handler_init_flag = HANDLER_INITIALIZED;

    LOG_DEBUG("===mpu inst is end===");

    return MPUxxx_OK;

    NULL_ERROR:
        {
            LOG_ERROR("input api is null");
            return MPUxxx_ERROR;
        }
    RETURN_ERROR:
        {
            LOG_DEBUG("function is return a error");
            return MPUxxx_ERROR;
        }
}

void mpuxxx_handler_thread(void* argument)
{
    mpuxxx_status_t ret = MPUxxx_OK;
    uint8_t data = 0;
    LOG_DEBUG("mpuxxx_handler is start");
    NULL_CHECK(argument);
    mpuxxx_handler_input_api_t* pInputAPI = \
                                          (mpuxxx_handler_input_api_t*)argument;
    buffer_init(&mpuxxx_buf,10);
    bsp_mpuxxx_driver_t pMPU_driver = {0};
    handler_instance.pDriver = &pMPU_driver;
    handler_instance.Queue_handle = NULL;
    handler_instance.pUnpack_queue_handle = NULL;
    handler_instance.semaphore_binary_handle = NULL;
    handler_instance.Queue_item_size = 1;
    handler_instance.Queue_length = 20;
    ret = mpuxxx_handler_inst(&handler_instance,pInputAPI);
    if (MPUxxx_OK != ret)
    {
        LOG_ERROR("mpuxxx handler inst is ng:[%d]",ret);
    }
    for (;;)
    {
        if (1 == mpuxxx_flag_read())
        {
            ret = handler_instance.pInput_API->pOS->os_queue_put(
                                          handler_instance.pUnpack_queue_handle,
                                          &data,
                                          0);
            if (ret!=MPUxxx_OK)
            {
                LOG_ERROR("mpuxxx queue send is ng");
            }
            mpuxxx_flag_set(0);
        }
        vTaskDelay(1);
    }
NULL_ERROR:
    {
        LOG_ERROR("input api is null");
        vTaskDelete(NULL);
        return;
    }

}
//********************************* Functions ********************************//