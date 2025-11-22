//
// Created by capting on 2025/11/22.
//

#include "DWT_delay.h"
#include "stm32f411xe.h"

void DWT_Delay_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // 启用DWT跟踪
    DWT->CYCCNT = 0x00000000;                        // 清零计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;             // 启用CYCCNT计数
}

void DWT_Delay_us(uint32_t us)
{
    uint32_t start_cnt = DWT->CYCCNT;
    uint32_t target_cnt = start_cnt + (us * CPU_FREQ_MHZ);
    if (target_cnt < start_cnt)
    {
        while (DWT->CYCCNT >= start_cnt && DWT->CYCCNT < target_cnt);
    }
    else
    {
        while (DWT->CYCCNT < target_cnt);
    }
}

void DWT_Delay_ms(uint32_t ms)
{
    DWT_Delay_us(ms*1000UL);
}
