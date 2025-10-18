#ifndef __IIC_SOFTWARE_H
#define __IIC_SOFTWARE_H

#include "stm32f1xx_hal.h"

typedef struct
{
  GPIO_TypeDef * IIC_SDA_PORT;
  GPIO_TypeDef * IIC_SCL_PORT;
  uint16_t IIC_SDA_PIN;
  uint16_t IIC_SCL_PIN;
  //void (*CLK_ENABLE)(void);
}iic_bus_t;

//iic起始条件
void IICStart(iic_bus_t *bus);
//iic停止条件
void IICStop(iic_bus_t *bus);
//iic等待回应
unsigned char IICWaitAck(iic_bus_t *bus);
//iic发送回应位
void IICSendAck(iic_bus_t *bus);
//iic不发送回应位
void IICSendNotAck(iic_bus_t *bus);
//iic发送一个比特数据
void IICSendByte(iic_bus_t *bus, unsigned char cSendByte);
//iic接收一个比特数据
unsigned char IICReceiveByte(iic_bus_t *bus);


//iic初始化
void IICInit(iic_bus_t *bus);
//iic写一个比特数据
uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t data);
//iic写多位数据
uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[]);
//iic读一位数据
unsigned char IIC_Read_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg);
//iic读多位数据
uint8_t IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[]);


#endif /* __IIC_SOFTWARE_H */





