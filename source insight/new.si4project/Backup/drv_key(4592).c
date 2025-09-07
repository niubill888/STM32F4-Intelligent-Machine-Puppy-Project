#include "drv_key.h"
#include "drv_systick.h"

//PF9引脚对应按键USER
/*
函数名：drv_key_init
函数功能：初始化按键
函数参数：无
返回值：无
*/
void drv_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能RCC时钟
	GPIO_Init(GPIOF, &GPIO_InitStruct);
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
			while(0==GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9))
			keyvalue=~keyvalue;
			ret = 1;
			return ret;
		}
	}
	
	
	return ret;
}
