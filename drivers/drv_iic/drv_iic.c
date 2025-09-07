#include "drv_iic.h"
#include "drv_systick.h"

// 引脚定义：PB6=SCL（时钟线），PB7=SDA（数据线）
// 宏说明：SCL_L/H控制SCL高低，SDA_L/H控制SDA高低，SDA_READ读SDA电平，ACK=0（应答），NOACK=1（非应答）


// 初始化IIC引脚：配置为开漏输出（兼容IIC总线特性）
void iic_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    // 输出模式
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;   // 开漏输出（支持线与）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // SCL和SDA引脚
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // 无上下拉（依赖外部上拉）
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}


// 配置SDA为输入模式（用于读取从机数据或应答）
void sda_in(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;     // 输入模式
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;        // 仅配置SDA
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}


// 配置SDA为输出模式（用于主机发送数据或控制信号）
void sda_out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    // 输出模式
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;   // 开漏输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;        // 仅配置SDA
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}


// 发送IIC起始信号：SCL高电平时，SDA从高→低跳变
void iic_start(void)
{
    SCL_L;
    sda_out();
    SDA_H;    
    SCL_H;
    delay_us(5);  // 满足时序最小间隔
    SDA_L;
    delay_us(5);
    SCL_L;
}


// 发送IIC停止信号：SCL高电平时，SDA从低→高跳变
void iic_stop(void)
{
    SCL_L;
    sda_out();
    SDA_L;    
    SCL_H;
    delay_us(5);
    SDA_H;
    delay_us(5);
}


// 发送应答信号（ACK）：SCL高时SDA保持低电平
void send_ACK(void)
{
    SCL_L;
    sda_out();
    SDA_L;
    SCL_H;
    delay_us(5);
    SCL_L;
}


// 发送非应答信号（NOACK）：SCL高时SDA保持高电平
void send_NOACK(void)
{
    SCL_L;
    sda_out();
    SDA_H;
    SCL_H;
    delay_us(5);
    SCL_L;
}


// 等待从机应答：超时未收到应答返回NOACK，否则返回ACK
int wait_ACK(void)
{
    uint16_t time_out=0;
    SCL_L;
    sda_in();
    SCL_H;
    // 等待SDA被从机拉低（应答）
    while(SDA_READ)
    {
        time_out++;
        if(time_out>300)  // 超时阈值（约300us）
        {
            iic_stop();
            return NOACK;
        }
    }
    SCL_L;
    return ACK;
}


// 发送1字节数据：高位先行，每bit在SCL高电平时被从机采样
void send_data(uint8_t data)
{
		uint8_t i;
    SCL_L;
    sda_out();
    for(i=0;i<8;i++)
    {
        // 输出当前最高位
        if(data & 0x80) 
            SDA_H;
        else 
            SDA_L;
        SCL_H;
        delay_us(1);
        SCL_L;
        delay_us(1);
        data <<= 1;  // 左移准备下一位
    }
}


// 读取1字节数据：高位先行，读完后根据ack_flag发送ACK/NOACK
uint8_t read_data(uint8_t ack_flag)
{
    uint8_t r_data=0, i;
    SCL_L;
    sda_in();
    for(i=0;i<8;i++)
    {   
        r_data <<= 1;  // 左移空出低位
        SCL_H;
        if(SDA_READ) 
            r_data |= 0x01;  // 读取当前bit
        delay_us(1);
        SCL_L;
        delay_us(1);
    }
    // 发送应答/非应答
    if(ack_flag == ACK) 
        send_ACK();
    else 
        send_NOACK();
    SCL_L;
    return r_data;
}