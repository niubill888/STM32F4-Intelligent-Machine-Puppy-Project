#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>  // 引入标准布尔类型定义
#include "drv_led.h"
#include "drv_beep.h"
#include "drv_fan.h"
#include "drv_motor.h"
#include "drv_systick.h"
#include "FreeRTOS.h"
#include "semphr.h"




#ifndef __DRV_ESP12F_H_
#define __DRV_ESP12F_H_

extern uint8_t log;
// 声明全局信号量句柄（供其他文件使用）
extern SemaphoreHandle_t esp_rx_semaphore;

extern uint8_t key1_flag;
// 定义ESP状态枚举（增加SUCCESS状态）
typedef enum {
    ESP_STATE_INIT,
    ESP_STATE_WIFI_CONNECT,
    ESP_STATE_TCP_CONNECT,
    ESP_STATE_DATA_SEND,
    ESP_STATE_SUCCESS,    // 添加成功状态
    ESP_STATE_ERROR,
    ESP_STATE_TIMEOUT     // 添加超时状态
} ESP_STATE;

#define ESP_RX_BUF_SIZE 256
extern char esp_rx_buf[ESP_RX_BUF_SIZE];
extern uint8_t esp_rx_idx;
extern ESP_STATE current_state;
extern uint8_t last_rx_time;
// 全局变量：记录当前指令索引和重试次数
extern uint8_t cmd_index;
extern uint8_t retry_count;
extern uint32_t cmd_start_time;
extern bool led_status;  // 记录LED状态
extern bool server_disconnected;  // 服务器断开标志

// 声明数组（不定义，仅告诉编译器“这些数组在其他地方存在”）
extern const char* cmds[];
extern const char* expect_responses[];
extern const uint32_t timeouts[];



void send_next_cmd(void);
void parse_esp_response(void);






#endif



