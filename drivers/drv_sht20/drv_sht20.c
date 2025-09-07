#include "FreeRTOS.h"  // FreeRTOS核心头文件（用于任务延时）
#include "drv_sht20.h" // SHT20温湿度传感器驱动头文件
#include "task.h"       // FreeRTOS任务相关头文件（vTaskDelay用）

//#include "drv_usart.h"  // 串口头文件（预留，原代码注释未启用）

// 全局结构体实例：存储SHT20采集到的温湿度数据
sht20_val sht20_data;


/**
 * @brief SHT20传感器初始化
 * @note  初始化IIC总线（SHT20基于IIC通信），为后续传感器操作做准备
 * @param 无
 * @return 无
 */
void sht20_init(void)
{
	iic_init();  // 调用IIC初始化函数，配置IIC引脚和时序
}


/**
 * @brief 复位SHT20传感器
 * @note  通过IIC发送复位命令（0xFE），恢复传感器默认状态
 * @return uint8_t：ACK（0，复位成功）/ NOACK（非0，复位失败）
 */
uint8_t sht20_reset(void)
{
	iic_start();          // IIC总线起始信号
	send_data(0x80);      // 发送SHT20从机写地址（固定0x80）
	if(wait_ACK() != ACK) // 等待传感器ACK响应
	{
		iic_stop();       // 无ACK，发送停止信号
		return NOACK;     // 返回失败
	}
	send_data(0xfe);      // 发送复位命令（SHT20复位命令固定为0xFE）
	if(wait_ACK() != ACK) // 等待命令ACK响应
	{
		iic_stop();       // 无ACK，发送停止信号
		return NOACK;     // 返回失败
	}
	iic_stop();           // 发送IIC停止信号
	return ACK;           // 复位成功，返回ACK
}


/**
 * @brief 读取SHT20指定命令的原始数据
 * @note  发送测量命令后，等待传感器转换完成，读取16位原始数据
 * @param cmd：测量命令（0xE3=温度测量，0xE5=湿度测量）
 * @return uint16_t：16位原始测量数据（失败返回NOACK）
 */
uint16_t sht20_read(uint8_t cmd)
{
	uint16_t r_data=0;    // 存储读取的原始数据
	uint8_t temp[2];      // 临时数组，存两个字节的原始数据
	
	iic_start();          // IIC起始信号
	send_data(0x80);      // 发送从机写地址，进入写模式
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;     // 地址无响应，返回失败
	}
	
	send_data(cmd);       // 发送测量命令（温度/湿度）
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;     // 命令无响应，返回失败
	}
	
	iic_start();          // 重新发送起始信号，切换读写模式
	send_data(0x81);      // 发送从机读地址，进入读模式
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;     // 读地址无响应，返回失败
	}
	
	vTaskDelay(120 / portTICK_PERIOD_MS); // 等待测量完成（SHT20转换需约100-120ms）
	temp[0] = read_data(ACK);  // 读高8位，发送ACK继续读
	temp[1] = read_data(NOACK); // 读低8位，发送NOACK结束读
	iic_stop();                // IIC停止信号
	
	r_data = (temp[0]<<8 | temp[1]); // 拼接高低8位，得到16位原始数据
	return r_data;
}


/**
 * @brief 获取SHT20温湿度数据并转换为实际值
 * @note  调用sht20_read读取原始数据，用SHT20手册公式计算实际温湿度
 * @param 无
 * @return 无（结果存入全局变量sht20_data）
 */
void sht20_getdata(void)
{
	uint16_t tmp=0;
	// 读取湿度原始数据（命令0xE5），计算实际湿度（公式来自SHT20 datasheet）
	tmp = sht20_read(0xe5);
	sht20_data.hum = -6 + 125 * ((float)tmp / 65536); // 湿度范围0-100%RH
	//printf("hum=%.2f\r\n",sht20_data.hum);  // 预留串口打印（原代码注释）

	// 读取温度原始数据（命令0xE3），计算实际温度（公式来自SHT20 datasheet）
	uint16_t temperature_raw = sht20_read(0xe3);
	sht20_data.temperature = -46.85 + 175.72 * ((float)temperature_raw / 65536); // 温度范围-40~125℃
	//printf("Temperature: %.2f\r\n", sht20_data.temperature);  // 预留串口打印（原代码注释）
}