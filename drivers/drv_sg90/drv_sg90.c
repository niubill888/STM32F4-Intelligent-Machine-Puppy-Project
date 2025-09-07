#include "drv_sg90.h"
 //250   200      150    100    50
 //50   100     150     200    250



// 全局变量
uint8_t dog_state[5] = {50, 100, 150, 200, 250};
QueueHandle_t xServoQueue;  // 舵机指令队列（接收来自语音任务的指令）
ServoState_t servo_state;   // 舵机状态机变量
//uint8_t oc1=0,oc2=0,oc3=0,oc4=0;//3,4是pb,,,


// 处理站立动作（原dog_stand）
void handle_stand(void) {
    switch (servo_state.step) {
        case 0:
            // 执行站立姿态
            TIM_SetCompare1(TIM3, dog_state[2]);
            TIM_SetCompare2(TIM3, dog_state[2]);
            TIM_SetCompare3(TIM3, dog_state[2]);
            TIM_SetCompare4(TIM3, dog_state[2]);
            servo_state.step = 1;
            vTaskDelay(pdMS_TO_TICKS(3000));  // 替代drv_systick_ms(3000)
            break;
        case 1:
            // 动作完成，重置状态机
            servo_state.current_cmd = 0;
            servo_state.step = 0;
            break;
    }
}

// 处理前进动作（原dog_forward，多步骤+循环）
void handle_forward(void) {
    if (servo_state.loop_count == 0) {
        // 所有循环完成，复位姿态
        TIM_SetCompare1(TIM3, dog_state[2]);
        TIM_SetCompare2(TIM3, dog_state[2]);
        TIM_SetCompare3(TIM3, dog_state[2]);
        TIM_SetCompare4(TIM3, dog_state[2]);
        servo_state.current_cmd = 0;
        servo_state.step = 0;
        return;
    }

    // 单轮循环内的4个步骤
    switch (servo_state.step) {
        case 0:  // 步骤1：初始姿态
            TIM_SetCompare1(TIM3, dog_state[4]);
            TIM_SetCompare2(TIM3, dog_state[2]);
            TIM_SetCompare3(TIM3, dog_state[0]);
            TIM_SetCompare4(TIM3, dog_state[2]);
            servo_state.step = 1;
            vTaskDelay(pdMS_TO_TICKS(200));  // 非阻塞延时
            break;
        case 1:  // 步骤2：前腿前移
            TIM_SetCompare1(TIM3, dog_state[0]);
            TIM_SetCompare3(TIM3, dog_state[4]);
            servo_state.step = 2;
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
        case 2:  // 步骤3：后腿抬起
            TIM_SetCompare2(TIM3, dog_state[4]);
            TIM_SetCompare4(TIM3, dog_state[0]);
            TIM_SetCompare1(TIM3, dog_state[2]);
            TIM_SetCompare3(TIM3, dog_state[2]);
            servo_state.step = 3;
            vTaskDelay(pdMS_TO_TICKS(200));
            break;
        case 3:  // 步骤4：后腿前移（完成一轮循环）
            TIM_SetCompare2(TIM3, dog_state[0]);
            TIM_SetCompare4(TIM3, dog_state[4]);
            servo_state.step = 0;  // 回到步骤0，开始下一轮循环
            servo_state.loop_count--;  // 循环计数-1
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
    }
}

// 处理后退动作（类似前进，状态机实现）
void handle_backup(void) {
    if (servo_state.loop_count == 0) {
        // 复位姿态
        TIM_SetCompare1(TIM3, dog_state[2]);
        TIM_SetCompare2(TIM3, dog_state[2]);
        TIM_SetCompare3(TIM3, dog_state[2]);
        TIM_SetCompare4(TIM3, dog_state[2]);
        servo_state.current_cmd = 0;
        servo_state.step = 0;
        return;
    }

    switch (servo_state.step) {
        case 0:  // 步骤1：初始姿态
            TIM_SetCompare1(TIM3, dog_state[2]);
            TIM_SetCompare2(TIM3, dog_state[0]);
            TIM_SetCompare3(TIM3, dog_state[2]);
            TIM_SetCompare4(TIM3, dog_state[4]);
            servo_state.step = 1;
            vTaskDelay(pdMS_TO_TICKS(200));
            break;
        case 1:  // 步骤2：前腿动作
            TIM_SetCompare2(TIM3, dog_state[4]);
            TIM_SetCompare4(TIM3, dog_state[0]);
            servo_state.step = 2;
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
        case 2:  // 步骤3：后腿抬起
            TIM_SetCompare2(TIM3, dog_state[2]);
            TIM_SetCompare4(TIM3, dog_state[2]);
            TIM_SetCompare1(TIM3, dog_state[0]);
            TIM_SetCompare3(TIM3, dog_state[4]);
            servo_state.step = 3;
            vTaskDelay(pdMS_TO_TICKS(200));
            break;
        case 3:  // 步骤4：后腿动作（完成一轮循环）
            TIM_SetCompare1(TIM3, dog_state[4]);
            TIM_SetCompare3(TIM3, dog_state[0]);
            servo_state.step = 0;
            servo_state.loop_count--;
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
    }
}

// 处理俯身动作（原dog_bow）
void handle_bow(void) {
    switch (servo_state.step) {
        case 0:
            TIM_SetCompare1(TIM3, dog_state[0]);
            TIM_SetCompare2(TIM3, dog_state[2]);
            TIM_SetCompare3(TIM3, dog_state[4]);
            TIM_SetCompare4(TIM3, dog_state[2]);
            servo_state.step = 1;
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        case 1:
            servo_state.current_cmd = 0;
            servo_state.step = 0;
            break;
    }
}

// 处理抬头动作（原dog_lift_head）
void handle_lift_head(void) {
    switch (servo_state.step) {
        case 0:
            TIM_SetCompare1(TIM3, dog_state[2]);
            TIM_SetCompare2(TIM3, dog_state[4]);
            TIM_SetCompare3(TIM3, dog_state[2]);
            TIM_SetCompare4(TIM3, dog_state[0]);
            servo_state.step = 1;
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        case 1:
            servo_state.current_cmd = 0;
            servo_state.step = 0;
            break;
    }
}

// 处理坐下动作（原dog_sitdown）
void handle_sitdown(void) {
    switch (servo_state.step) {
        case 0:
            TIM_SetCompare1(TIM3, dog_state[4]);
            TIM_SetCompare2(TIM3, dog_state[4]);
            TIM_SetCompare3(TIM3, dog_state[0]);
            TIM_SetCompare4(TIM3, dog_state[0]);
            servo_state.step = 1;
            vTaskDelay(pdMS_TO_TICKS(3000));
            break;
        case 1:
            servo_state.current_cmd = 0;
            servo_state.step = 0;
            break;
    }
}