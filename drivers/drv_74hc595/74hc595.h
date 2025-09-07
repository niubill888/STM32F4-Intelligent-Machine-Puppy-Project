#include <stm32f4xx.h>
#include "drv_sht20.h"

#ifndef __DRV_74HC595_H
#define __DRV_74HC595_H

// 全局存储要显示的数字
extern uint8_t disp_n1, disp_n2, disp_n3, disp_n4;
extern uint8_t key1_flag;//温湿度转换

extern const uint8_t bit_code[4];
extern const uint8_t data_code[17];


void drv_74hc595_init(void);
void drv_seg_display(uint8_t bit,uint8_t data);
void drv_sht20_update(sht20_val sht20_data);


#endif


