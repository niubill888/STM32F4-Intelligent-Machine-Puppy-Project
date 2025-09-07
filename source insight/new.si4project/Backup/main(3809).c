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
	char cmd1[] = "AT+CWMODE=1\r\n";  
	char cmd2[] = "AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"\r\n";
	char cmd3[] = "AT+CWMODE=1\r\n";
	//printf("AT+CWMODE=1\r\n");
	while(1)
	{ 
		usart_sendstr(USART3, cmd1, strlen(cmd1));  // strlen 返回 13，正好包含 \r\n  
		usart_sendstr(USART3, cmd2, strlen(cmd2)); 
		usart_sendstr(USART3, cmd3, strlen(cmd3)); 
		//drv_timer_init();
		drv_systick_ms(500);
	}

}

