/**
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include <stdio.h>
#include <string.h>
#include "drv_led.h"
#include <stdlib.h>
#include "drv_systick.h"
#include "drv_esp12f.h"
#include "74hc595.h"
#include "drv_sg90.h"
#include "drv_spi2.h"
#include "drv_usart.h"


/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*void SVC_Handler(void)
{
}*/

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*void PendSV_Handler(void)
{
}*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*void SysTick_Handler(void)
{
}*/

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s/startup_stm32f429x.s).    */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 


char rcbuffer6[12]={0};
char rcdata6=0;
int i6=0;
char rcbuffer3[50]={0};
char rcdata3=0;
int i3=0;


void USART6_IRQHandler(void)
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE))//判断是不是接收串口的中断
	{
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
		// 接收一个字节数据
        if(i6 < ESP_RX_BUF_SIZE - 1)
        {
        	//printf("%c\r\n",rcbuffer6[i6]);
            rcbuffer6[i6++] = (uint8_t)USART_ReceiveData(USART6);
        }
        else
        {
            // 缓冲区已满，重置索引
            i6 = 0;
        }
		
	}
	else if(USART_GetITStatus(USART6, USART_IT_IDLE))
	{
		USART_ReceiveData(USART6);//要想关闭空闲中断必须执行读取数据寄存器的值
		if(i6 > 0)
        {
            rcbuffer6[i6] = '\0'; // 添加字符串结束符
            printf("%s",rcbuffer6);
            //rx_complete = 1;    // 设置接收完成标志
            /*if(0==strcmp(rcbuffer6,"SEND OK\0"))
            {
				drv_led_flow();
            }*/
            
            // 重置缓冲区索引
            i6 = 0;
		}
	}
}
/*
void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3, USART_IT_RXNE)!=RESET)
	{
		 // 1. 读取接收到的字符
        char c = (char)USART_ReceiveData(USART3);
        
         // 2. 存储到缓冲区（防止溢出）
        if (esp_rx_idx < ESP_RX_BUF_SIZE - 1) {
            esp_rx_buf[esp_rx_idx++] = c;
            last_rx_time = xTaskGetTickCount(); // 改用FreeRTOS Tick计数
        }
		USART_SendData(USART6, USART_ReceiveData(USART3));//发送数据给USART6相当于发送给串口助手
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
	}
	else if(USART_GetITStatus(USART3, USART_IT_IDLE))
	{
		USART_ReceiveData(USART3);//要想关闭空闲中断必须执行读取数据寄存器的值
		if(esp_rx_idx>0)
		log = 1;//parse_esp_response(); // 解析 ESP 响应，更新状态		;
	}
}*/

void USART3_IRQHandler(void)
{	
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;  // 标记是否需切换任务
	if(USART_GetITStatus(USART3, USART_IT_RXNE)!=RESET)
	{
		 // 1. 读取接收到的字符
        char c = (char)USART_ReceiveData(USART3);
        
         // 2. 存储到缓冲区（防止溢出）
        if (esp_rx_idx < ESP_RX_BUF_SIZE - 1) {
            esp_rx_buf[esp_rx_idx++] = c;
            last_rx_time = xTaskGetTickCount(); // FreeRTOS 时间戳
        }
		USART_SendData(USART6, USART_ReceiveData(USART3));// 回显到串口助手
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
	}
	else if(USART_GetITStatus(USART3, USART_IT_IDLE))
	{
		USART_ReceiveData(USART3);// 清除空闲中断需读数据寄存器
		if(esp_rx_idx>0)
		{
//			 printf("Enter ISR: Give Semaphore\n"); // 测试中断是否触发
            // 释放信号量，通知任务“有数据待解析”
            xSemaphoreGiveFromISR(esp_rx_semaphore, &xHigherPriorityTaskWoken);
//			        // -------- 判断xHigherPriorityTaskWoken的值 --------
//        if (xHigherPriorityTaskWoken == pdTRUE) {
//            // 有更高优先级任务被唤醒（需执行任务切换）
//            printf("+++++++++++++++++++++++++++++++++！\n");
//        } else {
//            // 无更高优先级任务被唤醒（无需额外调度）
//            printf("_________________________________\n");
//        }
		}
	}
    // 若释放信号量后，有更高优先级任务就绪，则立即切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


uint8_t beep_alert_flag = 0;// 蜂鸣器报警标志

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) { 
        uint8_t cmd = USART_ReceiveData(USART2); // 读取语音指令
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // 从ISR向队列发送指令（使用xQueueSendFromISR，非阻塞）
        if (xASRQueue != NULL) {
            xQueueSendFromISR(xASRQueue, &cmd, &xHigherPriorityTaskWoken);
            // 若有更高优先级任务被唤醒，触发任务切换
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清除中断标志
    }
}
/*
void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) { 
        uint8_t cmd = USART_ReceiveData(USART2); // 读取语音模块指令
        
        switch (cmd) {
            // ==================== 灯光控制 ====================
            case 0x01: // 打开卧室灯（PA0）
                drv_led_open(); 
                break;
            case 0x02: // 关闭卧室灯（PA0）
                drv_led_close(); 
                break;

            // ====================  servo/马达动作 ====================
            case 0x03: // 起立（PA1 高电平）
                dog_stand();
                break;
            case 0x04: // 趴下（PA1 低电平）
                dog_sitdown();
                break;
            case 0x05: // 前进（PA2 高电平）
                dog_forward();
                break;
            case 0x06: // 后退（PA2 低电平）
                dog_backup();
                break;
            case 0x07: // 俯身（PA3 高电平）
              	dog_bow();
                break;
            case 0x08: // 抬头（PA3 低电平）
                dog_lift_head();
                break;

            // ==================== 温湿度显示 ====================
            case 0x09: // 显示室温 → 触发更新
//								{key1_flag=0;	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
//	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);}
							
                break;
            case 0x0A: // 显示湿度 → 切换模式
//                {key1_flag=1;TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
//	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);}
                break;

            // ==================== 报警/风扇/马达 ====================
            case 0x0B: // 拨打119 → 蜂鸣器报警（5秒）
                drv_beep_open();
                break;
            case 0x0C: // 打开风扇
                drv_fan_open(); 
                break;
            case 0x0D: // 关闭风扇
                drv_fan_close(); 
                break;
            case 0x0E: // 打开马达
                drv_motor_open(); 
                break;
            case 0x0F: // 关闭马达
                drv_motor_close(); 
                break;

            // ==================== 未知指令 ====================
            default: 
                // 可选：打印未知指令
                break;
        }
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清除中断标志
    }
}

*/

////全局变量：当前扫描的位索引
//volatile uint8_t seg_index = 0;


//void TIM6_DAC_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
//			TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

//			// 轮流显示4位
//			switch(seg_index) {
//					case 0: drv_seg_display(bit_code[0], data_code[disp_n1]); break;
//					case 1: drv_seg_display(bit_code[1], data_code[disp_n2] | data_code[16]); break;
//					case 2: drv_seg_display(bit_code[2], data_code[disp_n3]); break;
//					case 3: drv_seg_display(bit_code[3], data_code[disp_n4]); break;
//			}
//			seg_index = (seg_index + 1) % 4;  // 循环切换
//}
//}

//void TIM7_IRQHandler(void)
//{
//if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
//{
//	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
//	sht20_getdata();
//	drv_sht20_update(sht20_data);
//}
//}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
