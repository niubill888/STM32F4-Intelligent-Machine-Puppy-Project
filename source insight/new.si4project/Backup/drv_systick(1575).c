#include "drv_systick.h"

static uint16_t fs_ms=0;
static uint16_t fs_us=0;
volatile uint32_t tick_cnt = 0;

void drv_systick_init(uint32_t systick)
{
	SysTick->CTRL &= ~(0x05);//关闭systick定时器
	fs_us=systick/8;   //8分频，1us;
	fs_ms=fs_us*1000;  //1ms
}

void drv_systick_ms(uint16_t ms)
{
	uint32_t temp=0;
		uint8_t systick_flg=0;
		while(systick_flg == 0)
		{
			if(ms > 798)
			{
				SysTick->LOAD = 798*fs_ms;//延时时间需要计总次数
				ms=ms-798;
			}
			else
			{
				SysTick->LOAD = ms*fs_ms;//延时时间需要计总次数
				systick_flg = 1;
			}
			SysTick->VAL = 0;//清空当前值寄存器
			SysTick->CTRL |= 0x01;//开启定时器
			do
			{
				temp = SysTick->CTRL;//获取控制状态寄存器的值
			}
			while(!(temp & (0x01<<16)));//判断定时时间是否到
			SysTick->CTRL &= ~0x01;//关闭定时器
		}
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
uint32_t GetTick(void) {
    return tick_cnt; // 返回当前毫秒数
}

