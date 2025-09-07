#include "drv_timer.h"

void drv_timer_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    //TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period=10-1;
    TIM_TimeBaseInitStruct.TIM_Prescaler=8400-1;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel=TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM6, ENABLE);
}

void drv_timer6_init(void)//???2ms????
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    //TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period=50-1;
    TIM_TimeBaseInitStruct.TIM_Prescaler=8400-1;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel=TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM6, ENABLE);
}

void drv_timer7_init(void)//?????5s??????
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period=5000-1;
    TIM_TimeBaseInitStruct.TIM_Prescaler=8400-1;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel=TIM7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM7, ENABLE);
}
