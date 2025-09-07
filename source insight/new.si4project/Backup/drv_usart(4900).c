#include "drv_usart.h"


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
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel=USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART6, ENABLE);
}

void drv_usart3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStruct);
	
	USART_ClearFlag(USART3,USART_FLAG_TC);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART3, ENABLE);

	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	
}


void drv_usart2_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // PD端口时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // USART2时钟

    // 2. 配置GPIO（PD5=TX，PD6=RX，复用AF模式）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;     
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStruct);           

    // 3. 配置复用功能（USART2对应AF7）
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // PD5→USART2_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // PD6→USART2_RX

    // 4. 配置USART2参数（9600波特率，8N1）
    USART_InitStruct.USART_BaudRate = 9600;              
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_InitStruct.USART_Parity = USART_Parity_No;     
    USART_InitStruct.USART_StopBits = USART_StopBits_1;  
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
    USART_Init(USART2, &USART_InitStruct);              

    // 5. 使能接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);       

    // 6. 配置NVIC中断优先级
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;       
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        // 子优先级
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;          
    NVIC_Init(&NVIC_InitStruct);                         

    // 7. 使能USART2
    USART_Cmd(USART2, ENABLE);                           
}


void usart_sendstr(USART_TypeDef* USARTx,char *str,int size)
{
	uint8_t i=0;
	USART_ClearFlag(USARTx, USART_FLAG_TC);
	for(i=0;i<size;i++)
	{
		USART_SendData(USARTx, *str);
		while(RESET==USART_GetFlagStatus(USARTx, USART_FLAG_TC));
		USART_ClearFlag(USARTx, USART_FLAG_TC);
		str++;
	}
}
int fputc(int ch, FILE *f)
{
    USART_SendData(USART6, (uint8_t)ch);
    while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
    USART_ClearFlag(USART6, USART_FLAG_TC);
    return ch;
}    
