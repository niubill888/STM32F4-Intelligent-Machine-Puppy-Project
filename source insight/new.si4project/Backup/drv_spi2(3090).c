#include "drv_spi2.h"



void drv_spi2_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

    // NSS引脚配置
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_CPOL=SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA=SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_Mode=SPI_Mode_Master;
    SPI_InitStruct.SPI_CRCPolynomial=7;
    SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
    SPI_InitStruct.SPI_DataSize=SPI_DataSize_8b;
    SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_NSS=SPI_NSS_Soft;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    SPI_Init(SPI2, &SPI_InitStruct);
    SPI_Cmd(SPI2, ENABLE);
    NSS_DISABLE;
}

// 定义超时次数（根据SPI时钟频率调整，确保能发送完成）
#define SPI_TX_TIMEOUT  1000

uint8_t drv_spi2_senddata(uint8_t data)
{
    uint32_t timeout = SPI_TX_TIMEOUT;
    // 等待发送缓冲区为空，带超时
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        timeout--;
        if(timeout == 0)
        {
            // 超时处理（可选：记录错误、复位SPI等）
            return 1; // 返回错误码
        }
    }
    // 发送数据
    SPI_I2S_SendData(SPI2, (uint16_t)data);
    return 0; // 发送成功
}