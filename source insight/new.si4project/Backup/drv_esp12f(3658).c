#include "drv_esp12f.h"

void parse_esp_response(void) {
    // 若缓冲区有数据，且 100ms 无新数据，判定为一帧结束
    if (esp_rx_idx > 0 && (GetTick() - last_rx_time) > 100) {
        esp_rx_buf[esp_rx_idx] = '\0'; // 补全字符串
        printf("ESP响应: %s\r\n", esp_rx_buf);

        // 根据当前状态解析响应
        switch (current_state) {
            // -------------------- 初始化阶段（AT/RESTORE/CWMODE/CIPMUX） --------------------
            case ESP_STATE_INIT:
                if (strstr(esp_rx_buf, "OK") != NULL) {
                    printf("初始化命令执行成功\r\n");
                } else if (strstr(esp_rx_buf, "ERROR") != NULL) {
                    current_state = ESP_STATE_ERROR;
                    printf("初始化失败！进入错误状态\r\n");
                }
                break;

            // -------------------- WiFi 连接阶段（CWJAP_DEF） --------------------
            case ESP_STATE_WIFI_CONNECT:
                if (strstr(esp_rx_buf, "WIFI CONNECTED") != NULL) {
                    current_state = ESP_STATE_TCP_CONNECT; // 切换到 TCP 连接状态
                    printf("WiFi连接成功！开始连接TCP服务器\r\n");
                } else if (strstr(esp_rx_buf, "FAIL") != NULL) {
                    current_state = ESP_STATE_ERROR;
                    printf("WiFi连接失败！进入错误状态\r\n");
                }
                break;

            // -------------------- TCP 连接阶段（CIPSTART） --------------------
            case ESP_STATE_TCP_CONNECT:
                if (strstr(esp_rx_buf, "CONNECT") != NULL) {
                    current_state = ESP_STATE_DATA_SEND; // 切换到数据发送状态
                    printf("TCP连接成功！准备发送数据\r\n");
                } else if (strstr(esp_rx_buf, "ERROR") != NULL) {
                    current_state = ESP_STATE_ERROR;
                    printf("TCP连接失败！进入错误状态\r\n");
                }
                break;

            // -------------------- 数据发送阶段（CIPSEND/led_open） --------------------
            case ESP_STATE_DATA_SEND:
                if (strstr(esp_rx_buf, "SEND OK") != NULL) {
                    printf("数据发送成功！\r\n");
                } else if (strstr(esp_rx_buf, "CLOSED") != NULL) {
                    current_state = ESP_STATE_INIT; // 连接关闭，回到初始化
                    printf("TCP连接已关闭！重新初始化\r\n");
                }
                break;

            // -------------------- 错误状态 --------------------
            case ESP_STATE_ERROR:
                printf("ESP处于错误状态！\r\n");
                break;

            default:
                break;
        }

        // 清空缓冲区，准备下一次接收
        memset(esp_rx_buf, 0, sizeof(esp_rx_buf));
        esp_rx_idx = 0;
    }
}
void sendto_esp_12f(void) {
    char cmd1[] = "AT+RESTORE\r\n"; 
    char cmd2[] = "AT\r\n"; 
    char cmd3[] = "AT+CWMODE=1\r\n"; 
    char cmd4[] = "AT+CIPMUX=0\r\n";
    char cmd5[] = "AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"\r\n"; 
    char cmd6[] = "AT+CIPSTART=\"TCP\",\"192.168.20.118\",7777\r\n";  
    char cmd7[] = "AT+CIPSEND=8\r\n";
    char cmd8[] = "led_open\r\n";

    // 1. AT+RESTORE（初始化阶段）
    current_state = ESP_STATE_INIT;
    usart_sendstr(USART3, cmd1, strlen(cmd1)); 
    drv_systick_ms(1000);

    // 2. AT（初始化阶段）
    current_state = ESP_STATE_INIT;
    usart_sendstr(USART3, cmd2, strlen(cmd2)); 
    drv_systick_ms(1000);

    // 3. AT+CWMODE=1（初始化阶段）
    current_state = ESP_STATE_INIT;
    usart_sendstr(USART3, cmd3, strlen(cmd3)); 
    drv_systick_ms(1000);

    // 4. AT+CIPMUX=0（初始化阶段）
    current_state = ESP_STATE_INIT;
    usart_sendstr(USART3, cmd4, strlen(cmd4));  
    drv_systick_ms(1000);

    // 5. AT+CWJAP_DEF（WiFi连接阶段）
    current_state = ESP_STATE_WIFI_CONNECT;
    usart_sendstr(USART3, cmd5, strlen(cmd5)); 
    drv_systick_ms(10000);

    // 6. AT+CIPSTART（TCP连接阶段）
    current_state = ESP_STATE_TCP_CONNECT;
    usart_sendstr(USART3, cmd6, strlen(cmd6)); 
    drv_systick_ms(10000);

    // 7. AT+CIPSEND + 数据（数据发送阶段）
    current_state = ESP_STATE_DATA_SEND;
    usart_sendstr(USART3, cmd7, strlen(cmd7)); 
    drv_systick_ms(5000);		
    usart_sendstr(USART3, cmd8, strlen(cmd8)); 
    drv_systick_ms(5000);
}
