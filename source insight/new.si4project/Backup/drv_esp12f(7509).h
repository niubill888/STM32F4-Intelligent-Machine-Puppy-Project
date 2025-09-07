#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>  // 引入标准布尔类型定义


#ifndef __DRV_ESP12F_H_
#define __DRV_ESP12F_H_

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
// 全局变量：记录当前指令索引和重试次数
extern uint8_t cmd_index;
extern uint8_t retry_count;
extern uint32_t cmd_start_time;
extern bool led_status = false;  // 记录LED状态

// 指令数组和对应的期望响应
const char* cmds[] = {
    "AT+RESTORE\r\n",
    "AT\r\n",
    "AT+CWMODE=1\r\n",
    "AT+CIPMUX=0\r\n",
    "AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"\r\n",
    "AT+CIPSTART=\"TCP\",\"192.168.20.118\",7777\r\n",
    "AT+CIPSEND=8\r\n",
    "led_open\r\n"
};

const char* expect_responses[] = {
    "OK",           // AT+RESTORE
    "OK",           // AT
    "OK",           // AT+CWMODE=1
    "OK",           // AT+CIPMUX=0
    "WIFI CONNECTED", // AT+CWJAP_DEF
    "CONNECT",      // AT+CIPSTART
    ">",            // AT+CIPSEND=8
    "SEND OK"       // led_open
};

// 超时时间（毫秒）
const uint32_t timeouts[] = {
    2000, 2000, 2000, 2000, 15000, 10000, 5000, 5000
};


void send_next_cmd(void);
void parse_esp_response(void);






#endif



