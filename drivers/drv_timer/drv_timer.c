#include "drv_timer.h"


// 初始化TIM2作为us级延时定时器
// 说明：APB1时钟84MHz，分频84后计数频率1MHz（1计数=1us）
// 最大延时65535us（65ms），超过会溢出
void tim2_delay_init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 使能TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 配置时基参数
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // 最大计数值，决定最大延时
    TIM_TimeBaseStructure.TIM_Prescaler = 84;   // 84MHz/84=1MHz，1us计数一次
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM2, DISABLE);  // 先关定时器，用的时候再开
}


// 微秒级延时
// 注意：必须先调用tim2_delay_init()，us值不能超过65535
void delay_us(uint32_t us) {
    TIM_SetCounter(TIM2, 0);  // 计数器清零
    TIM_Cmd(TIM2, ENABLE);    // 启动计数
    while (TIM_GetCounter(TIM2) < us);  // 等计数到目标值
    TIM_Cmd(TIM2, DISABLE);   // 停止计数
}


// 毫秒级延时（基于us延时实现）
// 精度一般，适合简单场景
void delay_ms(uint32_t ms) {
    while (ms--) {
        delay_us(1000);  // 1ms = 1000us
    }
}


// 初始化TIM3，配置4路PWM输出
// 对应引脚：PA6(CH1)、PA7(CH2)、PB0(CH3)、PB1(CH4)
// 用途：可驱动舵机、电机等
void drv_timer3_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    // 使能外设时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // PA6/7用到
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // PB0/1用到
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   // TIM3时钟

    // 配置GPIO为复用功能（接TIM3）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;  // 复用模式
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // PA6、PA7
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 不上拉不下拉
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;  // 推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;  // 高速
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  // PB0、PB1
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 引脚复用映射到TIM3
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
    
    // 配置TIM3时基（决定PWM周期）
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 2000 - 1;  
    TIM_TimeBaseInitStruct.TIM_Prescaler = 840 - 1;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    // 配置PWM模式（4个通道通用这套参数）
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;  // 使能输出
    TIM_OCInitStruct.TIM_Pulse = 0;  // 初始占空比0
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  // 高电平有效

    // 初始化4个通道
    TIM_OC1Init(TIM3, &TIM_OCInitStruct);
    TIM_OC2Init(TIM3, &TIM_OCInitStruct);
    TIM_OC3Init(TIM3, &TIM_OCInitStruct);
    TIM_OC4Init(TIM3, &TIM_OCInitStruct);

    // 设置触发输出（更新事件触发，全局配置一次就行）
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    // 使能预加载
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    // 启动PWM输出
    TIM_CtrlPWMOutputs(TIM3, ENABLE);  // 有些定时器需要这句才能输出
    TIM_Cmd(TIM3, ENABLE);  // 启动定时器
}