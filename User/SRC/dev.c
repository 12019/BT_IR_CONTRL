
#include "myconfig.h"
/*******************************************************************************
* Function Name  : Quque_Configuration
* Description    : Configures the Quque.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitQue(Quque *q)
{
	u16 i;
	q->head = 0;
	q->tail = 0;

	for(i=0;i<MAXOFRXQ;i++)
	{
		q->elem[i] = 0x00;
	}
}

u8 IsFull(Quque *q)
	{
	if(q->head==(q->tail+1)%MAXOFRXQ)  
		return QUEFULL;
	else
		return 0;	
	}
u8 InsertQue(Quque *q,u8 elem)
	{
	if(IsFull(q) == QUEFULL) 
		return QUEFULL;
	q->elem[q->tail]=elem;
	q->tail=(q->tail+1)%MAXOFRXQ; 
	return QUESUC;
	}
u8 IsEmpty(Quque *q)
	{
	if(q->head==q->tail) //??
		return QUEEMP;
	else 
		return QUESUC;
	}
u8 OutQue(Quque *q,u8 *pelem,u16 len)
	{
	u16 i;
	if(IsEmpty(q) == QUEEMP)
		return QUEEMP;
	if(q->head < q->tail)
		{
		if((q->tail-q->head)<len)
			return QUENOEGH;
		else
			{
			for(i=0;i<len;i++)
				{
				pelem[i] = q->elem[q->head + i];
				}
			q->head =(q->head+len)%MAXOFRXQ;
			}
		}
	if(q->head > q->tail)
		{
		if((MAXOFRXQ - q->head + q->tail )<len)
			return QUENOEGH;
		else
			{
			if((MAXOFRXQ - q->head)>len)
				{
				for(i=0;i<len;i++)
					{
					pelem[i] = q->elem[q->head + i];
					}
				q->head =(q->head+len)%MAXOFRXQ;
				}
			else
				{
				for(i=0;i<(MAXOFRXQ-q->head);i++)
					{
					pelem[i] = q->elem[q->head + i];
					}				
				for(i=(MAXOFRXQ-q->head);i<len;i++ )
					{
					pelem[i] = q->elem[(q->head + i)%MAXOFRXQ];
					}
				q->head =(q->head+len)%MAXOFRXQ;
				}
			}
		}
	return QUESUC;
	}
u8 AllOutQue(Quque *q,u8 *pelem)
	{
	u16 i;
	if(IsEmpty(q) == QUEEMP)
		return QUEEMP;
	if(q->head > q->tail)
		{
		for(i=0;i<(MAXOFRXQ - q->head + q->tail);i++)
			{
			pelem[i] = q->elem[q->head + i];
			}				
		InitQue(q);
		}
	if(q->head < q->tail)
		{
		for(i=0;i<(q->tail - q->head);i++)
			{
			pelem[i] = q->elem[q->head + i];
			}
		InitQue(q);
		}		
	return QUESUC;
	}
u16 NumOfQue(Quque *q)
	{
	u16 Re;
	if(IsEmpty(q) == QUEEMP)
		Re = 0;
	else if(q->head > q->tail)
		{
		Re = MAXOFRXQ - q->head + q->tail;
		}
	else if(q->head < q->tail)
		{
		Re = q->tail - q->head;
		}
	return Re;
	}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
	{
//	ErrorStatus HSEStartUpStatus;
 
	/* RCC system reset(for debug purpose) */
//	RCC_DeInit();
	
	/* Enable HSE */
//	RCC_HSEConfig(RCC_HSE_ON);
	/* Wait till HSE is ready */
//	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	//使用内部HSI	
	/* Enable HSI */
	RCC_HSICmd(ENABLE);	
	/* Wait till HSI is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
		{
		}	
//	if(HSEStartUpStatus == SUCCESS)									//使用外部HSE
		{
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
		
		/* PLLCLK = 8MHz * 3 = 24 MHz */
