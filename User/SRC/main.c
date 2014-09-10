/* Includes ------------------------------------------------------------------*/
#define root
#include "myconfig.h"
#include "ESAM.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
extern __IO uint16_t ADC_ConvertedValue;
	
int main(void)
{			
	vu16 Convert = 0;
	u8 i = 0;
	u8 BLset_command[] = {"AT+UART=115200, 0,2\r\n"};
	u8 IrData = 0,Rx485 = 0,Tx485 = 0;
	
	/* System Clocks Configuration */
	RCC_Configuration();
	
#ifdef  IWDGSW
	Check_Rst();
#endif
	
	/* Configure the GPIO ports */
	GPIO_Configuration();
	/*USART configuration*/
	USART_Configuration();
	/*7816 configuration*/
	ISO7816_Enable();
	/*TIM configuration*/
	TIM_Configuration();
	/* NVIC configuration */
	NVIC_Configuration(); 		
  /* Configure the SysTick to generate an interrupt each 10 millisecond */
  SysTick_Configuration();
	/*ADC configuration*/
	myADC_init();	
	
/*********************BLSET***************************/	
if(0 == LoadFlash())//未设置BL
{
	PWR_BL_ON();
	AT_FLAG = 0;
	IWDG_ReloadCounter();
	Clear_RxBuffer3();
	SetUartState(COM3,CBR_38400,USART_Parity_No);//BL 38400 1停 无
	Set_BL_Enter();//进入AT模式
	delay_nms(1000);
	USART3send(BLset_command,sizeof(BLset_command)-1);//设置115200 1停 偶
	while(!OK)//'OK' 
	{
		delay_nms(10);
		i++;
		if(i>=30)//BL设置失败 超过3s超时退出
		{
		NVIC_SystemReset();
		}
	}
	WriteFLAG();//设置完成
	NVIC_SystemReset();
}
/*****************************************************/	
	
 	PowerOff();
	
	Clear_Flag = 0;
	GET_ESAM = 0;//默认关闭ESAM数据获取
	IrTimeBegin = 0;
	Buf_Flag = 1;//默认IR_Buf1接收
	IrRxCounter1 = 0;
	IrRxCounter2 = 0;
	Buf1_FULL = 0;
	Buf2_FULL = 0;
	POW_IR = 1;
	OK = 0;
	Ver_flag = 0;
	IR_to_BL = 0;
	BL_STA = 0;
	BL_TIME = 0;
	Bat_Low = 0;
	Bat_FLAG = 0;
//	ADC_time = 0;
	POW_TIME = 0;
	BL_REQ_FLAG = 0;//读参数应答数据帧标志
	TX_FLAG = 0;
	RX_FLAG = 0;
	IR_TC = 0;
	Rs485_TC = 0;
	IR_DB_RZ = 0;
	ZF_LEN = 1;
	Usart3_Wtime = 0;
	IR_Wtime = 0;
	time_sleep = 0;
	sysread = 0;
	W_Mode = BLMODE;
	W_Time = 10;//初始化等待休眠时间 分钟
	IR_BaudRate_Time = 83;//IR BaudRate
	
	InitQue(&RxQUE1);//485接收队列
//	InitQue(&RxQUE2);//红外接收队列
	InitQue(&RxQUE3);//蓝牙接收队列
	
	Clear_IrRxBuffer1();
	Clear_IrRxBuffer2();

//	Clear_RxBuffer1();
	Clear_RxBuffer2();
	Clear_RxBuffer3();
	
	ESAM_Info();
	W_Bat = ADC_filter();
	
	PowerUp();//BL等待配对	
	delay_nms(100);
	Req_BL();//请求蓝牙数据
	
  while (1)
  {
		IWDG_ReloadCounter();		

		if(IR_TC == 1)//红外通道透传
		{
			while(IsEmpty(&RxQUE3) != QUEEMP)//队列非空
			{
				OutQueOneByte(&RxQUE3,&IrData);
				if(Clear_Flag)
				{
					Clear_Flag = 0;
					IrRxCounter1 = 0;
					IrRxCounter2 = 0;
				}
				SendOneByte(IrData);
			}
		}
		else if(Rs485_TC == 1)//485通道透传
		{
			if(IsEmpty(&RxQUE3) != QUEEMP)//队列非空
			{
				OutQue(&RxQUE3,&Rx485,1);
				USART1send(&Rx485,1);//BL发给485
			}
			
			if(IsEmpty(&RxQUE1) != QUEEMP)//队列非空
			{
				OutQue(&RxQUE1,&Tx485,1);
				USART3send(&Tx485,1);//485数据透传给BL
			}
		}
		else//协议支持
		{
			if(Usart3_Wtime > Byte_Time)//蓝牙接收数据帧结束
			{
				Usart3_Wtime = 0;
				Usart3_EN = 0;
				
				if(TrasferMode == 3)//红外抄表
				{
					BL_Unpack(RxBuffer3,RxCounter3);//帧数据解包
				}
			}
		}
		
//if(IR_to_BL == 1)//数据回传
//{
//	if(IsEmpty(&RxQUE2) != QUEEMP)//队列非空
//	{
//		OutQue(&RxQUE2,&Tx_Data,1);
//		USART3send(&Tx_Data,1);//IR数据透传给BL
//	}
//}
		
		if(IR_to_BL == 1)//数据回传
		{
				if(Buf1_FULL)
				{
					USART3send(IrBuf1,MAXIRBUFLEN);
					Buf1_FULL = 0;
					Clear_IrRxBuffer1();
				}
				else if(Buf2_FULL)
				{
					USART3send(IrBuf2,MAXIRBUFLEN);
					Buf2_FULL = 0;
					Clear_IrRxBuffer2();
				}

					if(IR_Wtime >= Byte_Time_BL)
					{
						IrTimeBegin = 0;
						IR_Wtime = 0;
						
						if(Buf_Flag == 1)
						{
							USART3send(IrBuf1,IrRxCounter1);
							Clear_IrRxBuffer1();
						}
						else if(Buf_Flag == 2)
						{
							USART3send(IrBuf2,IrRxCounter2);
							Clear_IrRxBuffer2();
						}
					}
		}
		
//	if(QUE_TIME>=5000)//清队列
//	{
//		InitQue(&RxQUE1);//485接收队列
//	//	InitQue(&RxQUE2);//红外接收队列
//		InitQue(&RxQUE3);//蓝牙接收队列
//		QUE_TIME = 0;
//	}
		
	if(POW_TIME>=M_Time*1000)//关IR or 485
	{
		POW_TIME = 0;
		PWM_Disable();
		PWR_IR_OFF();
		PWR_485_OFF();
	}

//		if(ADC_time >= 5000)
//		{
//			ADC_time = 0;
//			ADC_filter();
//		}
	
		if(Bat_Low&&((W_Time*60*1000-time_sleep)>120000))//电量低且时间大于2MIN
		{
			Bat_Low = 0;
			W_Time = 2;
			time_sleep = 0;
		}
		
		if(time_sleep>=(W_Time*60*1000))//等待休眠时间 无数据超时进入睡眠模式 
		{
			time_sleep = 0;
			PowerDown();
		}
		
		if(LED2_0)//断开服务
		{
			NVIC_SystemReset();
		}
	}
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
