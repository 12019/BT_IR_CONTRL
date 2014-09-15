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

u8 n = 0;
u8 tmp = 0;
//u8 USART_RX_BUF[USART_REC_LEN]; 
//u16 USART_RX_STA=0;

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
//  u8 tmp;

	if(TX_FLAG)
	{
		CountTX++;
		if(Tx_bit == 0)//发送起始位
		{
		TXD_low(); 
		CountTX = 0;
		Tx_bit = 1;
		}
		else if((CountTX == IR_BaudRate_Time)&&(Tx_bit > 0)&&(Tx_bit < 9))//8bit数据
		{
			tmp	= (BYTE >> (Tx_bit - 1))& 0x01;
			if(tmp == 0)
			{
				TXD_low();		
			}
			else
			{
				TXD_high();
				Tx_Parity++;
			}
			CountTX = 0;
			Tx_bit++;
		}
		else if((CountTX == IR_BaudRate_Time)&&(Tx_bit == 9))//效验位
		{
			if(Tx_Parity%2 == 0)
			{
				TXD_low();	
			}
			else
			{
				TXD_high();
			}
			CountTX = 0;
			Tx_bit++;
		}
		else if((CountTX == IR_BaudRate_Time)&&(Tx_bit == 10))//停止位
		{
			TXD_high();
			CountTX = 0;
			Tx_bit++;
		}
		else if((CountTX == IR_BaudRate_Time)&&(Tx_bit == 11))
		{
		CountTX = 0;
		TX_FLAG = 0;
		Tx_bit = 0;
		}
	}
	
	if(RX_FLAG)
	{
		CountRX++;
		if((CountRX == (IR_BaudRate_Time/2))&&(Receive_bit == 0))
		{
			if(GPIOA->IDR&(1<<5)) 
			{
				RX_FLAG = 0;
				CountRX = 0;
				Receive_bit = 0;
				EXTI9_5_ENABLE();	
			}
			else//起始位
			{
				CountRX = 0;
				Receive_bit = 1;
				Rx_Parity = 0;
			}
		}			
		else if((CountRX == IR_BaudRate_Time)&&(Receive_bit > 0)&&(Receive_bit < 9))//8位数据
		{
			tmp_data >>= 1;		
			if(GPIOA->IDR&(1<<5)) 
			{
				tmp_data |=0x80;
				Rx_Parity++;
			}
			CountRX = 0;
			Receive_bit++;
		}
		else if((CountRX == IR_BaudRate_Time)&&(Receive_bit == 9))//效验位
		{
			Rx_Parity_tmp = GPIOA->IDR&(1<<5);
			CountRX = 0;
			Receive_bit++;
		}
		else if((CountRX == IR_BaudRate_Time)&&(Receive_bit == 10))//停止位
		{
			if(GPIOA->IDR&(1<<5)) 
			{
				if(((Rx_Parity%2 == 0)&&(Rx_Parity_tmp == 0))||((Rx_Parity%2 != 0)&&(Rx_Parity_tmp != 0)))
				{
					if(Ver_flag)
					{
						IrDARxBuffer1[IrDARxCounter++] = tmp_data;
					}
					else 
					{		
						IR_to_BL = 1;		
						InsertQue(&RxQUE2,tmp_data);
					}
				}
				RX_FLAG = 0;
				CountRX = 0;
				Receive_bit = 0;
				EXTI9_5_ENABLE();			
			}
		}
		IR_Wtime = 0;
		IrTimeBegin = 1;
	}	
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)//485
{ 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
			InsertQue(&RxQUE1, USART_ReceiveData(USART1));
	}
	
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{	
		/* Write one byte to the transmit data register */
				USART_SendData(USART1, TxBuffer1[TxCounter1++]); 
				if(TxCounter1 >= MaxNbrofTx1)
				{				
					/* Disable the USART1 Transmit interrupt */
					USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
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
void USART2_IRQHandler(void)//ESAM 协议支持
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		RxBuffer2[RxCounter2++] = USART_ReceiveData(USART2);
	}
	
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		/* Write one byte to the transmit data register */
				USART_SendData(USART2, TxBuffer2[TxCounter2++]); 
				if(TxCounter2 >= MaxNbrofTx2)
				{				
					/* Disable the USART1 Transmit interrupt */
					USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
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
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收寄存器满中断
	{
		time_sleep = 0;
		POW_TIME = 0;
		
		if(BL_REQ_FLAG == 1)//读参数应答第一帧数据
		{
		/* Read one byte from the receive data register */		
		InsertQue(&RxQUE3, USART_ReceiveData(USART3));	
		}	
		else if(BL_REQ_FLAG == 2)//非第一帧应答数据 
		{
			if(TrasferMode == 0)//透传字节转发
			{
				InsertQue(&RxQUE3,USART_ReceiveData(USART3));
				
				if(W_Mode == IRDAMODE)//红外通道透传
				{
					POW_IR = 1;
//					InsertQue(&RxQUE3,USART_ReceiveData(USART3));
				}
//				else if((W_Mode == UARTMODE)&&(sysread == 1))//485通道数据透传 USART1
//				{
//					InsertQue(&RxQUE3,USART_ReceiveData(USART3));	
//				}
			}
			else//协议支持  TrasferMode:03=红外抄表	04=ESAM
			{
					POW_IR = 1;
					
				/* Read one byte from the receive data register */
					RxBuffer3[RxCounter3++] = USART_ReceiveData(USART3);
					if(Usart3_EN == 0)
					{
						Usart3_EN = 1;
						Usart3_Wtime = 0;
					}
					else
					{
						Usart3_Wtime = 0;
					}
			}
		}
		
		if(AT_FLAG)//AT配置应答
		{
			RxBuffer3[RxCounter3++] = USART_ReceiveData(USART3);		
			
			if((RxBuffer3[RxCounter3-2] == 0x4f)&&(RxBuffer3[RxCounter3-1] == 0x4b))
			{
				OK = 1;
			}
			if(RxCounter3 == 4)
			{
				AT_FLAG = 0;
			}
		}	
		
	}  
	
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)//协议支持 透传在中断中处理
	{  
		time_sleep = 0;	
		
		USART_SendData(USART3, TxBuffer3[TxCounter3++]);
		
		if(TxCounter3 >= MaxNbrofTx3)
		{				
			/* Disable the USART1 Transmit interrupt */
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
	}
}

void TIM3_IRQHandler(void)//帧数据结束计时器
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		/* Clear TIM3 Update interrupt pending bit*/
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
			time_sleep++;
		
			POW_TIME++;
		
			if(BL_STA == 1)//BL未连接
			{
				BL_TIME++;
				if(BL_TIME == 200)
				{
					BL_TIME = 0;
					RUN_TOG();
				}
			}
			else if(BL_STA == 2)//BL已连接
			{
				BL_TIME++;
				if(BL_TIME == 1000)
				{
					BL_TIME = 0;
					RUN_TOG();
				}
			}
		
			if(Usart3_EN == 1)
			{
				Usart3_Wtime++;
			}	
			
			if(IrTimeBegin)
			{
				IR_Wtime++;
			}
	}
}

void EXTI9_5_IRQHandler(void)//PA5
{
   if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
	/* Disable the Selected IRQ Channels -------------------------------------*/
	//   	  NVIC->ICER[EXTI9_5_IRQn >> 0x05] =
	//      		(uint32_t)0x01 << (EXTI9_5_IRQn & (uint8_t)0x1F);
		EXTI_ClearITPendingBit(EXTI_Line5);
		if(TX_FLAG == 0)
		{
			RX_FLAG = 1;
			CountRX = 0;
			Receive_bit = 0;
			EXTI9_5_DISABLE();
		}
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
