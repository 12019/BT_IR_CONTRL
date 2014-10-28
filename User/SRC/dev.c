#include "myconfig.h"
#include "string.h"
#include <stdio.h>

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
	{
//  使用外部HSE
//	ErrorStatus HSEStartUpStatus;
// 
//	/* RCC system reset(for debug purpose) */
//	RCC_DeInit();
//	
//	/* Enable HSE */
//	RCC_HSEConfig(RCC_HSE_ON);
//	
//	/* Wait till HSE is ready */
//	HSEStartUpStatus = RCC_WaitForHSEStartUp();
//			
//	if(HSEStartUpStatus == SUCCESS)									
//		{
//		/* Enable Prefetch Buffer */
//		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
//		
//		/* Flash 2 wait state */
//		FLASH_SetLatency(FLASH_Latency_2);
//		
//		/* HCLK = SYSCLK */
//		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
//		
//		/* PCLK2 = HCLK */
//		RCC_PCLK2Config(RCC_HCLK_Div1); 
//		
//		/* PCLK1 = HCLK/2 */
//		RCC_PCLK1Config(RCC_HCLK_Div2);
//		
//		/* PLLCLK = 8MHz * 3 = 24 MHz */
//		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_3);
//		
//		/* Enable PLL */ 
//		RCC_PLLCmd(ENABLE);
//		
//		/* Wait till PLL is ready */
//		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//			{
//			}
//		
//		/* Select PLL as system clock source */
//		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
//		
//		/* Wait till PLL is used as system clock source */
//		while(RCC_GetSYSCLKSource() != 0x08)
//			{
//			}
//		}

	//使用内部HSI	
	/* Enable HSI */
	RCC_HSICmd(ENABLE);	
	/* Wait till HSI is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
	{}	
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	
	/* Flash 2 wait state */
	FLASH_SetLatency(FLASH_Latency_2);
	
	/* HCLK = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	
	/* PCLK2 = HCLK */
	RCC_PCLK2Config(RCC_HCLK_Div1); 
	
	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config(RCC_HCLK_Div2);

	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_6);  //使用内部HSI	
	/* Enable PLL */ 
	RCC_PLLCmd(ENABLE);
	
	/* Wait till PLL is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{}
	
	/* Select PLL as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	/* Wait till PLL is used as system clock source */
	while(RCC_GetSYSCLKSource() != 0x08)
	{}

	/* Enable PWR and BKP clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* Enable write access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);

	/* Enable GPIOA clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	/* Enable GPIOB clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	/* Enable AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* ADC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	/* Enable USART1 USART2 USART3 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
	{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	#endif
	
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
			
	/* Enable the USART3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
		/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
		/* Enable the TIM3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//  /* Configure and enable ADC interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
	{
	GPIO_InitTypeDef GPIO_InitStructure;
		
			/* Enable GPIOA clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	/* Enable GPIOB clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		

	/*********Configure USART3 as UART****/		
	/* Configure USART3 Rx (PB.11) as input floating  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Tx (PB.10) as alternate function open-drain */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	/*Configuration PA1 (TIM2 channel 2) as alternate function push-pull */ //PA1 PWM 输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

/*Configuration PA2(IR_TXD)  as alternate push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*Configuration PB13(PWR_BL)  as alternate push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);//关BL IR ESAM电源	
	
		/* Configure PA04(BT_SET) as alternate push-pull  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	
	/*Configuration PA00(LED-RUN) PA12(B_RE) as alternate push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_12);//BL RESET HI
	
	/* Configure PA06(LED1) PA07(LED2) as input floating  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RUN_ON();
	}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(void)
	{
	USART_InitTypeDef USART_InitStructure;

		/* USART1 USART3 configuration ------------------------------------------------------*/
	/* USART1 USART3 configured as follow:
	    - BaudRate = 115200 baud  
	    - Word Length = 9 Bits
	    - One Stop Bit
	    - Even parity
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	/* Configure USART1 */
	USART_InitStructure.USART_BaudRate = 1200;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* Configure USART3*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	/* Enable the USART1 USART3*/
