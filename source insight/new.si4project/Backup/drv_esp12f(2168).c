#include "drv_esp12f.h"




uint32_t cmd_start_time = 0;

// 指令数组（删除最后一条"fan_open\r\n"）
const char* cmds[] = {
    "AT+RESTORE\r\n",
    "AT\r\n",
    "AT+CWMODE=1\r\n",
    "AT+CIPMUX=0\r\n",
    "AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"\r\n",
    "AT+CIPSTART=\"TCP\",\"192.168.20.118\",7777\r\n",
    "AT+CIPSEND=8\r\n"  // 仅保留发送数据前的指令（实际不发送固定内容，改为接收命令）
};

// 对应期望响应（删除最后一条"SEND OK"）
const char* expect_responses[] = {
    "OK",           // AT+RESTORE
    "OK",           // AT
    "OK",           // AT+CWMODE=1
    "OK",           // AT+CIPMUX=0
    "WIFI CONNECTED", // AT+CWJAP_DEF
    "CONNECT",      // AT+CIPSTART
    ">"             // AT+CIPSEND=8
};

// 超时时间（删除最后一条）
const uint32_t timeouts[] = {
    2000, 2000, 2000, 2000, 15000, 10000, 5000
};

// Parse commands from TCP server to control peripherals
void parse_tcp_command(const char* cmd) {
    // 解析 ESP8266 透传数据格式：提取 "[+IPD,...:]" 后的真实命令
    const char* real_cmd = cmd;
    char* colon = strchr(cmd, ':');  // 定位协议头中的冒号
    if (colon != NULL) {
        real_cmd = colon + 1;  // 跳过冒号，指向实际命令内容
    }

    if (strstr(real_cmd, "fan_open") != NULL) {
        drv_fan_open();
        printf("Fan turned on\r\n");
    } else if (strstr(real_cmd, "fan_close") != NULL) {
        drv_fan_close();  
        printf("Fan turned off\r\n");
    } else if (strstr(real_cmd, "led_on") != NULL) {
        drv_led_open();     
        printf("LED turned on\r\n");
    } else if (strstr(real_cmd, "beep_on") != NULL) {
        drv_beep_open();    
        printf("Buzzer turned on\r\n");
    } else if (strstr(real_cmd, "motor_start") != NULL) {
        drv_motor_open();
        printf("Motor started\r\n");
    } else {
        printf("Unknown command: %s\r\n", real_cmd);
    }
}


void parse_esp_response(void) {
    esp_rx_buf[esp_rx_idx] = '\0'; // Null-terminate the buffer

    // Update state based on command index (7 commands total now)
    if (cmd_index >= 0 && cmd_index <= 3) {
        current_state = ESP_STATE_INIT;
    } else if (cmd_index == 4) {
        current_state = ESP_STATE_WIFI_CONNECT;
    } else if (cmd_index == 5) {
        current_state = ESP_STATE_TCP_CONNECT;
    } else if (cmd_index == 6) {  // Only AT+CIPSEND state remains
        current_state = ESP_STATE_DATA_SEND;
    } else if (current_state == ESP_STATE_SUCCESS) {
        // After TCP connection, parse received server commands directly
        parse_tcp_command(esp_rx_buf);
        memset(esp_rx_buf, 0, ESP_RX_BUF_SIZE);
        esp_rx_idx = 0;
        return;
    }

    // Handle responses during connection phase
    switch (current_state) {
        case ESP_STATE_INIT:
        case ESP_STATE_WIFI_CONNECT:
        case ESP_STATE_TCP_CONNECT:
        case ESP_STATE_DATA_SEND:
            if (strstr(esp_rx_buf, expect_responses[cmd_index]) != NULL) {
                cmd_index++;     
                retry_count = 0; 
                // Enter success state when all commands are executed
                if (cmd_index >= sizeof(cmds)/sizeof(cmds[0])) {
                    current_state = ESP_STATE_SUCCESS;
                    printf("TCP connection successful, waiting for server commands...\r\n");
                    return;
                }
                send_next_cmd(); 
            } else if (strstr(esp_rx_buf, "ERROR") != NULL || 
                       strstr(esp_rx_buf, "FAIL") != NULL ||
                       strstr(esp_rx_buf, "CLOSED") != NULL) {
                current_state = ESP_STATE_ERROR;
                printf("Command [%d] failed!\r\n", cmd_index);
            }
            break;
        case ESP_STATE_ERROR:
            printf("ESP is in ERROR state!\r\n");
            break;
        default:
            break;
    }

    // Clear buffer
    memset(esp_rx_buf, 0, ESP_RX_BUF_SIZE);
    esp_rx_idx = 0;
}



void send_next_cmd(void) {
    if (cmd_index >= sizeof(cmds)/sizeof(cmds[0])) {
        current_state = ESP_STATE_SUCCESS;
        printf("All commands executed, waiting for server commands...\r\n");
        return;
    }
    // Keep delays for critical commands
    if (cmd_index == 1) drv_systick_ms(3000);  
    if (cmd_index == 5) drv_systick_ms(9000);  
    if (cmd_index == 6) drv_systick_ms(7000);  
    cmd_start_time = GetTick();  
    usart_sendstr(USART3, cmds[cmd_index], strlen(cmds[cmd_index]));
    retry_count++;
}



