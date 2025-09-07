#include "drv_led.h"
#include "drv_beep.h"
#include "drv_systick.h"
#include "drv_key.h"
#include "drv_exti.h"
#include "drv_fan.h"
#include "drv_motor.h"
#include "drv_usart.h"
#include "drv_timer.h"
#include "drv_esp12f.h"

uint8_t key1_flag=0;
uint8_t key2_flag=0;
uint8_t key3_flag=0;

char esp_rx_buf[ESP_RX_BUF_SIZE];      // 定义：实际分配内存
uint8_t esp_rx_idx = 0;                // 定义
uint32_t last_rx_time = 0;             // 定义
ESP_State current_state = ESP_STATE_INIT; // 定义


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	drv_systick_init(168);
	drv_usart6_init();
	drv_usart3_init();
	
	sendto_esp_12f();
	while(1)
	{ 
		 
		// 错误状态处理：重新初始化
		if (current_state == ESP_STATE_ERROR) {
			printf("检测到错误，重新初始化 ESP...\r\n");
			sendto_esp_12f();
		}		 
		drv_systick_ms(10); // 主循环延时，降低 CPU 负载
	}

}

