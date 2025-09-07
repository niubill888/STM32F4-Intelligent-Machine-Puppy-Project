#include "drv_beep.h"
/*
函数名：drv_beep_init
函数功能：对蜂鸣器引脚进行初始化
输入参数：无
输出参数：无
*/
//beep 对应引脚    pa15    高电平发声，低电平不发声
void drv_beep_init(void)
{

	RCC->AHB1ENR |= 0x01;//开启GPIOA的时钟
	GPIOA->MODER &= ~(0x03<<30);//清空GPIOA
	GPIOA->MODER |= 0X01<<30;//将GPIOA设置为输出模式
	GPIOA->ODR &= ~(0x01<<15);//PA15置低电平，蜂鸣器不发声
}

void drv_beep_open(void)
{/*
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	*/
	GPIOA->ODR |= (0x01<<15);//PA15置高电平，蜂鸣器发声
	//drv_systick_ms(5000);
	//GPIOA->ODR &= ~(0x01<<15);//PA15置低电平，蜂鸣器不发声
}
void drv_beep_close(void)
{/*
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	*/
	GPIOA->ODR &= ~(0x01<<15);//PA15置低电平，蜂鸣器不发声
}


