#include <stm32f4xx.h>

#ifndef __DRV_IIC_H_
#define __DRV_IIC_H_
#define SCL_H GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define SCL_L GPIO_ResetBits(GPIOB, GPIO_Pin_6)

#define SDA_H GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SDA_L GPIO_ResetBits(GPIOB, GPIO_Pin_7)

#define SDA_READ GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)

#define ACK 0
#define NOACK 1

void iic_init(void);
void sda_in(void);
void sda_out(void);
void iic_start(void);
void iic_stop(void);
void send_ACK(void);
void send_NOACK(void);
int wait_ACK(void);
void send_data(uint8_t data);
uint8_t read_data(uint8_t ack_flag);

#endif
