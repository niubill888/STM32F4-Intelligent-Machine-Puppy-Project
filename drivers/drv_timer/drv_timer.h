#ifndef __DRV_TIMER_H__
#define __DRV_TIMER_H__
#include <stm32f4xx.h>
void drv_timer6_init(void);
void drv_timer7_init(void);
void drv_timer3_init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif //__DRV_TIMER_H__