//	USART_Cmd(USART1, ENABLE); 
	USART_Cmd(USART3, ENABLE); 

	/* Enable USART1 Receive and Transmit interrupts */
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* Enable USART3 Receive and Transmit interrupts */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	}
	
	/*******************************************************************************
* Function Name  : ISO7816_Enable
* Description    : Enable the ISO7816.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ISO7816_Enable(void)
	{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;	

	/* USART2  configuration -------------------------------------------------------*/
	/* USART2 configured as follow:
	    - Word Length = 9 Bits
	    - 0.5 Stop Bit
	    - Even parity
	    - BaudRate = 12096 baud
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Tx and Rx enabled
	    - USART Clock enabled
	    - USART CPOL Low
	    - USART CPHA on first edge
	    - USART Last Bit Clock Enabled
	*/
	
	/* USART Clock set to 3MHz (PCLK1 = 12 MHZ / 4) */
	USART_SetPrescaler(USART2, 0x02);
	/* USART Guard Time set to 2 Bit */
	USART_SetGuardTime(USART2, 0x2);
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockInit(USART2, &USART_ClockInitStructure);	
	
	USART_InitStructure.USART_BaudRate = 8064;//12096;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &USART_InitStructure);  
	
	/* Enable the USART Parity Error Interrupt */
	USART_ITConfig(USART2, USART_IT_PE, ENABLE);

	/* Enable USART2 Receive and Transmit Interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* Enable USART2  */
	USART_Cmd(USART2, ENABLE);
	
	/* Enable the NACK Transmission */
	USART_SmartCardNACKCmd(USART2, ENABLE);	
	
	/* Enable the Smartcard Interface */
	USART_SmartCardCmd(USART2, ENABLE);	
	}
	
	/*******************************************************************************
* Function Name  : ISO7816_Disable
* Description    : Disable the ISO7816.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ISO7816_Disable(void)
	{
//	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;	

	/* USART2  configuration -------------------------------------------------------*/
	/* USART2 configured as follow:
	    - Word Length = 9 Bits
	    - 0.5 Stop Bit
	    - Even parity
	    - BaudRate = 12096 baud
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Tx and Rx enabled
	    - USART Clock enabled
	    - USART CPOL Low
	    - USART CPHA on first edge
	    - USART Last Bit Clock Enabled
	*/
	
	/* USART Clock set to 3MHz (PCLK1 = 12 MHZ / 4) */
	USART_SetPrescaler(USART2, 0x02);
	/* USART Guard Time set to 2 Bit */
	USART_SetGuardTime(USART2, 0x2);
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInit(USART2, &USART_ClockInitStructure);	
	/* Disable the USART Parity Error Interrupt */
	USART_ITConfig(USART2, USART_IT_PE, DISABLE);

	
	/* Disable USART2 Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

	/* Disable USART2  */
	USART_Cmd(USART2, DISABLE);
	
	/* Disable the NACK Transmission */
	USART_SmartCardNACKCmd(USART2, DISABLE);	
	
	/* Disable the Smartcard Interface */
	USART_SmartCardCmd(USART2, DISABLE);	
	}

