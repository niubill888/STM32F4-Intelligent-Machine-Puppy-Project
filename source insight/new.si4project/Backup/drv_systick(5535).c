#include "drv_systick.h"

static uint16_t fs_ms=0;
static uint16_t fs_us=0;

void drv_systick_init(uint32_t systick)
{
	SysTick->CTRL &= ~(0x05);//关闭systick定时器
	fs_us=systick/8;   //8分频，1us;
	fs_ms=fs_us*1000;  //1ms
}

void drv_systick_ms(uint16_t ms)
{
	uint32_t temp=0;
	SysTick->LOAD = ms*fs_ms;  //延时时间需要计数次数
	SysTick->VAL=0;//清空寄存器当前值
	SysTick->CTRL |= 0x01;  //开启定时器
	do
	{
		temp=SysTick->CTRL;//获取当前寄存器的值
	}
	while(!(temp&(0x01<<16)));  //判定时间到了没有
	SysTick->CTRL &=~(0x01);  //关闭定时器
}

void drv_systick_us(uint16_t us)
{
	uint32_t temp=0;
	SysTick->LOAD = us*fs_us;  //延时时间需要计数次数
	SysTick->VAL=0;//清空寄存器当前值
	SysTick->CTRL |= 0x01;  //开启定时器
	do
	{
		temp=SysTick->CTRL;//获取当前寄存器的值
	}
	while(!(temp&(0x01<<16)));  //判定时间到了没有
	SysTick->CTRL &=~(0x01);  //关闭定时器
}
