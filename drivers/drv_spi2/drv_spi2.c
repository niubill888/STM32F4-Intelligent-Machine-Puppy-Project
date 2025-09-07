#include "drv_spi2.h"

/**
 * 初始化SPI2接口（主机模式）
 * 引脚分配：
 * - PB13: SPI2_SCK（时钟线）
 * - PB14: SPI2_MISO（主机输入，从机输出）
 * - PB15: SPI2_MOSI（主机输出，从机输入）
 * - PB12: 软件片选NSS（输出模式，高电平无效）
 */
void drv_spi2_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    
    // 配置SPI2相关GPIO（SCK/MISO/MOSI）为复用功能
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;         // 复用功能模式（连接SPI2外设）
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;       // 推挽输出（SPI信号需要强驱动）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  // SCK/MISO/MOSI引脚
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;     // 无上下拉（SPI从机会控制电平）
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    // 50MHz速率（满足SPI通信需求）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
    GPIO_Init(GPIOB, &GPIO_InitStruct);               // 应用GPIO配置

    // 配置GPIO复用映射到SPI2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);  // PB13 → SPI2_SCK
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);  // PB14 → SPI2_MISO
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);  // PB15 → SPI2_MOSI

    // 配置PB12为软件片选NSS（输出模式）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;        // 推挽输出模式（软件控制片选）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;           // NSS片选引脚
    GPIO_Init(GPIOB, &GPIO_InitStruct);               // 应用配置

    // 配置SPI2参数（主机模式）
    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;           // 时钟极性：空闲时低电平
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;         // 时钟相位：第1个边沿采样
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;        // 主机模式
    SPI_InitStruct.SPI_CRCPolynomial = 7;             // CRC多项式（未使用可默认7）
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 波特率分频：APB1时钟/2
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;    // 数据宽度：8位
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 全双工模式
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;   // 高位先行（SPI默认协议）
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;            // 软件控制片选（不用硬件NSS）
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  // 使能SPI2时钟
    SPI_Init(SPI2, &SPI_InitStruct);                  // 应用SPI配置
    SPI_Cmd(SPI2, ENABLE);                            // 使能SPI2外设
    NSS_DISABLE;                                      // 初始片选无效（高电平，宏定义在头文件）
}

/**
 * 通过SPI2发送一个字节数据
 * @param data：待发送的8位数据
 */
void drv_spi2_senddata(uint8_t data)
{
    // 等待发送缓冲区为空（TXE标志置位）
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);	
    // 发送数据（强制转换为16位，兼容函数参数）
    SPI_I2S_SendData(SPI2, (uint16_t)data);
}