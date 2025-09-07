#include "drv_esp12f.h"
#include "drv_usart.h"
#include "drv_key.h"
#include "drv_asrpro.h"
#include "FreeRTOS.h"  // FreeRTOS核心头文件
#include "task.h"      // 任务管理相关函数

// 全局变量定义
uint8_t log = 0;  // ESP响应处理标志（0：未处理，1：需处理）
// ESP接收信号量句柄（用于同步中断与任务）
SemaphoreHandle_t esp_rx_semaphore = NULL;

// 命令类型枚举（按设备类型分类）
typedef enum {
    CMD_FAN,          // 风扇控制命令
    CMD_LED,          // LED控制命令
    CMD_BEEP,         // 蜂鸣器控制命令
    CMD_MOTOR,        // 电机控制命令
    CMD_TEMP_SHOW,    // 温度显示命令
    CMD_HUMI_SHOW,    // 湿度显示命令
    CMD_FLOW_LIGHT,   // 流水灯控制命令
    CMD_UNKNOWN       // 未知命令
} CmdType;

uint32_t cmd_start_time = 0;  // 命令发送起始时间（用于超时判断）
bool server_disconnected = false;  // 服务器断开标志
char esp_rx_buf[ESP_RX_BUF_SIZE];  // ESP接收缓冲区
uint8_t esp_rx_idx;  // 接收缓冲区索引
uint8_t last_rx_time;  // 最后一次接收数据的时间
uint8_t cmd_index;  // 当前执行的命令索引
uint8_t retry_count;  // 命令重试次数
bool led_status;  // LED状态记录（亮/灭）


// ESP初始化命令数组（按顺序执行）
const char* cmds[] = {
    "AT+RESTORE\r\n",                   // 0: 恢复出厂设置
    "AT\r\n",                           // 1: 测试AT指令
    "AT+CWMODE=1\r\n",                  // 2: 设置STA模式
    "AT+CIPMUX=0\r\n",                  // 3: 关闭多连接
    "AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"\r\n",  // 4: 连接指定WIFI
    "AT+CIPSTART=\"TCP\",\"192.168.20.248\",7777\r\n",  // 5: 连接TCP服务器
    "AT+CIPSEND=8\r\n"                  // 6: 准备发送数据（长度8）
};

// 命令对应的期望响应（用于判断命令是否执行成功）
const char* expect_responses[] = {
    "OK",           // 0: AT+RESTORE成功
    "OK",           // 1: AT测试成功
    "OK",           // 2: 模式设置成功
    "OK",           // 3: 多连接关闭成功
    "WIFI CONNECTED", // 4: WIFI连接成功
    "CONNECT",      // 5: TCP连接成功
    ">"             // 6: 准备发送成功
};

// 各命令超时时间（单位：ms）
const uint32_t timeouts[] = {
    2000, 2000, 2000, 2000, 15000, 10000, 5000
};


/**
 * 将字符串命令转换为枚举类型（便于switch处理）
 * @param cmd_str：待解析的命令字符串（格式如"fan_open"）
 * @return 对应的CmdType枚举值
 */
static CmdType get_cmd_type(const char* cmd_str) {
    char type[32] = {0};
    char* sep = strchr(cmd_str, '_');  // 查找下划线分隔符
    if (sep) strncpy(type, cmd_str, sep - cmd_str);  // 提取设备类型（下划线前部分）
    
    // 匹配设备类型
    if      (strcmp(type, "fan") == 0)        return CMD_FAN;
    else if (strcmp(type, "led") == 0)        return CMD_LED;
    else if (strcmp(type, "beep") == 0)       return CMD_BEEP;
    else if (strcmp(type, "motor") == 0)      return CMD_MOTOR;
    else if (strcmp(type, "temp") == 0)       return CMD_TEMP_SHOW;
    else if (strcmp(type, "hum") == 0)        return CMD_HUMI_SHOW;
    else if (strcmp(type, "flow") == 0)       return CMD_FLOW_LIGHT;
    else                                      return CMD_UNKNOWN;
}


/**
 * 解析TCP服务器发送的命令
 * @param cmd：原始命令（含ESP透传格式头）
 * 功能：提取有效命令内容，按"设备_操作"格式解析并执行对应动作
 */
