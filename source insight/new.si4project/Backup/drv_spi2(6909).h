#include <stm32f4xx.h>

#ifndef __DRV_SPI2_H
#define __DRV_SPI2_H

#define NSS_ENABLE GPIO_ResetBits(GPIOB, GPIO_Pin_12);
#define NSS_DISABLE GPIO_SetBits(GPIOB, GPIO_Pin_12);


void drv_spi2_init(void);
void drv_spi2_senddata(uint8_t);


#endif



