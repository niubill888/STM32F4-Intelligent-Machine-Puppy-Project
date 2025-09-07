#include <stm32f4xx.h>
#include "drv_systick.h"
#include "drv_iic.h"

#ifndef __DRV_SHT20_H_
#define __DRV_SHT20_H_

typedef struct
{
    float hum;
    float temperature;
}sht20_val;

extern sht20_val sht20_data;  // 声明全局变量


void sht20_init(void);
uint8_t sht20_reset(void);
uint16_t sht20_read(uint8_t cmd);
void sht20_getdata(void);






#endif
