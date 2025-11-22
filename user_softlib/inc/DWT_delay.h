//
// Created by capting on 2025/11/22.
//

#ifndef USER_MPUXXX_DWT_DELAY_H
#define USER_MPUXXX_DWT_DELAY_H

#include <stdint.h>

#define CPU_FREQ_MHZ     100UL   //CPU主频100MHz
#define CPU_FREQ_HZ      (CPU_FREQ_MHZ * 1000000UL)

void DWT_Delay_Init(void) ;

void DWT_Delay_us(uint32_t us);


void DWT_Delay_ms(uint32_t ms);

#endif //USER_MPUXXX_DWT_DELAY_H