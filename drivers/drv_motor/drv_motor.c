#include "drv_motor.h"

void drv_motor_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}
void drv_motor_open(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
}

void drv_motor_close(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}

