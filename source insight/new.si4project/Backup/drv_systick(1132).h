#include <stm32f4xx.h>

#ifndef __DRV_SYSTICK_H
#define __DRV_SYSTICK_H

void drv_systick_init(uint32_t systick);
void drv_systick_ms(uint16_t ms);
void drv_systick_us(uint16_t us);


#endif
