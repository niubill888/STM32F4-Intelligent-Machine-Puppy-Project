#include "drv_sht20.h"

//#include "drv_usart.h"

sht20_val sht20_data;
void sht20_init(void)
{
	iic_init();
}
uint8_t sht20_reset(void)
{
	iic_start();
	send_data(0x80);
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;
	}
	send_data(0xfe);
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;
	}
	iic_stop();
	return ACK;
}
uint16_t sht20_read(uint8_t cmd)
{
	uint16_t r_data=0;
	uint8_t temp[2];
	iic_start();
	send_data(0x80);//写地址
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;
	}
	
	send_data(cmd);
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;
	}
	
	iic_start();
	send_data(0x81);//读地址
	if(wait_ACK() != ACK)
	{
		iic_stop();
		return NOACK;
	}
	drv_systick_ms(120);
	temp[0] = read_data(ACK);
	temp[1] = read_data(NOACK);
	iic_stop();
	r_data =(temp[0]<<8|temp[1]);
	return r_data;
}
void sht20_getdata(void)
{
	uint16_t tmp=0;
	tmp = sht20_read(0xe5);//选择湿度
	sht20_data.hum	= -6+125*((float)tmp/65536);
	printf("hum=%.2f\r\n",sht20_data.hum);

	uint16_t temperature_raw = sht20_read(0xe3);
	sht20_data.temperature = -46.85 + 175.72 * ((float)temperature_raw / 65536);
	printf("Temperature: %.2f\r\n", sht20_data.temperature);

}
