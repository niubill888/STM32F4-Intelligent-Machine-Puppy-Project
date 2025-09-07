#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>


#ifndef __DRV_ESP12F_H_
#define __DRV_ESP12F_H_

typedef enum {
    ESP_STATE_INIT,         // 初始化阶段（AT/RESTORE/CWMODE/CIPMUX）
    ESP_STATE_READY,        // ESP就绪（AT响应OK）
    ESP_STATE_WIFI_CONNECT, // WiFi连接中（CWJAP_DEF）
    ESP_STATE_TCP_CONNECT,  // TCP连接中（CIPSTART）
    ESP_STATE_DATA_SEND,    // 数据发送阶段（CIPSEND/led_open）
    ESP_STATE_ERROR         // 错误状态
} ESP_State;

#define ESP_RX_BUF_SIZE 256

// 声明全局变量（不分配内存，仅告诉编译器“变量存在”）
extern char esp_rx_buf[ESP_RX_BUF_SIZE];          
extern uint8_t esp_rx_idx;         
extern uint32_t last_rx_time;      
extern ESP_State current_state;    

void sendto_esp_12f(void);
void parse_esp_response(void);






#endif



