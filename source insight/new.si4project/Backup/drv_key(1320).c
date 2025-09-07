#include "drv_key.h"
#include "drv_systick.h"



extern uint8_t key1_flag;

/**
 * @brief  按键外部中断初始化函数
 * @param  无
 * @retval 无
 */

void drv_key_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		
		// 使能外设时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
		
		// 配置PC5, PC8, PC9为输入
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_;  // 上拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		// 配置外部中断映射
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);  // PC5 -> EXTI5
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);  // PC8 -> EXTI8
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);  // PC9 -> EXTI9
		
		// 配置EXTI5 (KEY3)
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // 下降沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		// 配置EXTI8 (KEY2)
		EXTI_InitStructure.EXTI_Line = EXTI_Line8;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // 下降沿触发
		EXTI_Init(&EXTI_InitStructure);
		
		// 配置EXTI9 (KEY1)
		EXTI_InitStructure.EXTI_Line = EXTI_Line9;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // 下降沿触发
		EXTI_Init(&EXTI_InitStructure);
		
		// 配置NVIC中断分组
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		// 配置EXTI9_5_IRQn中断
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;  // 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		  // 子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

/*
函数名：drv_get_key
函数功能：获取键值
函数参数：无
返回值：uint8_t类型
*/
uint8_t drv_get_key(void)
{
	uint8_t ret=0;
	if(0==GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9))
	{
		drv_systick_ms(20);
		if(0==GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9))
		{
			while(0==GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9));
			key1_flag=~key1_flag;
			ret = 1;
			return ret;
		}
	}
	
	
	return ret;
}
