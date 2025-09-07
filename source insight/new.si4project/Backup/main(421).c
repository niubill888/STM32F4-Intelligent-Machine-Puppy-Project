#include "drv_led.h"
#include "drv_beep.h"
#include "drv_systick.h"
#include "drv_key.h"
#include "drv_exti.h"
#include "drv_fan.h"
#include "drv_motor.h"
#include "drv_usart.h"
#include "drv_timer.h"

uint8_t key1_flag=0;
uint8_t key2_flag=0;
uint8_t key3_flag=0;


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	drv_led_init();
	drv_led_close();
	drv_beep_init();
	drv_systick_init(168);
	drv_key_init();
	//drv_fan_init();
	//drv_motor_init();
	//drv_exit9_init();
	//drv_exit8_init();
	//drv_exit5_init();
	drv_usart6_init();
	drv_usart3_init();
	char cmd[] = "AT+CWMODE=1\r\n";  
	printf("发送指令: AT+CWMODE=1\r\n");
	printf("发送指令\r\n");
	usart_sendstr(USART3, cmd, strlen(cmd));  // strlen 返回 13，正好包含 \r\n  
	//drv_timer_init();
	drv_systick_ms(500);
	while(1)
	{ /* 
		if(key1_flag)
		{
			drv_beep_open();
		}
		else
		{
			drv_beep_close();
		}
		if(key2_flag)
		{
			drv_fan_open();
		}
		else
		{
			drv_fan_close();
		}
		if(key3_flag)
		{
			drv_motor_open();
			
		}
		else
		{
			drv_motor_close();
		}
		*/
	}

}

