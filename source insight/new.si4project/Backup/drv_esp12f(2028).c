#include "drv_esp12f.h"
#include "drv_usart.h"


typedef enum {
    CMD_FAN,          // 风扇
    CMD_LED,          // LED
    CMD_BEEP,         // 蜂鸣器
    CMD_MOTOR,        // 电机
    CMD_TEMP_SHOW,    // 温度显示
    CMD_HUMI_SHOW,    // 湿度显示
    CMD_FLOW_LIGHT,   // 流水灯
    CMD_HEART_RATE,   // 心率脉搏
    CMD_UNKNOWN       // 未知命令
} CmdType;


uint32_t cmd_start_time = 0;
bool server_disconnected = false; // 初始化标志

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
// 辅助函数：将字符串命令转换为枚举类型
static CmdType get_cmd_type(const char* cmd_str) {
    char type[32] = {0};
    char* sep = strchr(cmd_str, '_');
    if (sep) strncpy(type, cmd_str, sep - cmd_str); // 提取设备类型（下划线前）
    
    if      (strcmp(type, "fan") == 0)        return CMD_FAN;
    else if (strcmp(type, "led") == 0)        return CMD_LED;
    else if (strcmp(type, "beep") == 0)       return CMD_BEEP;
    else if (strcmp(type, "motor") == 0)      return CMD_MOTOR;
    else if (strcmp(type, "temp_show") == 0)  return CMD_TEMP_SHOW;
    else if (strcmp(type, "humi_show") == 0)  return CMD_HUMI_SHOW;
    else if (strcmp(type, "flow_light") == 0) return CMD_FLOW_LIGHT;
    else if (strcmp(type, "heart_rate") == 0) return CMD_HEART_RATE;
    else                                      return CMD_UNKNOWN;
}

// Parse commands from TCP server to control peripherals
void parse_tcp_command(const char* cmd) {
	// 解析 ESP8266 透传数据格式：提取 "[+IPD,...:]" 后的真实命令
		const char* real_cmd = cmd;
		char* colon = strchr(cmd, ':');  // 定位协议头中的冒号
		if (colon != NULL) {
			real_cmd = colon + 1;  // 跳过冒号，指向实际命令内容
		}


    // 拆分命令：设备类型 + 操作（如 "fan_open" → 类型"fan"，操作"open"）
    char action[32] = {0};
    char* sep = strchr(real_cmd, '_');
    if (!sep) { // 命令格式错误（无下划线）
        printf("Invalid command: %s (missing '_')\r\n", real_cmd);
        return;
    }
    strcpy(action, sep + 1); // 提取操作（下划线后）
    // 转换为枚举类型，进入switch分发
    CmdType cmd_type = get_cmd_type(real_cmd);
    switch (cmd_type) {
        // -------------------- 风扇 --------------------
        case CMD_FAN:
            if      (strcmp(action, "open\r\n") == 0)  drv_fan_open();
            else if (strcmp(action, "close\r\n") == 0) drv_fan_close();
            else { printf("Invalid action for FAN: %s\r\n", action); return; }
            printf("Fan %s\r\n", action);
            break;

        // -------------------- LED --------------------
        case CMD_LED:
            if      (strcmp(action, "open\r\n") == 0)  drv_led_open();
            else if (strcmp(action, "close\r\n") == 0) drv_led_close();
            else { printf("Invalid action for LED: %s\r\n", action); return; }
            printf("LED %s\r\n", action);
            break;

        // -------------------- 蜂鸣器 --------------------
        case CMD_BEEP:
            if      (strcmp(action, "open\r\n") == 0)  drv_beep_open();
            else if (strcmp(action, "close\r\n") == 0) drv_beep_close();
            else { printf("Invalid action for BEEP: %s\r\n", action); return; }
            printf("Buzzer %s\r\n", action);
            break;

   /*     // -------------------- 电机 --------------------
        case CMD_MOTOR:
            if      (strcmp(action, "open") == 0)  drv_motor_open();
            else if (strcmp(action, "close") == 0) drv_motor_close();
            else { printf("Invalid action for MOTOR: %s\r\n", action); return; }
            printf("Motor %s\r\n", action);
            break;

        // -------------------- 温度显示 --------------------
        case CMD_TEMP_SHOW:
            if      (strcmp(action, "open") == 0)  drv_temp_display_open();  // 需实现：开启温度显示
            else if (strcmp(action, "close") == 0) drv_temp_display_close(); // 需实现：关闭温度显示
            else { printf("Invalid action for TEMP: %s\r\n", action); return; }
            printf("Temperature display %s\r\n", action);
            break;

        // -------------------- 湿度显示 --------------------
        case CMD_HUMI_SHOW:
            if      (strcmp(action, "open") == 0)  drv_humi_display_open();  // 需实现：开启湿度显示
            else if (strcmp(action, "close") == 0) drv_humi_display_close(); // 需实现：关闭湿度显示
            else { printf("Invalid action for HUMI: %s\r\n", action); return; }
            printf("Humidity display %s\r\n", action);
            break;

        // -------------------- 流水灯 --------------------
        case CMD_FLOW_LIGHT:
            if      (strcmp(action, "open") == 0)  drv_flow_light_open();    // 需实现：开启流水灯
            else if (strcmp(action, "close") == 0) drv_flow_light_close();   // 需实现：关闭流水灯
            else { printf("Invalid action for FLOW_LIGHT: %s\r\n", action); return; }
            printf("Flow light %s\r\n", action);
            break;

        // -------------------- 心率脉搏 --------------------
        case CMD_HEART_RATE:
            if      (strcmp(action, "open") == 0)  drv_heart_rate_start();   // 需实现：启动心率检测
            else if (strcmp(action, "close") == 0) drv_heart_rate_stop();    // 需实现：停止心率检测
            else { printf("Invalid action for HEART_RATE: %s\r\n", action); return; }
            printf("Heart rate test %s\r\n", action);
            break;
*/
        // -------------------- 未知命令 --------------------
        case CMD_UNKNOWN:
        default:
            printf("Unknown command: %s\r\n", real_cmd);
            break;
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
            }
            // 错误响应处理（新增服务器断开检测）
            else if (strstr(esp_rx_buf, "ERROR") != NULL || 
                     strstr(esp_rx_buf, "FAIL") != NULL ||
                     strstr(esp_rx_buf, "CLOSED") != NULL) {
                current_state = ESP_STATE_ERROR;
                // 检测到 "CLOSED" 时置位断开标志
                if (strstr(esp_rx_buf, "CLOSED") != NULL) {
                    server_disconnected = true; 
                }
                printf("Command [%d] failed! Response: %s\r\n", cmd_index, esp_rx_buf);
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



