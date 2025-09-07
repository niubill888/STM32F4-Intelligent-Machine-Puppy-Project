#include "drv_led.h"
#include "drv_beep.h"
#include "drv_systick.h"
#include "drv_key.h"
#include "drv_exti.h"
#include "drv_fan.h"
#include "drv_motor.h"
#include "drv_usart.h"
#include "drv_timer.h"
#include "drv_esp12f.h"
#include <stdbool.h>  // 引入标准布尔类型（bool/true/false）
#include "drv_sht20.h"
#include "74hc595.h"
#include "drv_sg90.h"
#include "FreeRTOS.h"
#include "task.h"


// ========================== 全局变量定义 ==========================
// ESP12F模块状态机变量（0对应ESP_STATE_INIT，枚举在drv_esp12f.h中定义）
ESP_STATE current_state = 0;

// 数码管动态扫描位索引（volatile：防止编译器优化，任务中会修改）
volatile uint8_t seg_index = 0;

// 按键1标志位（用于标记按键1的触发状态，具体逻辑在其他模块）
uint8_t key1_flag = 0;

// SHT20温湿度传感器数据缓存（存储原始采集数据）
uint16_t SHT20_data = 0;

// FreeRTOS通信对象（信号量/队列，用于任务间同步/通信）
xSemaphoreHandle esp_rx_semaphore;  // ESP响应处理信号量（二值信号量）
xQueueHandle xServoQueue;           // 舵机指令队列（存储ServoCmd_t类型指令）
xQueueHandle xASRQueue;             // 语音指令队列（原代码中未显式定义，保持原有逻辑）


// ========================== ESP响应处理任务 ==========================
/**
 * @brief ESP12F模块响应处理任务
 * @note  阻塞等待esp_rx_semaphore信号量，收到信号后解析ESP响应
 *        替代原中断驱动的响应处理，避免中断中做复杂逻辑
 * @param pvParameters：任务参数（未使用，传NULL）
 */
void esp_process_task(void *pvParameters) {
    while (1) {
        // 阻塞等待信号量（无限超时，直到ESP有数据需要处理）
        if (xSemaphoreTake(esp_rx_semaphore, portMAX_DELAY) == pdTRUE) {
            parse_esp_response();  // 解析ESP模块的串口响应数据
        }
    }
}


// ========================== 数码管相关任务 ==========================
/**
 * @brief 数码管动态扫描任务
 * @note  替代原TIM7定时器中断，循环切换数码管位选、输出段码
 *        扫描周期5ms，防止显示闪烁；4位数码管循环扫描
 * @param pvParameters：任务参数（未使用，传NULL）
 */
static void vSegScanTask(void *pvParameters) {
    // 数码管段码表：0~9（共阴极，最后一位0x80是小数点段码）
    const uint8_t data_code[11] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 
                                   0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x80};
    // 数码管位选码：控制哪一位点亮（共5位，实际用前4位）
    const uint8_t bit_code[5] = {0x01, 0x02, 0x04, 0x08, 0x02};
    uint8_t wei_pos = 0;  // 位选位置（未实际使用，保留原变量）

    while (1) {
        // 根据当前扫描索引，点亮对应位数码管
        switch (seg_index) {
            case 0: drv_seg_display(bit_code[0], data_code[disp_n1]); break;  // 第1位
            case 1: drv_seg_display(bit_code[1], data_code[disp_n2] | data_code[10]); break;  // 第2位（带小数点）
            case 2: drv_seg_display(bit_code[2], data_code[disp_n3]); break;  // 第3位
            case 3: drv_seg_display(bit_code[3], data_code[disp_n4]); break;  // 第4位
        }
        // 循环切换扫描位（0→1→2→3→0）
        seg_index = (seg_index + 1) % 4;

        // 延时5ms：平衡显示亮度和CPU占用，防止闪烁
        vTaskDelay(pdMS_TO_TICKS(5));
        // delay_us(20);  // 原延时，保留注释供参考
    }
}

/**
 * @brief 数据更新任务
 * @note  替代原TIM6定时器中断，定时采集SHT20温湿度数据并更新显示
 *        采集周期500ms，避免频繁读取传感器影响稳定性
 * @param pvParameters：任务参数（未使用，传NULL）
 */
