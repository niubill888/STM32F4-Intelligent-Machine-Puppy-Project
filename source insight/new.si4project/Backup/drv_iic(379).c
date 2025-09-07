#include "drv_iic.h"
#include "drv_systick.h"

/*
函数名：iic_init()
函数参数：无
函数功能：初始化软件模拟iic的gpio引脚
函数返回值：无
其他：
*/
void iic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;//
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void sda_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void sda_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void iic_start(void)
{
	SCL_L;
	sda_out();
	SDA_H;	
	SCL_H;
	drv_systick_us(5);
	SDA_L;
	drv_systick_us(5);
	SCL_L;
}
void iic_stop(void)
{
	SCL_L;
	sda_out();
	SDA_L;	
	SCL_H;
	drv_systick_us(5);
	SDA_H;
	drv_systick_us(5);
}
void send_ACK(void)
{
	SCL_L;
	sda_out();
	SDA_L;
	SCL_H;
	drv_systick_us(5);
	SCL_L;
}
void send_NOACK(void)
{
	SCL_L;
	sda_out();
	SDA_H;
	SCL_H;
	drv_systick_us(5);
	SCL_L;
}
int wait_ACK(void)
{
	uint16_t time_out=0;
	SCL_L;
	sda_in();
	SCL_H;
	while(SDA_READ)
	{
		time_out++;
		if(time_out>300)
		{
			iic_stop();
			return NOACK;
		}
	}
	SCL_L;
	return ACK;
}
void send_data(uint8_t data)
{
	uint8_t i=0;
	SCL_L;
	sda_out();
	for(i;i<8;i++)
	{
		if(data & 0x80)
		{
			SDA_H;
		}
		else
		{
			SDA_L;
		}
		SCL_H;
		drv_systick_us(1);
		SCL_L;
		drv_systick_us(1);
		data<<=1;
	}

}
uint8_t read_data(uint8_t ack_flag)
{
	uint8_t r_data=0,i=0;
	SCL_L;
	sda_in();
	for(i;i<8;i++)
	{	
		r_data<<=1;
		SCL_H;
		if(SDA_READ)
		{
			r_data|=0x01;
		}
		drv_systick_us(1);
		SCL_L;
		drv_systick_us(1);
	}
	if(ack_flag==ACK)
	{
		send_ACK();
	}
	else
	{
		send_NOACK();
	}
	SCL_L;
	return r_data;
}