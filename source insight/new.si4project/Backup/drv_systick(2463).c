#include "drv_systick.h"

uint16 fs_ms;
uint16 fs_us=1000*fs_ms;

void drv_systick_init(uint32_t )
{
	SysTick->CTRL &= ~(0x05);//关闭systick定时器
	SysTick->LOAD 
}

void drv_systick_ms(uint16 ms)
{

}

void drv_systick_us(uint16 us)
{

}
