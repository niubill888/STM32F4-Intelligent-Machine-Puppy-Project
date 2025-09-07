#include "74hc595.h"
#include "drv_spi2.h"
#include "drv_systick.h"
#include <stdio.h>

// 全局存储要显示的数字
uint8_t disp_n1=0, disp_n2=0, disp_n3=0, disp_n4=0;

extern uint8_t key1_flag;//温湿度转换


const uint8_t data_code[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,
0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x80};
const uint8_t bit_code[4]={0x01,0x02,0x04,0x08};

void drv_74hc595_init(void)
{
	drv_spi2_init();
}

void drv_seg_display(uint8_t bit,uint8_t data)
{	
	drv_spi2_senddata(bit);
	drv_spi2_senddata(data);
	NSS_ENABLE;
	drv_systick_us(100);//为什么一定要延时？
	NSS_DISABLE;
}

void drv_sht20_update(sht20_val sht20_data)//更新温湿度
{
	uint8_t sht20_arr[10] = {0};
	uint32_t sht20_h_or_t=0;
	if(key1_flag)
	{
		sht20_h_or_t = (uint32_t)(sht20_data.hum * 100);
	}
	else
	{
		sht20_h_or_t = (uint32_t)(sht20_data.temperature * 100);
	}
	sprintf((char*)sht20_arr, "%04u", sht20_h_or_t);
		
		// 更新全局显示变量
		disp_n1 = sht20_arr[0] - '0';
		disp_n2 = sht20_arr[1] - '0';
		disp_n3 = sht20_arr[2] - '0';
		disp_n4 = sht20_arr[3] - '0';
	
	printf("%d+++++++\r\n",disp_n1);
}
