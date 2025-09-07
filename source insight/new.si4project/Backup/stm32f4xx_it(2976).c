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
extern uint8_t key1_flag;
extern uint8_t key2_flag;
extern uint8_t key3_flag;
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9))
	{
		drv_systick_ms(20);
		if(EXTI_GetITStatus(EXTI_Line9))
		{
			EXTI_ClearITPendingBit(EXTI_Line9);
			key1_flag^=1;
		}
		
	}
	if(EXTI_GetITStatus(EXTI_Line8))
	{
		drv_systick_ms(20);
		if(EXTI_GetITStatus(EXTI_Line8))
		{
			EXTI_ClearITPendingBit(EXTI_Line8);
			key2_flag^=1;
		}
	}
	if(EXTI_GetITStatus(EXTI_Line5))
	{
		drv_systick_ms(20);
		if(EXTI_GetITStatus(EXTI_Line5))
		{
			EXTI_ClearITPendingBit(EXTI_Line5);
			key3_flag^=1;
		}

	}

} 

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
        if(i6 < sizeof(rcbuffer6) - 1)
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
            //rx_complete = 1;    // 设置接收完成标志
            
            
            // 重置缓冲区索引
            i6 = 0;
		}
	}
}
void USART3_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE)!=RESET)
	{
		USART_SendData(USART6, USART_ReceiveData(USART3));//发送数据给USART6相当于发送给串口助手
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		
	}
}





void TIM6_DAC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		GPIO_ToggleBits(GPIOC, GPIO_Pin_4);
	}
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
