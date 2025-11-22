//
// Created by capting on 2025/11/22.
//
//******************************** Includes *********************************//
#include "at24c02_driver.h"
#include "elog.h"
//******************************** Includes *********************************//
//---------------------------------------------------------------------------//
//********************************* define **********************************//
#ifdef LOG_TAG
    #undef LOG_TAG
    #define LOG_TAG "eeprom_driver"
#else
    #define LOG_TAG "eeprom_driver"
#endif

#define DEBUG

#ifdef DEBUG
    #define DEBUG_LOG(x,...)   log_d(x,##__VA_ARGS__)
    #define ERROR_LOG(x,...)   log_e(x,##__VA_ARGS__)
#else
#define DEBUG_LOG(x,...)    (void(0))
#define ERROR_LOG(x,...)    (void(0))
#endif

#define NULL_CHECK(x) do{                                     \
                            if(NULL == x){                    \
                                ERROR_LOG("check null ptr");  \
                                goto NULL_ERROR;              \
                            }                                 \
                      }while (0)
//********************************* define **********************************//
//---------------------------------------------------------------------------//
//******************************** Function *********************************//

static eeprom_status_t eeprom_init(bsp_eeprom_driver_t* p_eeprom);
static eeprom_status_t eeprom_readid(bsp_eeprom_driver_t* p_eeprom);
static eeprom_status_t eeprom_write(bsp_eeprom_driver_t* p_eeprom,
                             uint8_t* p_data,uint8_t len);
static eeprom_status_t eeprom_read(bsp_eeprom_driver_t* p_eeprom,
                                  uint8_t* p_data,uint8_t len);





#ifdef SOFTWARE_IIC
eeprom_status_t eeprom_inst(bsp_eeprom_driver_t* p_eeprom,
                                 uint8_t eeprom_7bit_addr,
                      eeprom_software_iic_driver_t* p_iic)
{
    DEBUG_LOG("=============eeprom inst start==========");
    eeprom_status_t ret = EEPROM_OK;
    NULL_CHECK(p_eeprom);
    NULL_CHECK(p_iic);
    NULL_CHECK(p_iic->iic_handle);
    NULL_CHECK(p_iic->pf_iic_init);
    NULL_CHECK(p_iic->pf_iic_deinit);
    NULL_CHECK(p_iic->pf_iic_mem_read);
    NULL_CHECK(p_iic->pf_iic_mem_write);
    NULL_CHECK(p_iic->critical_enable);
    NULL_CHECK(p_iic->critical_disable);

    p_eeprom->iic_read_addr = (eeprom_7bit_addr<<1)|1;
    p_eeprom->iic_write_addr = (eeprom_7bit_addr<<1)|0;
    p_eeprom->iic_handle = p_iic->iic_handle;
    p_eeprom->pf_eeprom_init    =eeprom_init;
    p_eeprom->pf_eeprom_readid =eeprom_readid;
    p_eeprom->pf_eeprom_write = eeprom_write;
    p_eeprom->pf_eeprom_read = eeprom_read;

    ret = eeprom_init(p_eeprom);
    if (EEPROM_OK!=ret)
    {
        ERROR_LOG("eeprom init is ng");
        return ret;
    }
    DEBUG_LOG("======eeprom inst is end=========");
    return ret;
NULL_ERROR:
    {
        ERROR_LOG("eeprom inst check null ptr");
        return EEPROM_ERROR;
    }
}
#else
eeprom_status_t eeprom_inst(bsp_eeprom_driver_t* p_eeprom,
                                 uint8_t eeprom_7bit_addr,
                      eeprom_hardware_iic_driver_t* p_iic);
#endif