void parse_tcp_command(const char* cmd) {
    // 提取真实命令（去除"[+IPD,...:]"协议头）
    const char* real_cmd = cmd;
    char* colon = strchr(cmd, ':');  // 定位冒号分隔符
    if (colon != NULL) {
        real_cmd = colon + 1;  // 指向冒号后的实际命令
    }

    // 拆分命令为"设备类型"和"操作"（如"fan_open"→设备"fan"，操作"open"）
    char action[32] = {0};
    char* sep = strchr(real_cmd, '_');
    if (!sep) {  // 命令格式错误（无下划线）
        printf("Invalid command: %s (missing '_')\r\n", real_cmd);
        return;
    }
    strcpy(action, sep + 1);  // 提取操作部分

    // 根据命令类型执行对应操作
    CmdType cmd_type = get_cmd_type(real_cmd);
    switch (cmd_type) {
        case CMD_FAN:  // 风扇控制
            if      (strcmp(action, "open\r\n") == 0)  drv_fan_open();
            else if (strcmp(action, "close\r\n") == 0) drv_fan_close();
            else { printf("Invalid action for FAN: %s\r\n", action); return; }
            printf("Fan %s\r\n", action);
            break;

        case CMD_LED:  // LED控制
            if      (strcmp(action, "open\r\n") == 0)  drv_led_open();
            else if (strcmp(action, "close\r\n") == 0) drv_led_close();
            else { printf("Invalid action for LED: %s\r\n", action); return; }
            printf("LED %s\r\n", action);
            break;

        case CMD_BEEP:  // 蜂鸣器控制
            if      (strcmp(action, "open\r\n") == 0)  drv_beep_open();
            else if (strcmp(action, "close\r\n") == 0) drv_beep_close();
            else { printf("Invalid action for BEEP: %s\r\n", action); return; }
            printf("Buzzer %s\r\n", action);
            break;

        case CMD_MOTOR:  // 电机控制
            if      (strcmp(action, "open\r\n") == 0)  drv_motor_open();
            else if (strcmp(action, "close\r\n") == 0) drv_motor_close();
            else { printf("Invalid action for MOTOR: %s\r\n", action); return; }
            printf("Motor %s\r\n", action);
            break;

        case CMD_TEMP_SHOW:  // 温度显示
            if      (strcmp(action, "open\r\n") == 0) 	{} // 触发温度更新（预留）
            else { printf("Invalid action for TEMP: %s\r\n", action); return; }
            printf("Temperature display %s\r\n", action);
            break;

        case CMD_HUMI_SHOW:  // 湿度显示
            if      (strcmp(action, "open\r\n") == 0){}// 切换湿度显示（预留）
            else { printf("Invalid action for HUMI: %s\r\n", action); return; }
            printf("Humidity display %s\r\n", action);
            break;

        case CMD_FLOW_LIGHT:  // 流水灯控制
            if      (strcmp(action, "open\r\n") == 0)  drv_led_flow();
            else if (strcmp(action, "close\r\n") == 0) drv_led_close();
            else { printf("Invalid action for FLOW_LIGHT: %s\r\n", action); return; }
            printf("Flow light %s\r\n", action);
            break;

        default:  // 未知命令
            printf("Unknown command: %s\r\n", real_cmd);
            break;
    }
}


/**
 * 解析ESP8266模块的响应数据
 * 功能：根据当前状态判断命令执行结果，更新状态机，触发下一步操作
 */
void parse_esp_response(void) {
    esp_rx_buf[esp_rx_idx] = '\0';  // 字符串结尾标记

    // 更新当前状态（基于命令索引）
    if (cmd_index >= 0 && cmd_index <= 3) {
        current_state = ESP_STATE_INIT;  // 初始化阶段
    } else if (cmd_index == 4) {
        current_state = ESP_STATE_WIFI_CONNECT;  // WIFI连接阶段
    } else if (cmd_index == 5) {
        current_state = ESP_STATE_TCP_CONNECT;  // TCP连接阶段
    } else if (cmd_index == 6) {
        current_state = ESP_STATE_DATA_SEND;  // 数据发送阶段
    } else if (current_state == ESP_STATE_SUCCESS) {
        // 连接成功后，解析服务器命令
        parse_tcp_command(esp_rx_buf);
        memset(esp_rx_buf, 0, ESP_RX_BUF_SIZE);  // 清空缓冲区
        esp_rx_idx = 0;
        return;
    }

    // 按当前状态处理响应
    switch (current_state) {
        case ESP_STATE_INIT:  // 初始化阶段（前4条命令）
            if (strstr(esp_rx_buf, expect_responses[cmd_index]) != NULL) {
                cmd_index++;  // 命令成功，执行下一条
                retry_count = 0;
                send_next_cmd();
            } else if (strstr(esp_rx_buf, "ERROR") || strstr(esp_rx_buf, "FAIL")) {
                current_state = ESP_STATE_ERROR;  // 命令失败，进入错误状态
                printf("Command [%d] failed! Response: %s\r\n", cmd_index, esp_rx_buf);
            }
            break;

        case ESP_STATE_WIFI_CONNECT:  // WIFI连接阶段（cmd_index=4）
            if (strstr(esp_rx_buf, "WIFI CONNECTED") != NULL) {
                send_voice_hex(0xAB);  // 语音提示：WIFI连接成功
                cmd_index++;
                retry_count = 0;
                send_next_cmd();
            } else if (strstr(esp_rx_buf, "FAIL") || strstr(esp_rx_buf, "ERROR")) {
                send_voice_hex(0xAE);  // 语音提示：WIFI连接失败
                current_state = ESP_STATE_ERROR;
                printf("WIFI connection failed! Response: %s\r\n", esp_rx_buf);
            }
            break;

        case ESP_STATE_TCP_CONNECT:  // TCP连接阶段（cmd_index=5）
            if (strstr(esp_rx_buf, "CONNECT") != NULL) {
                send_voice_hex(0xAD);  // 语音提示：服务器连接成功
                cmd_index++;
                retry_count = 0;
                send_next_cmd();
            } else if (strstr(esp_rx_buf, "CLOSED") || strstr(esp_rx_buf, "FAIL") || strstr(esp_rx_buf, "ERROR")) {
                send_voice_hex(0xAF);  // 语音提示：服务器连接失败
                current_state = ESP_STATE_ERROR;
                server_disconnected = true;
                printf("Server connection failed! Response: %s\r\n", esp_rx_buf);
            }
            break;

        case ESP_STATE_DATA_SEND:  // 数据发送阶段（cmd_index=6）
            if (strstr(esp_rx_buf, expect_responses[cmd_index]) != NULL) {
                cmd_index++;
                retry_count = 0;
                if (cmd_index >= sizeof(cmds)/sizeof(cmds[0])) {
                    current_state = ESP_STATE_SUCCESS;  // 所有命令执行完成
                    printf("TCP connection successful, waiting for server commands...\r\n");
                    return;
                }
                send_next_cmd();
            } else if (strstr(esp_rx_buf, "ERROR") || strstr(esp_rx_buf, "FAIL") || strstr(esp_rx_buf, "CLOSED")) {
                current_state = ESP_STATE_ERROR;
                if (strstr(esp_rx_buf, "CLOSED")) server_disconnected = true;
                printf("Command [%d] failed! Response: %s\r\n", cmd_index, esp_rx_buf);
            }
            break;

        case ESP_STATE_ERROR:  // 错误状态
            printf("ESP is in ERROR state!\r\n");
            break;

        default:
            break;
    }

    // 清空接收缓冲区，准备下次接收
    memset(esp_rx_buf, 0, ESP_RX_BUF_SIZE);
    esp_rx_idx = 0;
}


