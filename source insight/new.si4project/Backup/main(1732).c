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
#include <stdbool.h>  // 引入标准布尔类型定义


uint8_t key1_flag=0;
uint8_t key2_flag=0;
uint8_t key3_flag=0;

char esp_rx_buf[ESP_RX_BUF_SIZE];      // 定义：实际分配内存
uint8_t esp_rx_idx=0;
ESP_STATE current_state=0;
uint8_t last_rx_time=0;
// 全局变量：记录当前指令索引和重试次数
uint8_t cmd_index = 0;
uint8_t retry_count = 0;
bool led_status = false;  // 记录LED状态


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	drv_led_init();
	drv_beep_init();
	drv_fan_init();
	drv_motor_init();
	drv_systick_init(168);
	drv_usart6_init();
	drv_usart3_init();
	//drv_usart2_init();
	current_state = ESP_STATE_INIT;
	drv_timer_init();
	drv_systick_ms(100);
    send_next_cmd();
	while(1) {
			
		   // 处理超时：100ms无新数据 → 认为一帧结束
		   /*if (esp_rx_idx > 0 && (GetTick() - last_rx_time) > 100) {
			   parse_esp_response(); // 超时后解析
		   }*/
		   // 处理超时
		   if (current_state != ESP_STATE_ERROR && current_state != ESP_STATE_SUCCESS) {
			   if (GetTick() - cmd_start_time > timeouts[cmd_index]) {
				   printf("Command [%d] timeout!+++++++++++++\r\n", cmd_index);
				   
				   if (retry_count < 3) { // 重试3次
					   printf("Retrying command [%d]...\r\n", cmd_index);
					   send_next_cmd();
				   } else {
					   current_state = ESP_STATE_TIMEOUT;
					   printf("Max retries reached. Entering ERROR state.\r\n");
				   }
			   }
		   }
	
		   // 错误状态处理：重置并重新开始
		   if (current_state == ESP_STATE_ERROR || current_state == ESP_STATE_TIMEOUT) {
			   printf("Resetting ESP connection...\r\n");
			   cmd_index = 0;	   // 重置指令索引
			   retry_count = 0;    // 重置重试计数
			   current_state = ESP_STATE_INIT;
			   led_status = false; // 重置LED状态
			   send_next_cmd();
		   }
	
		   drv_systick_ms(10); // 降低CPU负载
	   }

}

