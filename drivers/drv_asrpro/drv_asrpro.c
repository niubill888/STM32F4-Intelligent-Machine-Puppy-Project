#include "drv_asrpro.h"

// 语音发送状态机枚举
typedef enum {
    VOICE_SEND_IDLE,    // 空闲状态
    VOICE_SEND_ACTIVE   // 发送中状态
} VoiceSendState;

// 全局状态变量
VoiceSendState voice_send_state = VOICE_SEND_IDLE;
uint8_t voice_send_cmd = 0;  // 待发送的十六进制指令

// 启动向语音模块发送十六进制指令（非阻塞）
void send_voice_hex(uint8_t cmd) {
    if (voice_send_state == VOICE_SEND_IDLE) {
        voice_send_state = VOICE_SEND_ACTIVE;
        voice_send_cmd = cmd;
    }
}

// 语音发送状态机处理函数（在主循环中调用）
void voice_send_process(void) {
    if (voice_send_state == VOICE_SEND_ACTIVE) {
        if (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == SET) {
            USART_SendData(USART2, voice_send_cmd);
            voice_send_state = VOICE_SEND_IDLE;  // 发送完成，回到空闲状态
        }
    }
}