void SetUartState(u8 COM, u32 BaudRate,u16 Parity)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	
	if(Parity == NOPARITY)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;		
	}
	else
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	}
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
	switch(COM)
		{
			case(COM1)://485
			{
				/* Disable USART1 */
				USART_Cmd(USART1, DISABLE);	
				/* Configure USART1 */
				USART_Init(USART1, &USART_InitStructure);
				/* Enable USART1 */
				USART_Cmd(USART1, ENABLE);
				
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
				break;						
			}
		case(COM2)://ESAM
			{
				/* Dsable USART2 */
				USART_Cmd(USART2, DISABLE);	
				/* Configure USART2 */
				USART_Init(USART2, &USART_InitStructure);
				/* Enable USART2 */
				USART_Cmd(USART2, ENABLE);
				
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				break;						
			}
		case(COM3)://BL
			{
				/* Dsable USART3 */
				USART_Cmd(USART3, DISABLE);	
				/* Configure USART3 */
				USART_Init(USART3, &USART_InitStructure);
				/* Enable USART3 */
				USART_Cmd(USART3, ENABLE);
				
				USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
				break;						
			}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : USART send_Configuration
* Description    : Configures the USART send.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1send(u8 *buffer,u16 len)
{
	if(len > 0)
	{
		TxCounter1 = 0;
		MaxNbrofTx1 = len;
		TxBuffer1 = buffer;
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		while(TxCounter1 != MaxNbrofTx1);			
	}	
}
void USART2send(u8 *buffer,u16 len)
{
	if(len > 0)
	{
		TxCounter2 = 0;
		MaxNbrofTx2 = len;
		TxBuffer2 = buffer;
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		while(TxCounter2 != MaxNbrofTx2);
	}	
}
	
void USART3send(u8 *buffer,u16 len)
{
	if(len > 0)
	{		
		TxCounter3 = 0;
		MaxNbrofTx3 = len;
		TxBuffer3 = buffer;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		while(TxCounter3 < MaxNbrofTx3);
	}	
}
	
	
/*******************************************************************************
* Function Name  : TIM_Configuration
* Description    : Configures the TIM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_Configuration(void)
	{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
		/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		/* ---------------------------------------------------------------
	TIM2 Configuration: Output Compare Inactive Mode:
	TIM2CLK = 24 MHz, Prescaler = 631, TIM2 counter clock = 38.034KHz
	--------------------------------------------------------------- */ 	
	/* Time base configuration */ //PA1 PWM OUT
	TIM_TimeBaseStructure.TIM_Period = 631; //38.014K
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		/* Output Compare Active Mode configuration: Channel2 */
	// PWM1 Mode configuration: Channel2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 315;	//CCR1_Val;	//50% 472
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//TIM_OCPolarity_High ;//	//比较前(后)的电平状态
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	// TIM2 enable counter
  TIM_Cmd(TIM2, DISABLE);//TIM_Cmd(TIM2, ENABLE);
		
	/* ---------------------------------------------------------------
	TIM3 Configuration: Output Compare Inactive Mode:
	TIM3CLK = 24 MHz, Prescaler = 23999, TIM3 counter clock = 1 KHz
	--------------------------------------------------------------- */ 	
	/* Time base configuration */ //10ms 
	TIM_TimeBaseStructure.TIM_Period = 19;
	TIM_TimeBaseStructure.TIM_Prescaler = 11999;//23999 max=65535
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//0
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//ENABLE DISABLE
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);	
	}

void TIM2_Configuration_56K(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
		/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		/* ---------------------------------------------------------------
	TIM2 Configuration: Output Compare Inactive Mode:
	TIM2CLK = 24 MHz, Prescaler = 631, TIM2 counter clock = 38.034KHz
	--------------------------------------------------------------- */ 	
	/* Time base configuration */ //PA1 PWM OUT
	TIM_TimeBaseStructure.TIM_Period = 428; //38.014K
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		/* Output Compare Active Mode configuration: Channel2 */
	// PWM1 Mode configuration: Channel2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 214;	//CCR1_Val;	//50% 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//TIM_OCPolarity_High ;//	//比较前(后)的电平状态
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	// TIM2 enable counter
  TIM_Cmd(TIM2, DISABLE);//TIM_Cmd(TIM2, ENABLE);
}

void PWM_Disable(void)
{
	TIM_Cmd(TIM2, DISABLE);
}
void PWM_Enable(void)
{
	TIM_Cmd(TIM2, ENABLE);
}

/*******************************************************************************
* Function Name  : IWDG_Configuration
* Description    : Configures the IWDG.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IWDG_Configuration(void)
{
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);
	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{}
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* IWDG counter clock: 40KHz(LSI) / 256 = 156 Hz */
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	/* Set counter reload value to 1999 */
	IWDG_SetReload(1999);
	
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();	
}

