#include "drv_usart.h"


/**
 * 初始化USART6（GPIO_Pin_9/TX, GPIO_Pin_14/RX，GPIOG端口）
 * 波特率115200，8N1，使能接收和空闲中断
 */
void drv_usart6_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    
    // 使能GPIOG和USART6时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
    
    // 配置GPIO为复用功能（推挽输出，上拉，25MHz速度）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_14;  // 9-TX,14-RX
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    // 引脚复用映射到USART6
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

    // 配置USART参数（115200波特率，8位数据，1位停止位，无校验，收发模式）
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART6, &USART_InitStruct);
    USART_ClearFlag(USART6, USART_FLAG_TC);  // 清除发送完成标志
    
    // 使能接收非空和空闲中断
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);

    // 配置NVIC（抢占优先级2，子优先级0）
    NVIC_InitStruct.NVIC_IRQChannel=USART6_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 使能USART6
    USART_Cmd(USART6, ENABLE);
}

/**
 * 初始化USART3（GPIO_Pin_10/TX, GPIO_Pin_11/RX，GPIOB端口）
 * 波特率115200，8N1，使能接收和空闲中断
 */
void drv_usart3_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    // 使能GPIOB和USART3时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    // 配置GPIO为复用功能（推挽输出，上拉，25MHz速度）
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;  // 10-TX,11-RX
    GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 引脚复用映射到USART3
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    // 配置USART参数（115200波特率，8位数据，1位停止位，无校验，收发模式）
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate=115200;
    USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity=USART_Parity_No;
    USART_InitStruct.USART_StopBits=USART_StopBits_1;
    USART_InitStruct.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStruct);
    
    // 清除标志，使能接收和空闲中断
    USART_ClearFlag(USART3,USART_FLAG_TC);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    USART_Cmd(USART3, ENABLE);  // 使能USART3

    // 配置NVIC（抢占优先级6，子优先级0）
    NVIC_InitTypeDef  NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

QueueHandle_t xASRQueue=NULL;  // 语音指令队列句柄（用于任务间传递指令）

/**
 * 初始化USART2（GPIO_Pin_2/TX, GPIO_Pin_3/RX，GPIOA端口）
 * 波特率9600，8N1，使能接收中断，创建语音指令队列
 */
void drv_usart2_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1. 使能GPIOA和USART2时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 2. 配置GPIO为复用功能（推挽输出，上拉，50MHz速度）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;     
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;  // 2-TX,3-RX
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStruct);           

    // 3. 引脚复用映射到USART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // 4. 配置USART2参数（9600波特率，8位数据，1位停止位，无校验，收发模式）
    USART_InitStruct.USART_BaudRate = 9600;              
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_InitStruct.USART_Parity = USART_Parity_No;     
    USART_InitStruct.USART_StopBits = USART_StopBits_1;  
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
    USART_Init(USART2, &USART_InitStruct);              

    // 5. 使能接收非空中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);       

    // 6. 配置NVIC（抢占优先级6，子优先级0）
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;       
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;       
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;          
    NVIC_Init(&NVIC_InitStruct);                         

    // 7. 使能USART2
    USART_Cmd(USART2, ENABLE);         

    // 初始化语音指令队列（长度8，存储uint8_t类型）
    xASRQueue = xQueueCreate(8, sizeof(uint8_t)); 
    if (xASRQueue == NULL) {
        // 队列创建失败，死循环提示
        while (1); 
    }
}


/**
 * 串口发送字符串
 * @param USARTx：串口句柄（如USART6）
 * @param str：待发送字符串
 * @param size：发送长度
 */
void usart_sendstr(USART_TypeDef* USARTx,char *str,int size)
{
    uint8_t i=0;
    USART_ClearFlag(USARTx, USART_FLAG_TC);  // 清除发送完成标志
    for(i=0;i<size;i++)
    {
        USART_SendData(USARTx, *str);  // 发送单个字符
        while(RESET==USART_GetFlagStatus(USARTx, USART_FLAG_TC));  // 等待发送完成
        USART_ClearFlag(USARTx, USART_FLAG_TC);  // 清除标志
        str++;  // 指针偏移
    }
}

/**
 * 重定向fputc函数到USART6，支持printf输出
 */
int fputc(int ch, FILE *f)
{
    USART_SendData(USART6, (uint8_t)ch);  // 发送字符
    while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);  // 等待发送完成
    USART_ClearFlag(USART6, USART_FLAG_TC);  // 清除标志
    return ch;
}    