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
	drv_usart6_init();
	drv_usart3_init();
	char cmd1[] = "AT+RESTORE\r\n"; 
	char cmd2[] = "AT\r\n"; 
	char cmd3[] = "AT+CWMODE=1\r\n"; 
	//char cmd4[] = "AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"\r\n";
	//char cmd5[] = "AT+CIPSTART=\"TCP\",\"192.168.91.24\",7777\r\n";
	char cmd6[] = "AT+CWJAP_DEF=\"llgdp\",\"12345678admin\"\r\n"; 
	char cmd7[] = "AT+CIPSTART=\"TCP\",\"192.168.240.244\",7777\r\n";  
	char cmd8[] = "AT+CIPSEND=5\r\n";
	//printf("AT+CWMODE=1\r\n");

		usart_sendstr(USART3, cmd3, strlen(cmd3)); 
		drv_systick_ms(1000);
		//usart_sendstr(USART3, cmd4, strlen(cmd4));
		//drv_systick_ms(500);
		//usart_sendstr(USART3, cmd5, strlen(cmd5));
		//drv_systick_ms(500);
		usart_sendstr(USART3, cmd6, strlen(cmd6));  // strlen 返回 13，正好包含 \r\n  
		drv_systick_ms(5000);
		usart_sendstr(USART3, cmd7, strlen(cmd7));  // strlen 返回 13，正好包含 \r\n  
		drv_systick_ms(7000);
		usart_sendstr(USART3, cmd8, strlen(cmd8));  // strlen 返回 13，正好包含 \r\n  
		drv_systick_ms(3000);
	while(1)
	{ 

	}

}