void IR_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //PA.6 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.6
	GPIO_SetBits(GPIOA,GPIO_Pin_6);						 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			  //浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				
	GPIO_SetBits(GPIOA,GPIO_Pin_5); 
   
	//GPIOA.5 中断线以及中断初始化配置   下降沿触发
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);

	EXTI_InitStructure.EXTI_Line=EXTI_Line5;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);				 
}

void EXTI9_5_DISABLE(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

void EXTI9_5_ENABLE(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

void Check_Rst(void)
{
	/* Check if the Power On Reset flag is set */
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)//上电复位检测
	{
		/* Clear reset flags */
    RCC_ClearFlag();
		IWDG_Configuration();
		BKP_WriteBackupRegister(BKP_DR5,IWDG_RUN);
	}
	else if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)//看门狗复位检测
	{
		/* Clear reset flags */
    RCC_ClearFlag();
		IWDG_Configuration();
		if(BKP_ReadBackupRegister(BKP_DR5) == IWDG_SLEEP)
		{
			PowerDown();
		}
	}
	else//按键复位
	{
		/* Clear reset flags */
    RCC_ClearFlag();
		IWDG_Configuration();		
		BKP_WriteBackupRegister(BKP_DR5,NORMAL);
	}
}

/*******************************************************************************
* Function Name  : SysTick_Configuration
* Description    : Configures the SysTick.Configures the SysTick to generate an interrupt each 1 millisecond.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Configuration(void)
{
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock/100000))
  { 
    /* Capture error */ 
    while (1);
  }
  /* Set SysTick Priority to  */
  NVIC_SetPriority(SysTick_IRQn, 0x04);
}

u8 Hex2Bcd(u8 b)
{
	u8 bcd ;

	bcd = ((b/10)<<4) | (b%10) ;

	return bcd ;
}

u8 Bcd2Hex(u8 b)
{
	u8 hex ;

	hex = (((b&0xf0)>>4)*10) + (b&0x0f) ;

	return hex ;
}

void GetBuildTime(void)
{
	char Date[] = __DATE__;
	char Time[] = __TIME__;
	char mon_s[20]={0};
	char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	int mon=0, day=0, year=0;
	int sec=0, min=0, hour=0;
	sscanf(Date, "%s %d %d", mon_s, &day, &year);
	sscanf(Time, "%d:%d:%d", &hour, &min, &sec);	
	mon = (strstr(month_names, mon_s)-month_names)/3+1;
	BuildTime.date = Hex2Bcd((unsigned char)day);
	BuildTime.mon = Hex2Bcd((unsigned char)mon);
	BuildTime.yearh = 0x20;//Hex2Bcd(0x14);
	BuildTime.yearl = Hex2Bcd((unsigned char)(year%100));	
	BuildTime.hour = Hex2Bcd((unsigned char)hour);
	BuildTime.min = Hex2Bcd((unsigned char)min);
	BuildTime.sec = Hex2Bcd((unsigned char)sec);
}

void Set_IRDA_power_ON(void)
{
//	IR_GPIO_Init();
	PWM_Enable();
	PWR_IR_ON();
	TX_FLAG = 0;
	RX_FLAG = 0;
	POW_IR = 1;
}
void Set_IRDA_power_OFF(void)
{
//	EXTI9_5_DISABLE();
	PWM_Disable();
	PWR_IR_OFF();
	POW_IR = 0;
}

void Set_RS485_power_ON(void)
{	
//	PWR_485_ON();
	POW_RS485 = 1;
}
void Set_RS485_power_OFF(void)
{	
//	PWR_485_OFF();	
	POW_RS485 = 0;
}
void Set_ESAM_power_ON(void)
{
	ISO7816_Enable();
//	PWR_ESAM_ON();	
	POW_ESAM = 1;
	ESAM_Reset();
}
void Set_ESAM_power_OFF(void)
{
	ISO7816_Disable();
//	PWR_ESAM_OFF();
	POW_ESAM =0;
}