/**
 * 发送下一条ESP初始化命令
 * 功能：按顺序发送cmds数组中的命令，含必要延时和语音提示
 */
void send_next_cmd(void) {
    // 所有命令执行完成，进入成功状态
    if (cmd_index >= sizeof(cmds)/sizeof(cmds[0])) {
        current_state = ESP_STATE_SUCCESS;
        printf("All commands executed, waiting for server commands...\r\n");
        return;
    }

    // 关键节点语音提示
    if (cmd_index == 4) {  // 即将连接WIFI
        send_voice_hex(0xAA);  // 语音：WIFI正在连接
    } else if (cmd_index == 5) {  // 即将连接服务器
        send_voice_hex(0xAC);  // 语音：正在连接服务器
    }

    // 必要延时（适配ESP模块响应时间）
    if (cmd_index == 1) vTaskDelay(pdMS_TO_TICKS(3000));  // 3秒延时
    if (cmd_index == 5) vTaskDelay(pdMS_TO_TICKS(9000));  // 9秒延时
    if (cmd_index == 6) vTaskDelay(pdMS_TO_TICKS(7000));  // 7秒延时

    // 发送命令并记录起始时间
    cmd_start_time = xTaskGetTickCount();  // 获取当前系统Tick
    usart_sendstr(USART3, cmds[cmd_index], strlen(cmds[cmd_index]));
    retry_count++;  // 重试计数+1
}


/**
 * ESP状态轮询函数
 * 功能：处理接收超时、命令超时和错误恢复
 */
void esp_state_rp(void) {
    // 接收超时处理（100ms无新数据则触发解析）
    if (esp_rx_idx > 0 && (xTaskGetTickCount() - last_rx_time) > pdMS_TO_TICKS(100)) {
        if (log == 0) {
            log = 1;  // 置位处理标志
        }
    }

    // 命令超时处理（未在规定时间内收到期望响应）
    if (current_state != ESP_STATE_ERROR && current_state != ESP_STATE_SUCCESS) {
        if (xTaskGetTickCount() - cmd_start_time > pdMS_TO_TICKS(timeouts[cmd_index])) {
            printf("Command [%d] timeout!+++++++++++++\r\n", cmd_index);
            if (retry_count < 3) {  // 重试3次
                printf("Retrying command [%d]...\r\n", cmd_index);
                send_next_cmd();
            } else {
                current_state = ESP_STATE_TIMEOUT;  // 超过最大重试次数，进入超时状态
                printf("Max retries reached. Entering ERROR state.\r\n");
            }
        }
    }

    // 错误/超时状态恢复（重新初始化连接）
    if (current_state == ESP_STATE_ERROR || current_state == ESP_STATE_TIMEOUT) {
        printf("Resetting ESP connection...\r\n");
        cmd_index = 0;		   
        retry_count = 0;		
        current_state = ESP_STATE_INIT;
        led_status = false; 	
        server_disconnected = false;  // 清除断开标志
        send_next_cmd();  // 重新开始发送命令
    }

    // 10ms延时，降低CPU占用
    vTaskDelay(pdMS_TO_TICKS(10));
}