//		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_3);//使用外部HSE
		
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_6);  //使用内部HSI	
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			{
			}
		
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
			{
			}
		}
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

	/* Enable USART1 clock */  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Enable USART2 USART3 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* ADC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* TIM3 clock enable */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	



	/* Enable the USART3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
  /* Configure and enable ADC interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
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
		
	/*********Configure USART1 as UART****/		
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	 	

	/*********Configure USART2 as UART****/		

	/* Configure USART2 Rx (PA.3) as input floating  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Tx (PA.2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

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

	/*Configuration PA1 (TIM2 channel 2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*Configuration PB05(PWR_OT) PB07(PWR_IR) PB12(B_SET) as alternate push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*Configuration PA00(LED-RUN) PA04(PWR_BL) PA05(B_RE) as alternate push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PA.06(LED1) PA.07(LED2) as input floating  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	RUN_ON();
	}


/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : Configures the different ADC ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Configuration(void)
{
	{
	ADC_InitTypeDef ADC_InitStructure;
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel1 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

	/* Enable ADC1 EOC interupt */
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);	

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	 
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	}
}
u16 ADC_Getvalue(void)
{
	u32 temp = 0;
	u16 max = 0,min = 0;
	u8 i;
	u8 tmp1,tmp2;
	while(ADCTure != 1);
	for(i=0;i<ADC_TIMES;i++)
	{
//		ADCConvertedValue[i] = ADC_GetConversionValue(ADC1);
		if(ADCConvertedValue[i] > max) max = ADCConvertedValue[i];
		if(min < ADCConvertedValue[i]) min = ADCConvertedValue[i];
		temp += (u32)ADCConvertedValue[i];
	}
	ADCTure = 0;
	temp = temp - max - min;
	temp = temp /(ADC_TIMES -2);
	if(temp > VBAT_MAX)
	{
		tmp1 = 99;
		tmp2 = 99;
		temp = (Hex2Bcd(tmp1)<<8) | Hex2Bcd(tmp2);
	}	
	else if(temp < VBAT_MIN)
	{
		temp = 0x0;
		Bat_Low = 1;
	}
	else
	{
		temp = (temp*100-VBAT_MIN)/VBAT_USE;
		tmp1 = temp%100;
		tmp2 = temp/100;
		temp = 0;
		temp = (Hex2Bcd(tmp1)<<8) | Hex2Bcd(tmp2);
	}
	return (u16)temp;
}
/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
	{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Reset Backup Domain */
//	BKP_DeInit();
  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
	/* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}	
	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(40000);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* To output second signal on Tamper pin, the tamper functionality
	   must be disabled (by default this functionality is disabled) */
	BKP_TamperPinCmd(DISABLE);
	
	/* Enable the RTC Second Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
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

		/* USART2 USART3 configuration ------------------------------------------------------*/
	/* USART2 USART3 configured as follow:
	    - BaudRate = 9600 baud  
	    - Word Length = 9 Bits
	    - One Stop Bit
	    - Even parity
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	/* Configure USART2 */
	USART_Init(USART1, &USART_InitStructure);
		
	USART_InitStructure.USART_BaudRate = 1200;
	USART_InitStructure.USART_Parity = USART_Parity_Even;		
	USART_Init(USART2, &USART_InitStructure);
	/* Configure USART3*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_Parity = USART_Parity_No;		
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
	USART_Init(USART3, &USART_InitStructure);

	/* Enable the USART2 USART3*/
	USART_Cmd(USART1, ENABLE); 
	USART_Cmd(USART2, ENABLE); 
	USART_Cmd(USART3, ENABLE); 

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* Enable USART2 Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	/* Enable USART3 Receive and Transmit interrupts */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
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
			case(COM1):
			{
				/* Dsable USART1 */
				USART_Cmd(USART1, DISABLE);	
				/* Configure USART1 */
				USART_Init(USART1, &USART_InitStructure);
				/* Enable USART1 */
				USART_Cmd(USART1, ENABLE);
				break;						
			}
		case(COM2):
			{
				/* Dsable USART2 */
				USART_Cmd(USART2, DISABLE);	
				/* Configure USART2 */
				USART_Init(USART2, &USART_InitStructure);
				/* Enable USART2 */
				USART_Cmd(USART2, ENABLE);
				break;						
			}
		case(COM3):
			{
				/* Dsable USART3 */
				USART_Cmd(USART3, DISABLE);	
				/* Configure USART3 */
				USART_Init(USART3, &USART_InitStructure);
				/* Enable USART3 */
				USART_Cmd(USART3, ENABLE);
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
void UARTsend(u8 ch)
{
	USART_SendData(USART1, ch);
}
void USART1send(Quque *q)
{
	u8 ch,i;
	if(COMTXON == Com3first)
	{
		Com3first = COMNFIR;
		PWR_OT_ON();
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
	i = OutQue(&RxQUE3,&ch,1);
	if(i == QUESUC)
	{		
		iRDAsend(ch);
	}		
	else
	{
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		InitQue(&RxQUE3);
		Com3first = COMFIR;
		time_send = 0;
		time_sleep = 0;		
	}
}
void iRDAsend(u8 ch)
{
//	PWR_IR_ON();
//	PWM_Enable();
	USART_SendData(USART2, ch);
//	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
//	PWM_Disable();
}
void USART2send(Quque *q)
{
	u8 ch,i;
	if(COMTXON == Com3first)
	{
		Com3first = COMNFIR;
		PWR_IR_ON();
		PWM_Enable();
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
	i = OutQue(&RxQUE3,&ch,1);
	if(i == QUESUC)
	{		
		iRDAsend(ch);
	}		
	else
	{
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		InitQue(&RxQUE3);
		Com3first = COMFIR;
		time_send = 0;
		time_sleep = 0;		
	}
}
void USART3send(u8 ch)
	{
//	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  USART_SendData(USART3, ch);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);			
	}
void BLsend(u8 *buffer,u16 len)
{
	TxCounter3 = 0;
	MaxNbrofTx3 = len;
	TxBuffer3 = buffer;
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	while(TxCounter3 != MaxNbrofTx3);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
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
	/* ---------------------------------------------------------------
	TIM2 Configuration: Output Compare Inactive Mode:
	TIM2CLK = 24 MHz, Prescaler = 631, TIM2 counter clock = 38.034KHz
	--------------------------------------------------------------- */ 	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 631; //38.014K
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* Output Compare Active Mode configuration: Channel2 */
	// PWM1 Mode configuration: Channel2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 315;	//CCR1_Val;	//50%
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//TIM_OCPolarity_High ;//	//比较前(后)的电平状态
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);

	// TIM2 enable counter
  TIM_Cmd(TIM2, ENABLE);
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
	
	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
	IWDG_SetPrescaler(IWDG_Prescaler_128);
	
	/* Set counter reload value to 999 */
	IWDG_SetReload(999);
	
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();	
}

void Check_Rst(void)
{
	/* Check if the Power On Reset flag is set */
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) 
	{
		/* Clear reset flags */
    RCC_ClearFlag();
		IWDG_Configuration();
		PowerOn = RST_PWRON;
		BKP_WriteBackupRegister(BKP_DR5,IWDG_RUN);
	}
	else if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* Clear reset flags */
    RCC_ClearFlag();
		IWDG_Configuration();
		PowerOn = RST_IWDG;
		if(BKP_ReadBackupRegister(BKP_DR5) == IWDG_SLEEP)
		{
			PowerDown();
		}
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
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
  /* Set SysTick Priority to 3 */
  NVIC_SetPriority(SysTick_IRQn, 0x0C);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
#ifdef  IWDGSW	
	IWDG_ReloadCounter();
#endif	
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
