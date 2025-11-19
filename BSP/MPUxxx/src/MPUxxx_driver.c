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
#define DEBUG_LOG(x)  log_d(x)
#define NULL_CHECK(x,tag) do{\
    if(NULL == x){\
        log_e("discover null prt");\
        goto tag\
    }}while (0)
#endif//end of DEBUG

//******************************** Macros ***********************************//
//---------------------------------------------------------------------------//