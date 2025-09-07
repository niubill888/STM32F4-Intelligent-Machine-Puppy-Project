#include "drv_usart.h"
#include "stdio.h"
/**************************************
函数名：drv_usart6_init
函数功能：串口6初始化
输入参数：无
输出参数：无
其它：//PG9->USART6_RX   
	  //PG14->USART6_TX
	  波特率115200
****************************************/
void drv_usart6_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//开启GPIOG的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);//开启串口6的时钟
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//配置引脚为复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_14;//串口用的引脚
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;//速率25Mhz
	GPIO_Init(GPIOG, &GPIO_InitStruct);//使用上面配置的参数初始化GPIOG

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);//指定引脚复用的具体功能为串口6
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;//开启接收和发送模式
	USART_InitStruct.USART_Parity = USART_Parity_No;//没有校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_Init(USART6, &USART_InitStruct);//使用上面配置的参数初始化串口6
	USART_ClearFlag(USART6, USART_FLAG_TC);//清空发送完成标志，否则会出现第一个字母丢失
	USART_Cmd(USART6, ENABLE);//使能串口6
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART6, ch);//通过串口6发送数据
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
	USART_ClearFlag(USART6, USART_FLAG_TC);//清空发送完成标志
	return ch;
}
