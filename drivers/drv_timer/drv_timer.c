#include "drv_timer.h"


// ��ʼ��TIM2��Ϊus����ʱ��ʱ��
// ˵����APB1ʱ��84MHz����Ƶ84�����Ƶ��1MHz��1����=1us��
// �����ʱ65535us��65ms�������������
void tim2_delay_init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // ʹ��TIM2ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // ����ʱ������
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // ������ֵ�����������ʱ
    TIM_TimeBaseStructure.TIM_Prescaler = 84;   // 84MHz/84=1MHz��1us����һ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM2, DISABLE);  // �ȹض�ʱ�����õ�ʱ���ٿ�
}


// ΢�뼶��ʱ
// ע�⣺�����ȵ���tim2_delay_init()��usֵ���ܳ���65535
void delay_us(uint32_t us) {
    TIM_SetCounter(TIM2, 0);  // ����������
    TIM_Cmd(TIM2, ENABLE);    // ��������
    while (TIM_GetCounter(TIM2) < us);  // �ȼ�����Ŀ��ֵ
    TIM_Cmd(TIM2, DISABLE);   // ֹͣ����
}


// ���뼶��ʱ������us��ʱʵ�֣�
// ����һ�㣬�ʺϼ򵥳���
void delay_ms(uint32_t ms) {
    while (ms--) {
        delay_us(1000);  // 1ms = 1000us
    }
}


// ��ʼ��TIM3������4·PWM���
// ��Ӧ���ţ�PA6(CH1)��PA7(CH2)��PB0(CH3)��PB1(CH4)
// ��;������������������
void drv_timer3_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    // ʹ������ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // PA6/7�õ�
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // PB0/1�õ�
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   // TIM3ʱ��

    // ����GPIOΪ���ù��ܣ���TIM3��
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;  // ����ģʽ
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // PA6��PA7
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // ������������
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;  // �������
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;  // ����
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  // PB0��PB1
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // ���Ÿ���ӳ�䵽TIM3
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
    
    // ����TIM3ʱ��������PWM���ڣ�
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 2000 - 1;  
    TIM_TimeBaseInitStruct.TIM_Prescaler = 840 - 1;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    // ����PWMģʽ��4��ͨ��ͨ�����ײ�����
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;  // PWMģʽ1
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;  // ʹ�����
    TIM_OCInitStruct.TIM_Pulse = 0;  // ��ʼռ�ձ�0
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  // �ߵ�ƽ��Ч

    // ��ʼ��4��ͨ��
    TIM_OC1Init(TIM3, &TIM_OCInitStruct);
    TIM_OC2Init(TIM3, &TIM_OCInitStruct);
    TIM_OC3Init(TIM3, &TIM_OCInitStruct);
    TIM_OC4Init(TIM3, &TIM_OCInitStruct);

    // ���ô�������������¼�������ȫ������һ�ξ��У�
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    // ʹ��Ԥ����
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    // ����PWM���
    TIM_CtrlPWMOutputs(TIM3, ENABLE);  // ��Щ��ʱ����Ҫ���������
    TIM_Cmd(TIM3, ENABLE);  // ������ʱ��
}