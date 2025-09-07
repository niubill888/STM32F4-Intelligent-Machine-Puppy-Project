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
		GPIO_InitTypeDef GPIO_InitStruct;
		EXTI_InitTypeDef EXTI_InitStruct;
		NVIC_InitTypeDef NVIC_InitStruct;
		
		// 1. 使能时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // GPIOC时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // SYSCFG时钟（F4必须）
		
		// 2. 配置GPIO（PC5、PC8、PC9）
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;			   // 输入模式
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			   // 上拉电阻（按键按下为低电平）
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		// 3. 配置SYSCFG外部中断映射（F4用SYSCFG，替代F1的GPIO_EXTILineConfig）
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);  // PC5 -> EXTI5
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);  // PC8 -> EXTI8
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);  // PC9 -> EXTI9
		
		// 4. 配置EXTI线（共用结构体，依次配置）
		// 配置EXTI5（按键3）
		EXTI_InitStruct.EXTI_Line = EXTI_Line5;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;		// 中断模式
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;	// 下降沿触发（按键按下）
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct);
		
		// 配置EXTI8（按键2）
		EXTI_InitStruct.EXTI_Line = EXTI_Line8;
		EXTI_Init(&EXTI_InitStruct);  // 复用配置
		
		// 配置EXTI9（按键1）
		EXTI_InitStruct.EXTI_Line = EXTI_Line9;
		EXTI_Init(&EXTI_InitStruct);  // 复用配置
		
		// 5. 配置NVIC中断优先级
		NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn; 		// 对应EXTI5-9中断通道
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;// 抢占优先级（可根据需求调整）
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;		 // 子优先级
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);

}
// 软件触发按键1中断（EXTI_Line9，对应PC9）
void software_trigger_key1(void) {
    // 1. 使能EXTI时钟（确保已初始化）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // 2. 软件触发EXTI_Line9中断
    EXTI_GenerateSWInterrupt(EXTI_Line9);
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
		//	key1_flag=~key1_flag;
			ret = 1;
			return ret;
		}
	}
	
	
	return ret;
}
