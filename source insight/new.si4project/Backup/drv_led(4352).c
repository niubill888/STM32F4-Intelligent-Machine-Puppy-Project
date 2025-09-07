#include "drv_led.h"


//点亮扩展板LED1/LED2/LED3,给pc4/pc3/pc13高电平导通
void drv_led_init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct4;
	GPIO_InitStruct4.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruct4.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct4.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct4.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct4.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeDef GPIO_InitStruct3;
	GPIO_InitStruct3.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStruct3.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct3.GPIO_Speed=GPIO_Medium_Speed;
	GPIO_InitStruct3.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct3.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeDef GPIO_InitStruct13;
	GPIO_InitStruct13.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStruct13.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct13.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct13.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct13.GPIO_PuPd=GPIO_PuPd_UP;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_Init(GPIOC, &GPIO_InitStruct4);
	GPIO_Init(GPIOC, &GPIO_InitStruct3);
	GPIO_Init(GPIOC, &GPIO_InitStruct13);
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	GPIO_SetBits(GPIOC, GPIO_Pin_3);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
}

void drv_led_open(void)
{

	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	GPIO_SetBits(GPIOC, GPIO_Pin_3);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void drv_led_close(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_3);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void delay_ms(unsigned int delay_time)
{
	volatile unsigned int i, j;  // 使用volatile防止优化
    for(i=0; i<delay_time; i++) {
        for(j=0; j<10000; j++);  // 嵌套循环增加延时
    }
}

