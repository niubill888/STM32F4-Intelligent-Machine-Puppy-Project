
#include "drv_led.h"
#include "drv_systick.h"

//点亮扩展板LED1/LED2/LED3,给pc4/pc3/pc13高电平导通
void drv_led_init(void)
{
	RCC->AHB1ENR |= 0x04;//使能GPIOC时钟
	GPIOC->MODER &= ~(0x03<<8);//清空GPIOC     PC4
	GPIOC->MODER |= (0x01<<8);//设置PC4为推挽输出
	GPIOC->ODR |=0x01<<4;//设置输出引脚PC4为高电平->灯亮


	RCC->AHB1ENR |= 0x03;
	GPIOC->MODER &= ~(0x03<<6);
	GPIOC->MODER |= (0x01<<6);
	GPIOC->ODR |=0x01<<3;

	RCC->AHB1ENR |= 0x13;
	GPIOC->MODER &= ~(0x03<<26);
	GPIOC->MODER |= (0x01<<26);
	GPIOC->ODR |=0x01<<13;

}

void drv_led_open(void)
{
	GPIOC->ODR |=0x01<<4;
	GPIOC->ODR |=0x01<<3;
	GPIOC->ODR |=0x01<<13;

}

void drv_led_close(void)
{
	GPIOC->ODR &=~(0x01<<4);
	GPIOC->ODR &=~(0x01<<3);
	GPIOC->ODR &=~(0x01<<13);
}

void drv_led_flow(void)
{
	GPIOC->ODR |=0x01<<4;
	drv_systick_ms(500);
	drv_systick_ms(500);
	GPIOC->ODR |=0x01<<3;
	drv_systick_ms(500);
	drv_systick_ms(500);
	GPIOC->ODR |=0x01<<13;
	drv_systick_ms(500);
	drv_systick_ms(500);
	GPIOC->ODR &=~(0x01<<4);
	drv_systick_ms(500);
	drv_systick_ms(500);
	GPIOC->ODR &=~(0x01<<3);
	drv_systick_ms(500);
	drv_systick_ms(500);
	GPIOC->ODR &=~(0x01<<13);
	drv_systick_ms(500);
	drv_systick_ms(500);

}

void delay_ms(unsigned int delay_time)
{

	volatile unsigned int i, j;  // 使用volatile防止优化
    for(i=0; i<delay_time; i++) {
        for(j=0; j<10000; j++);  // 嵌套循环增加延时
    }
}

