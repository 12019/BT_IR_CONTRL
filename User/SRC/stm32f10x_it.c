/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "myconfig.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
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
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
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
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay--;
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);
		
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		time_sleep ++;
		time_send ++;

		if(Bat_Low == 1)
		{
			RUN_ON();			
		}
		else
		{
			RUN_TOG();			
		}
		/* Reset RTC Counter when Time is 23:59:59 */
		if (RTC_GetCounter() == 0x00015180)
		{
			/* Change the current time */
			RTC_SetCounter(0);
			/* Wait until last write operation on RTC registers has finished */
			RTC_WaitForLastTask();
		}
	}
}
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{ 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		RxData1 = USART_ReceiveData(USART1);
		USART3send(RxData1);
	}  
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{   
		USART2send(&RxQUE3);
	}
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
	{
		if(COMFIR == Com3first)
		{
			USART_ITConfig(USART1, USART_IT_TC, DISABLE);
		}
	}	
}
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		RxData2 = USART_ReceiveData(USART2);
		if(LED2_0)
		{
			RxTmp2[Rx2len++] = RxData2;
		}
		else
		{
			USART3send(RxData2);
		}		
	}
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		/* Disable the USART2 Transmit interrupt */
		USART2send(&RxQUE3);
//		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		if(COMFIR == Com3first)
		{
			USART_ITConfig(USART2, USART_IT_TC, DISABLE);
//			PWR_IR_OFF();
			PWM_Disable();
		}
	}
	
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		InsertQue(&RxQUE3,USART_ReceiveData(USART3));
		if((COMFIR == Com3first)&&(W_Mode == IRDAMODE)&&(sysread == 1))
			{
				Com3first = COMTXON;
				USART_ITConfig(USART2, USART_IT_TXE, ENABLE);				
//			USART2send(&RxQUE3);	
			}
		else if((COMFIR == Com3first)&&(W_Mode == UARTMODE)&&(sysread == 1))
		{
			Com3first = COMTXON;
			USART1send(&RxQUE3);			
		}
	}  
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{   
		/* Write one byte to the transmit data register */
		USART_SendData(USART3, TxBuffer3[TxCounter3++]); 
		if(TxCounter3 >= MaxNbrofTx3)
	    {				
		    /* Disable the USART1 Transmit interrupt */
				USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	    }  
	}
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_IRQHandler(void)
{
	/* Get converted value */
	ADCConvertedValue[ADCcount] = ADC_GetConversionValue(ADC1);
	ADCcount++;
	if((ADCcount >= ADC_TIMES)&&(ADCTure == 0))
	{
		for(ADCcount=0;ADCcount<ADC_TIMES;ADCcount++)
		{
			ADCConverted[ADCcount] = ADCConvertedValue[ADCcount];
		}
		ADCcount = 0;
		ADCTure = 1;
	}
	else if(ADCcount >= ADC_TIMES)
	{
		ADCcount = 0;
	}
}
/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/