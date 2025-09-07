#include "drv_usart.h"
#include "stdio.h"

void drv_usart6_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_14;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART6, &USART_InitStruct);
    USART_ClearFlag(USART6, USART_FLAG_TC);
    
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel=USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART6, ENABLE);
}

int fputc(int ch, FILE *f)
{
    USART_SendData(USART6, (uint8_t)ch);
    while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
    USART_ClearFlag(USART6, USART_FLAG_TC);
    return ch;
}    
