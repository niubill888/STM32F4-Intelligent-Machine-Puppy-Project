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
#include "drv_sht20.h"
#include "74hc595.h"


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
//	drv_led_init();
//	drv_beep_init();
//	drv_fan_init();
//	drv_motor_init();
	drv_key_init();
	drv_systick_init(168);
	drv_usart6_init();
		sht20_init();
	
	drv_74hc595_init();
	sht20_getdata();
	drv_sht20_update(sht20_data);
	drv_timer6_init();
	//drv_timer7_init();
//	drv_usart3_init();
	//drv_usart2_init();
//	current_state = ESP_STATE_INIT;
//	drv_timer_init();
//	drv_systick_ms(100);
//    send_next_cmd();
	while(1) {
//		esp_state_rp();
	printf("main====================\r\n");
		//drv_systick_ms(1000);

}
}