static void vDataUpdateTask(void *pvParameters) {
    uint16_t sensor_data;  // 传感器数据缓存（未实际使用，保留原变量）

    while (1) {
        // 读取SHT20温湿度数据（数据存入SHT20_data全局变量）
        sht20_getdata();
        // 更新温湿度数据到数码管显示缓冲区
        drv_sht20_update(sht20_data);

        // 延时500ms：温湿度变化慢，500ms周期足够
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


// ========================== 语音指令处理任务 ==========================
/**
 * @brief 语音指令处理任务
 * @note  从xASRQueue队列接收语音指令，分发到对应硬件控制逻辑
 *        任务优先级3，确保指令及时处理但不抢占核心任务（如舵机）
 * @param pvParameters：任务参数（未使用，传NULL）
 */
static void vASRProTask(void *pvParameters) {
    uint8_t cmd;  // 存储从队列接收的语音指令码

    while (1) {
        // 阻塞等待队列指令（无限超时，直到有语音指令）
        if (xQueueReceive(xASRQueue, &cmd, portMAX_DELAY) == pdPASS) {
            // 根据指令码执行对应硬件操作（原中断处理逻辑迁移至此）
            switch (cmd) {
                // 灯光控制
                case 0x01: drv_led_open();  break;  // 开灯
                case 0x02: drv_led_close(); break;  // 关灯

                // 舵机动作：指令转发到舵机队列（避免阻塞语音任务）
                case SERVO_STAND:    // 站立
                case SERVO_SITDOWN:  // 坐下
                case SERVO_FORWARD:  // 前进
                case SERVO_BACKUP:   // 后退
                case SERVO_BOW:      // 鞠躬
                case SERVO_LIFT_HEAD:// 抬头
                    // 非阻塞发送（超时10ms）：防止舵机队列满时阻塞语音任务
                    xQueueSend(xServoQueue, &cmd, pdMS_TO_TICKS(10));
                    break;

                // 温湿度显示模式切换（原TIM6/TIM7使能逻辑，保留注释）
                case 0x09: 
                    // key1_flag=0; TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
                    // TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
                    break;
                case 0x0A: 
                    // key1_flag=1; TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
                    // TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
                    break;

                // 报警/风扇/马达控制
                case 0x0B: drv_beep_open();  break;  // 蜂鸣器开
                case 0x0C: drv_fan_open();   break;  // 风扇开
                case 0x0D: drv_fan_close();  break;  // 风扇关
                case 0x0E: drv_motor_open(); break;  // 电机开
                case 0x0F: drv_motor_close();break;  // 机关闭

                // 未知指令：不处理
                default: break;
            }
        }
    }
}


// ========================== 舵机控制任务 ==========================
/**
 * @brief 舵机控制任务
 * @note  处理xServoQueue队列中的舵机指令，支持动作打断（新指令覆盖旧指令）
 *        任务优先级4（高于语音任务），确保舵机动作实时响应
 * @param pvParameters：任务参数（未使用，传NULL）
 */
static void vServoTask(void *pvParameters) {
    ServoCmd_t cmd;          // 接收的舵机指令
    BaseType_t xResult;      // 队列操作结果
    struct {                 // 舵机状态机（跟踪当前指令、步骤、循环次数）
        ServoCmd_t current_cmd;  // 当前执行的指令
        uint8_t step;            // 指令执行步骤（多步动作拆分）
        uint8_t loop_count;      // 循环次数（前进/后退需重复执行）
    } servo_state;

    // 初始化舵机状态机
    servo_state.current_cmd = 0;  // 初始无指令
    servo_state.step = 0;         // 初始步骤0
    servo_state.loop_count = 0;   // 初始循环次数0

    while (1) {
        // 非阻塞检查新指令（允许打断当前动作）
        xResult = xQueueReceive(xServoQueue, &cmd, 0);
        if (xResult == pdPASS) {
            // 收到新指令：重置状态机
            servo_state.current_cmd = cmd;
            servo_state.step = 0;
            // 前进/后退指令需要循环33次（原逻辑保留）
            if (cmd == SERVO_FORWARD || cmd == SERVO_BACKUP) {
                servo_state.loop_count = 33;
            }
        }

        // 根据当前指令执行对应动作（调用具体动作处理函数）
        switch (servo_state.current_cmd) {
            case SERVO_STAND:    handle_stand();    break;  // 执行站立动作
            case SERVO_SITDOWN:  handle_sitdown();  break;  // 执行坐下动作
            case SERVO_FORWARD:  handle_forward();  break;  // 执行前进动作
            case SERVO_BACKUP:   handle_backup();   break;  // 执行后退动作
            case SERVO_BOW:      handle_bow();      break;  // 执行鞠躬动作
            case SERVO_LIFT_HEAD:handle_lift_head();break;  // 执行抬头动作
            default:
                // 无指令时：延时10ms降低CPU占用（避免空循环）
                vTaskDelay(pdMS_TO_TICKS(10));
                break;
        }
    }
}


// ========================== 系统初始化任务 ==========================
/**
 * @brief 系统初始化任务
 * @note  优先级最高（5），完成所有外设初始化、FreeRTOS对象创建、任务创建
 *        初始化完成后自动删除自身，避免占用资源
 * @param pvParameters：任务参数（未使用，传NULL）
 */
static void vInitTask(void *pvParameters) {
    // 1. 硬件外设初始化（按依赖顺序）
    drv_led_init();        // LED初始化
    drv_beep_init();       // 蜂鸣器初始化
    drv_fan_init();        // 风扇初始化
    drv_motor_init();      // 电机初始化
    drv_key_init();        // 按键初始化
    drv_usart6_init();     // USART6初始化（具体用途根据项目定义）
    drv_usart3_init();     // USART3初始化（具体用途根据项目定义）

    // 2. FreeRTOS同步对象创建（ESP响应处理信号量）
    esp_rx_semaphore = xSemaphoreCreateBinary();
    if (esp_rx_semaphore == NULL) {
        // 信号量创建失败：死循环提示（实际项目可加LED闪烁报警）
        for (;;); 
    }

    // 3. 继续外设初始化
    drv_usart2_init();     // USART2初始化（ESP12F通信串口）
    current_state = ESP_STATE_INIT;  // 设置ESP初始状态
    drv_timer3_init();     // TIM3初始化（PWM输出，用于舵机/电机）

    // 4. 舵机指令队列创建（长度5，存储ServoCmd_t类型，防止指令丢失）
    xServoQueue = xQueueCreate(5, sizeof(ServoCmd_t));
    configASSERT(xServoQueue != NULL);  // 断言：确保队列创建成功（调试用）

    // 5. ESP12F模块初始化（发送第一条配置指令）
    send_next_cmd();
    tim2_delay_init();     // TIM2初始化（us级延时）

    // 6. 传感器与显示外设初始化
    sht20_init();          // SHT20温湿度传感器初始化
    drv_74hc595_init();    // 74HC595串转并芯片初始化（控制数码管）

    // 7. 创建FreeRTOS任务（按优先级从低到高，避免高优先级任务阻塞初始化）
    // ESP响应处理任务：优先级3，栈256（处理串口数据，需足够栈空间）
    xTaskCreate(esp_process_task, "ESP_Process", 256, NULL, 3, NULL);

    // 语音指令处理任务：优先级3，栈128（指令分发逻辑简单，栈可小）
    xTaskCreate(
        vASRProTask,          // 任务函数
        "ASRProTask",         // 任务名称
        128,                  // 栈大小（单位：字，1字=4字节）
        NULL,                 // 任务参数
        3,                    // 优先级
        NULL                  // 任务句柄（未使用）
    );

    // 舵机控制任务：优先级4，栈256（多步动作处理，需较大栈空间）
    xTaskCreate(
        vServoTask,
        "ServoTask",
        256,
        NULL,
        4,                    // 优先级高于语音任务，确保动作实时
        NULL
    );

    // 数码管扫描任务：优先级5，栈256（显示最优先，防止闪烁）
    xTaskCreate(vSegScanTask, "SegScan", 256, NULL, 5, NULL);

    // 数据更新任务：优先级2，栈256（采集周期长，优先级最低）
    xTaskCreate(vDataUpdateTask, "DataUpdate", 256, NULL, 2, NULL);

    // 8. 初始化完成：删除自身任务（初始化任务仅执行一次）
    vTaskDelete(NULL);
}


// ========================== 主函数 ==========================
/**
 * @brief 主函数
 * @note  仅完成NVIC分组配置、创建初始化任务、启动FreeRTOS调度器
 *        所有业务逻辑在任务中执行，主函数不做复杂操作
 */
int main(void)
{
    // NVIC优先级分组配置：4组（抢占优先级0~15，子优先级0）
    // 适配FreeRTOS任务优先级（FreeRTOS用抢占优先级）
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	

    // 创建系统初始化任务：优先级5（最高），确保先完成所有初始化
    xTaskCreate(
        vInitTask,  // 任务函数
        "Init",     // 任务名称
        256,        // 栈大小（初始化操作多，需足够栈）
        NULL,       // 任务参数
        5,          // 优先级（最高，避免被其他任务抢占）
        NULL        // 任务句柄（未使用）
    );

    // 启动FreeRTOS任务调度器（启动后不会返回，除非内存不足）
    vTaskStartScheduler();

    // ====================== 以下代码仅在调度器启动失败时执行 ======================
    // 原main函数中注释的初始化代码（保留供参考，不执行）
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	drv_led_init();
//	drv_beep_init();
//	drv_fan_init();
//	drv_motor_init();
//	drv_key_init();
//	drv_systick_init(168);
//	drv_usart6_init();
//	drv_usart3_init();
//	drv_usart2_init();
//	current_state = ESP_STATE_INIT;
//	drv_timer3_init();
//    send_next_cmd();
//	sht20_init();
//	drv_74hc595_init();
//		sht20_getdata();
//	drv_sht20_update(sht20_data);
//		drv_timer7_init();
//	drv_timer6_init();
//TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
//TIM_ITConfig(TIM7, TIM_IT_Update, DISABLE);
//seg_all_off();

    // 调度器启动失败：死循环（实际项目可加LED报警）
    while(1) {
//		voice_send_process();
//		esp_state_rp();
//	printf("main====================\r\n");
//		drv_systick_ms(1000);
    }
}