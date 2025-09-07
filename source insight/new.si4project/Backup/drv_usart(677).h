
#ifndef __DRV_USART_H__
#define __DRV_USART_H__
#include <stm32f4xx.h>
#include "stdio.h"

void drv_usart6_init(void);
void drv_usart3_init(void);
void usart_sendstr(USART_TypeDef* USARTx,char *str,int size);
int fputc(int ch, FILE *f);



#endif //__DRV_USART_H__


