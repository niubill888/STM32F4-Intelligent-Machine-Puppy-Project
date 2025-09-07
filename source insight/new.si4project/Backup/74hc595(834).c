#include "74hc595.h"
#include "drv_spi2.h"
#include "drv_systick.h"
#include <stdio.h>

// 全局显示变量
uint8_t disp_n1=0, disp_n2=0, disp_n3=0, disp_n4=0;


extern uint8_t key2_flag; // 温湿度切换标志

// 数码管段码（0~F + 小数点）
const uint8_t data_code[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,
                             0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x80};
const uint8_t bit_code[4]={0x01,0x02,0x04,0x08}; // 位选码

void drv_74hc595_init(void)
{
    drv_spi2_init();
}

void drv_seg_display(uint8_t bit, uint8_t data)
{	
    if(1==drv_spi2_senddata(bit))printf("error!\r\n");  // 发送位选
    if(1==drv_spi2_senddata(data))printf("error!\r\n");  // 发送段码
    drv_systick_us(1); 
    NSS_ENABLE;              // 锁存数据
    NSS_DISABLE;             // 释放总线
    // 移除阻塞延时：drv_systick_us(100); 
}

void drv_sht20_update(sht20_val sht20_data)
{
    uint8_t sht20_arr[10] = {0};
    uint32_t val = 0;
    
    // 根据key2_flag切换温湿度
    if(key2_flag) {
        val = (uint32_t)(sht20_data.hum * 100);   // 湿度×100
    } else {
        val = (uint32_t)(sht20_data.temperature * 100); // 温度×100
    }
    sprintf((char*)sht20_arr, "%04u", val);       // 格式化为4位数字
    
    // 更新显示变量
    disp_n1 = sht20_arr[0] - '0';
    disp_n2 = sht20_arr[1] - '0';
    disp_n3 = sht20_arr[2] - '0';
    disp_n4 = sht20_arr[3] - '0';
}
