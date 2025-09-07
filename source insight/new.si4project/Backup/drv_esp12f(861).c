#include "drv_esp12f.h"


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

void parse_esp_response(void) {
    esp_rx_buf[esp_rx_idx] = '\0'; // 补全字符串

	
	// 根据当前指令索引自动匹配状态（核心改进）
	if (cmd_index >= 0 && cmd_index <= 3) {
		current_state = ESP_STATE_INIT;
	} else if (cmd_index == 4) {
		current_state = ESP_STATE_WIFI_CONNECT;
	} else if (cmd_index == 5) {
		current_state = ESP_STATE_TCP_CONNECT;
	} else if (cmd_index >= 6 && cmd_index <= 7) {
		current_state = ESP_STATE_DATA_SEND;
	}

    // 根据当前状态解析响应
    switch (current_state) {
        case ESP_STATE_INIT:
        case ESP_STATE_WIFI_CONNECT:
        case ESP_STATE_TCP_CONNECT:
        case ESP_STATE_DATA_SEND:
            // 检查期望响应
            if (strstr(esp_rx_buf, expect_responses[cmd_index]) != NULL) {
                printf("Command [%d] success!\r\n", cmd_index);
                
                // 特殊处理：当发送"led_open"且收到"SEND OK"时，打开LED
                if (cmd_index == 7 && strstr(esp_rx_buf, "SEND OK") != NULL) {
                    drv_led_open();  // 打开LED
                    led_status = true;
                    printf("LED opened successfully!\r\n");
                }
                
                cmd_index++;     // 切换到下一条指令
                retry_count = 0; // 重置重试计数
                send_next_cmd(); // 发送下一条指令
            } 
            // 检查错误响应
            else if (strstr(esp_rx_buf, "ERROR") != NULL || 
                     strstr(esp_rx_buf, "FAIL") != NULL ||
                     strstr(esp_rx_buf, "CLOSED") != NULL) {
                current_state = ESP_STATE_ERROR;
                printf("Command [%d] failed! Error detected.\r\n", cmd_index);
            }
            break;

        case ESP_STATE_ERROR:
            printf("ESP is in ERROR state!\r\n");
            break;

        case ESP_STATE_SUCCESS:
            printf("ESP is in SUCCESS state!\r\n");
            break;

        default:
            break;
    }

    // 清空缓冲区
    memset(esp_rx_buf, 0, ESP_RX_BUF_SIZE);
    esp_rx_idx = 0;
}

// 发送下一条指令
void send_next_cmd(void) {
    if (cmd_index >= sizeof(cmds) / sizeof(cmds[0])) {
        current_state = ESP_STATE_SUCCESS;
        printf("All commands executed successfully!\r\n");
        return;
    }
    
    //printf("Sending command [%d]: %s", cmd_index, cmds[cmd_index]);
    usart_sendstr(USART3, cmds[cmd_index], strlen(cmds[cmd_index]));
    cmd_start_time = GetTick();  // 记录发送时间
    retry_count++;
}

