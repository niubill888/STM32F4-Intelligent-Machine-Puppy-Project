#include "drv_fan.h"

void drv_fan_init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_SetBits(GPIOB, GPIO_Pin_0);
}
void drv_fan_open(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void drv_fan_close(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}
