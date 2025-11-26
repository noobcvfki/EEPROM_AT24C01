//
// Created by capting on 2025/11/26.
//

#ifndef USER_MPUXXX_MPUXXX_HANDLER_H
#define USER_MPUXXX_MPUXXX_HANDLER_H
//********************************* Includes *********************************//
#include "MPUxxx_driver.h"
#include "FreeRTOS.h"
#include "task.h"
//********************************* Includes *********************************//
//----------------------------------------------------------------------------//
//********************************** enum ************************************//

//********************************** enum ************************************//
//----------------------------------------------------------------------------//
//********************************* Typedef **********************************//
typedef struct
{
    iic_driver_interface_t* pIIC_driver;
    timebase_interface_t*   pTimebase;
    delay_interface_t*      pDelay;
    os_interface_t*         pOS;
    yield_interface_t*      pYield;
}mpuxxx_handler_input_api_t;


typedef struct
{
    mpuxxx_handler_input_api_t* pInput_API;
    bsp_mpuxxx_driver_t*        pDriver;
    void*                       Queue_handle;
    void*                       pUnpack_queue_handle;
    uint32_t                    Queue_item_size;
    uint32_t                    Queue_length;

    void*                       semaphore_binary_handle;
    void*                       notify_handle;
}bsp_mpuxxx_handler_t;
//********************************* Typedef **********************************//
//----------------------------------------------------------------------------//
//******************************** Functions *********************************//
mpuxxx_status_t mpuxxx_handler_inst(
    bsp_mpuxxx_handler_t *pHandler,
    mpuxxx_handler_input_api_t *pInput_API
                                           );

void mpuxxx_handler_thread(void* argument);
#endif //USER_MPUXXX_MPUXXX_HANDLER_H